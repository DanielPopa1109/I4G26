"""One-time, checked migration of the legacy hand-written RTE call sites.

The script refuses to edit files when it cannot account for every RTE token.
It preserves the existing access semantics while using the actual owner globals
and direct HAL/RTOS/component functions. Run from any working directory.
"""
from pathlib import Path
import re

APP = Path(__file__).resolve().parents[2] / "I4_BCC_WORKSPACE" / "I4_BCC"
HEADER = (APP / "RTE" / "RTE.h").read_text()
SOURCE = (APP / "RTE" / "RTE.c").read_text()

simple_aliases = {}
call_aliases = {}
for line in HEADER.splitlines():
    match = re.match(r"#define\s+(Rte_\w+|RTE_\w+)(\([^)]*\))?\s+(.+)", line)
    if not match:
        continue
    name, parameters, expansion = match.groups()
    if parameters:
        target = re.match(r"(\w+)\s*\(", expansion.strip())
        if target:
            call_aliases[name] = target.group(1)
    else:
        simple_aliases[name] = expansion.strip()

functions = {}
for name, params, body in re.findall(
    r"\bVOID\s+(Rte_\w+)\s*\(([^)]*)\)\s*\{([^{}]*)\}", SOURCE
):
    arguments = [p.strip().split()[-1].lstrip("*") for p in params.split(",") if p.strip()]
    functions[name] = (arguments, body.strip())

def split_args(raw):
    out, start, depth = [], 0, 0
    for index, char in enumerate(raw):
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
        elif char == "," and depth == 0:
            out.append(raw[start:index].strip())
            start = index + 1
    if raw[start:].strip():
        out.append(raw[start:].strip())
    return out

def substitute(expression, formal, actual):
    for name, value in zip(formal, actual):
        expression = re.sub(r"\*\s*\b" + re.escape(name) + r"\b", "*(" + value + ")", expression)
        expression = re.sub(r"\b" + re.escape(name) + r"\b", "(" + value + ")", expression)
    return expression

def replace_calls(content):
    names = set(re.findall(r"\bRte_\w+(?=\s*\()", content))
    for name in sorted(names, key=len, reverse=True):
        if name in call_aliases:
            content = re.sub(r"\b" + name + r"(?=\s*\()", call_aliases[name], content)
            continue
        if name not in functions:
            raise ValueError(f"Unmapped call {name}")
        formal, body = functions[name]
        direct = re.fullmatch(r"(\w+)\s*\(([^;]*)\);", body)
        if direct and split_args(direct.group(2)) == formal:
            content = re.sub(r"\b" + name + r"(?=\s*\()", direct.group(1), content)
            continue
        pattern = re.compile(r"\b" + name + r"\s*\(")
        while match := pattern.search(content):
            pos, depth = match.end(), 1
            while depth and pos < len(content):
                if content[pos] == "(": depth += 1
                if content[pos] == ")": depth -= 1
                pos += 1
            if depth:
                raise ValueError(f"Unclosed call {name}")
            actual = split_args(content[match.end():pos - 1])
            if len(actual) != len(formal):
                raise ValueError(f"Argument mismatch {name}: {actual}")
            if name == "Rte_Write_SenCtrl_SenCtrlPort_SenCtrl_MeasuredValues":
                replacement = "do { SenCtrl_MeasuredValues[0] = Adc_ChannelOne_Buffer[0]; " \
                              "SenCtrl_MeasuredValues[1] = Adc_ChannelOne_Buffer[1]; } while (0)"
            else:
                statements = [s.strip() for s in body.split(";") if s.strip()]
                if len(statements) != 1:
                    raise ValueError(f"Nontrivial wrapper {name}")
                replacement = "(" + substitute(statements[0], formal, actual) + ")"
            content = content[:match.start()] + replacement + content[pos:]
    return content

targets = {}
for path in APP.rglob("*"):
    if path.suffix not in (".c", ".h") or any(part in ("Debug", "Release", "RTE") for part in path.parts):
        continue
    original = path.read_text(errors="replace")
    if "Rte_" not in original and "RTE_" not in original and '"Rte.h"' not in original:
        continue
    content = original
    if path.suffix == ".h":
        content = re.sub(r"^.*\b(?:Rte_\w+|RTE_\w+)\s*\([^\n]*;\s*\r?\n", "", content, flags=re.M)
        content = re.sub(r"^/\* Run time environment interface\. \*/\r?\n(?=/\*|\s*\r?\n)", "", content, flags=re.M)
    content = re.sub(r'#include\s+"Rte\.h"', '#include "App_Dependencies.h"', content, flags=re.I)
    content = replace_calls(content)
    for old, new in sorted(simple_aliases.items(), key=lambda pair: -len(pair[0])):
        content = re.sub(r"\b" + re.escape(old) + r"\b", new, content)
    remaining = re.findall(r"\b(?:Rte_|RTE_)\w+", content)
    if remaining:
        raise ValueError(f"Unmapped symbols in {path}: {set(remaining)}")
    targets[path] = content

deps = APP / "Core" / "Inc" / "App_Dependencies.h"
deps_text = """#ifndef APP_DEPENDENCIES_H
#define APP_DEPENDENCIES_H

/* Direct component and service declarations used by the legacy application. */
#include "Btc.h"
#include "CenLoc.h"
#include "ExtLights.h"
#include "IntLights.h"
#include "SecAlm.h"
#include "SenCtrl.h"
#include "CrcH.h"
#include "TimH.h"
#include "AdcH.h"
#include "Dem.h"
#include "Pdc.h"
#include "DiagCtrl.h"
#include "UartH.h"
#include "WatchdogManager.h"
#include "EcuM.h"
#include "SystemManager.h"

#endif
"""
project = APP / ".cproject"
project_text = project.read_text()
project_text = "\n".join(line for line in project_text.splitlines()
                         if '/Rte}' not in line and 'name="Rte"' not in line) + "\n"
if re.search(r"(?:Rte_|RTE_|/Rte}|name=\"Rte\")", project_text):
    raise ValueError("RTE project entry remains")

for path, content in targets.items():
    path.write_text(content, newline="")
deps.write_text(deps_text)
project.write_text(project_text)
for name in ("RTE.c", "RTE.h"):
    (APP / "RTE" / name).unlink()
(APP / "RTE").rmdir()
print(f"Migrated {len(targets)} source/header files and removed RTE source/build entries")
