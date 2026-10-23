"""Static integration/partition gates; actual build and runtime tests are separate."""
from pathlib import Path
import re, xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[2]
APP=ROOT/'I4_BCC_WORKSPACE/I4_BCC';FBL=ROOT/'G26_FBL'
for base in [APP,FBL]:
    for file in base.rglob('*'):
        if file.suffix not in ['.c','.h'] or {'Debug','Release'}.intersection(file.parts):continue
        text=file.read_text(errors='replace')
        assert not re.search(r'\b(?:Rte_|RTE_|Det_Report|MemMap)|["<](?:Rte|MemMap|Det)\.h',text),file
        assert not re.search(r'[A-Z]:[/\\].*(?:F93|ELC|ZGW)|\.\./.*(?:CBM_APPL|DMU_APPL)',text),file
    cproject=ET.parse(base/'.cproject');project=ET.parse(base/'.project')
    for entries in cproject.findall('.//sourceEntries'):
        assert {'Core','Drivers'}<=set(x.get('name') for x in entries)
        if base==APP:assert {'BSW','ApplicationLayer','Appl_Main','DiagnosticServices'}<=set(x.get('name') for x in entries)
    for link in project.findall('.//link'):
        uri=link.findtext('locationURI','')
        assert 'G26_FBL' not in uri and 'F93' not in uri and 'ELC' not in uri,uri
        assert link.find('location') is None,'Absolute vendor link remains'
for name in ['g26_board.h','g26_layout.h']:
    assert (APP/'Core/Inc'/name).read_bytes()==(FBL/'Core/Inc'/name).read_bytes()
for name in ['g26_mcp2515.c','g26_mcp_stm32.c','g26_mcp2515.h','g26_mcp_stm32.h','g26_can_types.h']:
    assert (APP/'BSW/MCAL'/name).read_bytes()==(FBL/'Core'/('Src' if name.endswith('.c') else 'Inc')/name).read_bytes()
board=(APP/'Core/Inc/g26_board.h').read_text()
for setting in ['G26_MCP2515_REWIRED_BOARD','G26_ENABLE_PROPOSED_BODY_COMMANDS','G26_ENABLE_PROPOSED_STATUS']:
    assert re.search(r'#define '+setting+r' 0\b',board)
assert not (APP/'Core/Src/g26_app_dcm.c').exists()
assert not (APP/'RTE').exists()
assert 'Dem_DtcArray' not in ''.join(f.read_text(errors='ignore') for f in APP.rglob('*.c') if not {'Debug','Release'}.intersection(f.parts))
assert 'G26_BswMainFunction();' in (APP/'Core/Src/freertos.c').read_text()
assert 'G26_BswInit();' in (APP/'SystemServices/EcuM/EcuM.c').read_text()
assert 'LENGTH = 0x3FE00' in (APP/'STM32F411RETX_FLASH.ld').read_text()
assert 'LENGTH = 64K' in (FBL/'STM32F411RETX_FLASH.ld').read_text()
assert 'sector=5;sector<=6;' in (FBL/'Core/Src/g26_flash_stm32.c').read_text()
print('PASS source integration: independent projects, every BSW source, exclusion gates, local MCAL copies, board interlocks, scheduling and partition bounds')
