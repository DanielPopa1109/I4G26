"""Syntax-parse the portable C core when a native C compiler is unavailable."""
from pathlib import Path
import re
from pycparser import c_parser

project = Path(__file__).resolve().parents[2] / "G26_FBL" / "Core"
headers = project / "Inc"
sources = project / "Src"
files = [headers / name for name in (
    "g26_update.h", "g26_tp.h", "g26_mcp2515.h", "g26_uds.h",
)]
files.extend(sources / name for name in
             ("g26_update.c", "g26_tp.c", "g26_mcp2515.c", "g26_uds.c"))
files.append(Path(__file__).resolve().parent / "test_core.c")
files.extend([headers / "g26_flash_stm32.h", sources / "g26_flash_stm32.c",
              headers / "g26_mcp_stm32.h", sources / "g26_mcp_stm32.c"])
source = "typedef unsigned long size_t; typedef unsigned char uint8_t; " \
         "typedef unsigned short uint16_t; typedef unsigned int uint32_t; " \
         "typedef signed int int32_t; typedef int HAL_StatusTypeDef; " \
         "typedef struct { unsigned TypeErase, VoltageRange, Sector, NbSectors; } " \
         "FLASH_EraseInitTypeDef;\n"
for path in files:
    body = path.read_text(encoding="utf-8")
    body = re.sub(r"/\*.*?\*/", "", body, flags=re.S)
    body = re.sub(r"//[^\n]*", "", body)
    body = re.sub(r"^\s*#.*$", "", body, flags=re.M)
    source += body + "\n"
c_parser.CParser().parse(source, filename="G26 portable core")
print("PASS: C core, STM32 adapter, and test harness syntax parsed")
