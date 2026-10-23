"""Fetch exactly the official CubeF4 v1.27.1 files referenced by G26.

Windows trust is used through pip's truststore; TLS verification stays enabled.
No installer, registry changes or system-wide package changes are performed.
"""
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor
import re, urllib.request, xml.etree.ElementTree as ET
import pip._vendor.truststore as truststore
truststore.inject_into_ssl()
ROOT=Path(__file__).resolve().parents[2]
CUBE=ROOT/'.build-deps/cube'
URL='https://raw.githubusercontent.com/STMicroelectronics/STM32CubeF4/v1.27.1/'
paths=set()
paths.add('Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/stm32_hal_legacy.h')
for project in ['I4_BCC_WORKSPACE/I4_BCC','G26_FBL']:
    for link in ET.parse(ROOT/project/'.project').findall('.//link'):
        location=link.findtext('location') or link.findtext('locationURI','')
        for marker in ['/STM32Cube_FW_F4_V1.27.1/','/.build-deps/cube/']:
            if marker in location:paths.add(location.split(marker)[1])
paths.update('Drivers/STM32F4xx_HAL_Driver/Src/'+n+'.c' for n in ['stm32f4xx_hal_spi','stm32f4xx_hal_flash','stm32f4xx_hal_flash_ex'])
dirs=['Drivers/STM32F4xx_HAL_Driver/Inc','Drivers/STM32F4xx_HAL_Driver/Inc/Legacy',
      'Drivers/CMSIS/Include','Drivers/CMSIS/Device/ST/STM32F4xx/Include',
      'Middlewares/Third_Party/FreeRTOS/Source/include',
      'Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2',
      'Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F']
local=set()
for project in ['I4_BCC_WORKSPACE/I4_BCC','G26_FBL']:
    for f in (ROOT/project).rglob('*.h'):
        if not {'Debug','Release'}.intersection(f.parts):local.add(f.name)
    for f in (ROOT/project).rglob('*'):
        if f.suffix in ('.h','.c') and not {'Debug','Release'}.intersection(f.parts):
            for n in re.findall(r'#\s*include\s*["<]([^">]+)[">]',f.read_text(errors='ignore')):
                if n.startswith(('stm32','core_cm','cmsis','mpu_')) or n in ['FreeRTOS.h','task.h','cmsis_os2.h']:
                    if n not in local:paths.add(n)
paths.update(['core_cm4.h','cmsis_version.h','cmsis_compiler.h','cmsis_gcc.h','mpu_armv7.h',
              'stm32f411xe.h','stm32f4xx.h','system_stm32f4xx.h','stm32f4xx_hal.h',
              'FreeRTOS.h','task.h','queue.h','timers.h','semphr.h','event_groups.h',
              'stream_buffer.h','list.h','croutine.h','portable.h','projdefs.h','deprecated_definitions.h','mpu_wrappers.h',
              'portmacro.h','cmsis_os2.h'])
paths.discard('stm32_lock_user.h')
done=set()
def get(path):
    candidates=[path] if '/' in path else [d+'/'+path for d in dirs]
    for candidate in candidates:
        output=CUBE/candidate
        if output.exists():return candidate,output.read_text(errors='ignore')
    for candidate in candidates:
        output=CUBE/candidate
        if output.exists():return candidate,output.read_text(errors='ignore')
        try:
            with urllib.request.urlopen(URL+candidate,timeout=30) as r:data=r.read()
        except urllib.error.HTTPError as e:
            if e.code==404:continue
            raise
        output.parent.mkdir(parents=True,exist_ok=True);output.write_bytes(data)
        return candidate,data.decode(errors='ignore')
    raise RuntimeError('Official Cube dependency unavailable: '+path)
while paths-done:
    batch=sorted(paths-done);done.update(batch)
    with ThreadPoolExecutor(max_workers=8) as pool:
        for candidate,data in pool.map(get,batch):
            if candidate.endswith('deprecated_definitions.h'):continue
            data=re.sub(r'/\*.*?\*/|//[^\n]*','',data,flags=re.S)
            for name in re.findall(r'#\s*include\s*"([^"/]+)"',data):
                if name.endswith('_test_access_functions.h'):continue
                if '\\' in name or name in ['stdint.h','stddef.h','stdlib.h','string.h','assert.h','freertos_tasks_c_additions.h','stm32_assert.h']:continue
                if name in local:continue
                # Unselected MCU/compiler ports are intentionally excluded.
                if name.startswith('stm32f4') and name.endswith('xx.h') and name!='stm32f4xx.h' and 'hal' not in name:continue
                if name.startswith('stm32f4') and name.endswith('xe.h') and name!='stm32f411xe.h':continue
                if name.startswith(('stm32','cmsis','core_cm','mpu_')) or candidate.startswith('Middlewares/'):
                    if name not in ['cmsis_armcc.h','cmsis_armclang.h','cmsis_iccarm.h','cmsis_ccs.h','cmsis_csm.h']:
                        paths.add(name)
    print('Official dependency files resolved:',len(done),flush=True)
print('Ready: minimal CubeF4 v1.27.1 dependency tree')
