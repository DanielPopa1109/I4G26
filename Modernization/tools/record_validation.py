"""Record freshly executed gates, linked memory use and artifact fingerprints."""
from pathlib import Path
import datetime, hashlib, json, re, subprocess, sys
ROOT=Path(__file__).resolve().parents[2]
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
compiler=ROOT/'.build-deps/arm-gcc/bin/arm-none-eabi-gcc.exe'
version=subprocess.check_output([str(compiler),'--version'],text=True).splitlines()[0]
gates=[]
for script in ['Modernization/tests/check_source_integration.py','Modernization/tests/test_ram_checker.py']:
    result=subprocess.run([sys.executable,str(ROOT/script)],capture_output=True,text=True,check=True)
    gates.append({'command':'python '+script,'result':'PASS','output':result.stdout})
native=ROOT/'build/host/native_tests.json'
assert native.exists(),'Execute run_tests.py first'
artifacts=[]
for variant in ['legacy','can','updater','legacy-debug','can-debug','updater-debug']:
    for kind in (['fbl'] if variant.startswith('updater') else ['app','fbl']):
        folder=ROOT/'build'/variant/kind;elf=folder/('G26_APP.elf' if kind=='app' else 'G26_FBL.elf')
        log=(folder/'build.log').read_text()
        source=ROOT/('I4_BCC_WORKSPACE/I4_BCC' if kind=='app' else 'G26_FBL')
        for path in source.rglob('*'):
            if (path.suffix in ['.c','.h','.s','.ld'] or path.name in ['.cproject','.project']) and not {'Debug','Release'}.intersection(path.parts):
                assert path.stat().st_mtime<=elf.stat().st_mtime,'Rebuild after source modification: '+str(path)
        regions={match[0]:int(match[1]) for match in re.findall(r'^\s*(RAM|FLASH|UPDATER|UPDSTACK):\s*(\d+) B',log,re.M)}
        assert regions,'Missing successful link memory report'
        record={'variant':variant,'project':kind,'result':'PASS','sources':len(json.loads((folder/'commands.json').read_text())),
                'memory_bytes':regions,'files':{}}
        for path in [elf,elf.with_suffix('.bin'),elf.with_suffix('.hex')]:
            record['files'][path.suffix]={'path':path.relative_to(ROOT).as_posix(),'bytes':path.stat().st_size,'sha256':digest(path)}
        record['flash_helpers']=json.loads(elf.with_suffix('.flash-helpers.json').read_text())
        record['flash_helpers'].pop('elf')
        if kind=='fbl':
            record['updater_closure']=json.loads(elf.with_suffix('.ram-closure.json').read_text());record['updater_closure'].pop('elf')
        artifacts.append(record)
source_hash=hashlib.sha256();source_count=0
for folder in ['I4_BCC_WORKSPACE/I4_BCC','G26_FBL','Modernization']:
    for path in sorted((ROOT/folder).rglob('*')):
        if path.is_file() and (path.suffix in ['.c','.h','.s','.ld','.py'] or path.name in ['.cproject','.project']) and not {'Debug','Release','__pycache__'}.intersection(path.parts):
            source_hash.update(path.relative_to(ROOT).as_posix().encode()+b'\0'+path.read_bytes());source_count+=1
report={'recorded_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'compiler':version,
        'dependencies':{'STM32CubeF4':'v1.27.1 official ST sources','native_compiler':'Zig 0.13.0 cc'},
        'scope':'GNU Cortex-M4 builds from Cube project source mappings; no CubeIDE GUI or hardware execution',
        'source_fingerprint':{'files':source_count,'sha256':source_hash.hexdigest()},'artifacts':artifacts,
        'native_suites':json.loads(native.read_text()),'gates':gates,
        'hardware_verified':False,'integration_ready':False,'software_deployed':False,
        'limitations':['Board wiring/oscillator and recovery unavailable','External ZGW/FCD target configuration pending',
                      'Single-bank erase timing and application regression require bench tests',
                      'RWX linker segments and legacy/vendor compiler warnings remain',
                      'No physical flashing, reset, option-byte changes, deployment, commit or push performed']}
target=ROOT/'docs/modernization/VALIDATION_RESULTS.json'
target.write_text(json.dumps(report,indent=2)+'\n')
print('PASS validation record:',target)
