"""Build the existing independent Cube projects with Cortex-M4 GNU tools.

Uses project source entries and linked official Cube sources, never generated
Debug makefiles. Output, command manifests, maps and logs live under build/.
"""
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor
import argparse, json, os, subprocess, sys, xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[2]
parser=argparse.ArgumentParser()
parser.add_argument('--variant',choices=['legacy','can','updater'],default='legacy')
parser.add_argument('--project',choices=['app','fbl','all'],default='all')
parser.add_argument('--debug',action='store_true')
args=parser.parse_args()
BIN=Path(os.environ.get('ARM_GCC_BIN',ROOT/'.build-deps/arm-gcc/bin'))
CC=BIN/'arm-none-eabi-gcc.exe'
def run(command):
    r=subprocess.run([str(x) for x in command],capture_output=True,text=True)
    return r.returncode,r.stdout+r.stderr
for kind,relative in [('app','I4_BCC_WORKSPACE/I4_BCC'),('fbl','G26_FBL')]:
    if args.project not in (kind,'all'):continue
    project=ROOT/relative
    output=ROOT/'build'/(args.variant+('-debug' if args.debug else ''))/kind;output.mkdir(parents=True,exist_ok=True)
    sources=[]
    entries=ET.parse(project/'.cproject').findall('.//sourceEntries')[0]
    for entry in entries:
        folder=project/entry.get('name')
        for source in folder.rglob('*'):
            if source.suffix in ('.c','.s') and not {'Debug','Release'}.intersection(source.parts):sources.append(source)
    for link in ET.parse(project/'.project').findall('.//link'):
        name=link.findtext('name')
        if not name.endswith(('.c','.s')):continue
        location=link.findtext('locationURI')
        if location:
            prefix,rest=location.split('/',1)
            n=int(prefix.split('-')[1]);source=project
            for _ in range(n):source=source.parent
            source=source/rest
        else:source=Path(link.findtext('location'))
        sources.append(source)
    sources=sorted(set(sources))
    includes={f.parent for f in project.rglob('*.h') if not {'Debug','Release'}.intersection(f.parts)}
    cube=ROOT/'.build-deps/cube'
    includes.update([cube/'Drivers/STM32F4xx_HAL_Driver/Inc',cube/'Drivers/STM32F4xx_HAL_Driver/Inc/Legacy',
        cube/'Drivers/CMSIS/Include',cube/'Drivers/CMSIS/Device/ST/STM32F4xx/Include'])
    if kind=='app':includes.update(cube/'Middlewares/Third_Party/FreeRTOS/Source'/d for d in ['include','CMSIS_RTOS_V2','portable/GCC/ARM_CM4F'])
    flags=['-mcpu=cortex-m4','-mthumb','-mfpu=fpv4-sp-d16','-mfloat-abi=hard','-std=gnu11','-O0' if args.debug else '-Os','-g3',
           '-ffunction-sections','-fdata-sections','-fstack-usage','-Wall','-Wextra','-DUSE_HAL_DRIVER','-DSTM32F411xE']
    if args.variant!='legacy':flags+=['-DG26_MCP2515_REWIRED_BOARD=1','-DG26_MCP_OSC_HZ=16000000U']
    if args.variant=='updater':flags+=['-DFBL_UPDATER_ACTIVATION_ALLOWED=1']
    for include in sorted(includes):flags+=['-I',str(include)]
    commands=[];objects=[]
    for i,source in enumerate(sources):
        obj=output/(str(i)+'_'+source.stem+'.o');objects.append(obj)
        command=[CC,*flags,'-c',source,'-o',obj];commands.append(command)
    (output/'commands.json').write_text(json.dumps([[str(x) for x in c] for c in commands],indent=2))
    with ThreadPoolExecutor(max_workers=8) as pool:results=list(pool.map(run,commands))
    log=''.join(text for _,text in results)
    if any(code for code,_ in results):
        (output/'build.log').write_text(log)
        print('\n'.join(dict.fromkeys(line for line in log.splitlines() if 'error:' in line))[:12000])
        raise SystemExit(kind+' compile FAILED; full diagnostics: '+str(output/'build.log'))
    elf=output/('G26_APP.elf' if kind=='app' else 'G26_FBL.elf')
    link=[CC,*flags,*objects,'-T',project/'STM32F411RETX_FLASH.ld','--specs=nano.specs','--specs=nosys.specs',
          '-Wl,--gc-sections,--emit-relocs,-Map='+str(output/'firmware.map'),'-Wl,--print-memory-usage',
          '-Wl,--start-group','-lc','-lm','-Wl,--end-group','-o',elf]
    code,text=run(link);log+=text;(output/'build.log').write_text(log)
    print(text)
    if code:print(log);raise SystemExit(kind+' link FAILED')
    for fmt,ext in [('binary','bin'),('ihex','hex')]:
        code,text=run([BIN/'arm-none-eabi-objcopy.exe','-O',fmt,elf,elf.with_suffix('.'+ext)])
        if code:raise SystemExit(text)
    code,text=run([BIN/'arm-none-eabi-size.exe',elf]);print(text)
    subprocess.run([sys.executable,str(ROOT/'Modernization/tools/check_flash_helpers.py'),str(elf)],check=True)
    if kind=='fbl':subprocess.run([sys.executable,str(ROOT/'Modernization/tools/check_ram_closure.py'),str(elf)],check=True)
    print('PASS',kind,args.variant,'sources',len(sources),flush=True)
