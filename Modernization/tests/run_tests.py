"""Execute portable G26 cores with a real native C compiler."""
from pathlib import Path
import os, shutil, subprocess, sys, json, hashlib
ROOT=Path(__file__).resolve().parents[2]
HERE=ROOT/'Modernization/tests';APP=ROOT/'I4_BCC_WORKSPACE/I4_BCC';FBL=ROOT/'G26_FBL/Core'
zig=ROOT/'.build-deps/host/ziglang/zig.exe'
cc=[str(zig),'cc'] if zig.exists() else [next((shutil.which(n) for n in ['clang','gcc','cc'] if shutil.which(n)),'')]
if not cc[0]:raise SystemExit('NO BUILD: native C compiler unavailable')
out=ROOT/'build/host';out.mkdir(parents=True,exist_ok=True)
results=[]
common=['-std=c11','-O1','-UNDEBUG','-g','-Wall','-Wextra','-Wno-unused-parameter']
def build(name,sources,includes,defines=[]):
    exe=out/(name+'.exe')
    cmd=cc+common+['-D'+x for x in defines]
    for inc in includes:cmd+=['-I',str(inc)]
    cmd +=[str(x) for x in sources]+['-o',str(exe)]
    result=subprocess.run(cmd,capture_output=True,text=True)
    (out/(name+'.build.log')).write_text(result.stdout+result.stderr)
    if result.returncode:
        print(result.stdout+result.stderr);raise SystemExit(name+' build FAILED')
    args=[str(exe)]
    if name=='core' and (ROOT/'build/can/app/G26_APP.bin').exists():args+=[str(ROOT/'build/can/app/G26_APP.bin')]
    if name=='ram_updater' and (ROOT/'build/updater/fbl/G26_FBL.bin').exists():args+=[str(ROOT/'build/updater/fbl/G26_FBL.bin')]
    result=subprocess.run(args,capture_output=True,text=True)
    (out/(name+'.run.log')).write_text(result.stdout+result.stderr)
    print(result.stdout+result.stderr,flush=True)
    if result.returncode:raise SystemExit(name+' FAILED: '+str(result.returncode))
    print('PASS '+name,flush=True)
    results.append({'suite':name,'result':'PASS','assertions_enabled':True,'compiler':cc,
                    'executable_sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),
                    'output':result.stdout+result.stderr})
build('core',[FBL/'Src'/n for n in ['g26_update.c','g26_tp.c','g26_uds.c','g26_mcp2515.c']]+[HERE/'test_core.c'],[FBL/'Inc'])
includes=[APP/'Core/Inc',APP/'ProjectIncludes',APP/'DiagnosticServices/Dem']
includes+=[p for p in (APP/'BSW').rglob('*') if p.is_dir()]
sources=list((APP/'BSW/Com').rglob('*.c'))+list((APP/'BSW/Diag').rglob('*.c'))
sources+=list((APP/'BSW/Mem').glob('*.c'))+list((APP/'BSW/Mem/MemIf').glob('*.c'))+list((APP/'BSW/Mem/Crc').glob('*.c'))
sources+=list((APP/'DiagnosticServices/Dem').glob('*.c'))+[HERE/'test_bsw.c']
build('bsw',sources,includes,['G26_HOST_TEST','G26_ENABLE_PROPOSED_STATUS=1'])
build('ram_updater',[FBL/'Src/ram_blob.c',FBL/'Src/Fbl_Updater.c',HERE/'test_ram_updater.c'],[FBL/'Inc'],['RAM_HOST_TEST'])
build('command_rx',[APP/'CommunicationServices/Uart/G26_CommandRx.c',HERE/'test_command_rx.c'],[APP/'CommunicationServices/Uart'],['G26_HOST_TEST'])
print('PASS: all G26 native suites executed')
(out/'native_tests.json').write_text(json.dumps(results,indent=2))
