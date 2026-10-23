"""Keep both CubeIDE projects self-contained and enumerate every BSW source."""
from pathlib import Path
import xml.etree.ElementTree as ET
import copy
ROOT=Path(__file__).resolve().parents[2]
for relative,parents in [('I4_BCC_WORKSPACE/I4_BCC',2),('G26_FBL',1)]:
    project=ROOT/relative;app=parents==2
    file=project/'.project';tree=ET.parse(file);root=tree.getroot();links=root.find('linkedResources')
    for link in list(links):
        location=link.find('location')
        if link.findtext('name','').startswith('G26_Shared/') or 'g26_tp' in link.findtext('name',''):
            links.remove(link);continue
        if location is not None and '/STM32Cube_FW_F4_V1.27.1/' in location.text:
            tail=location.text.split('/STM32Cube_FW_F4_V1.27.1/')[1]
            link.remove(location);ET.SubElement(link,'locationURI').text=f'PARENT-{parents}-PROJECT_LOC/.build-deps/cube/'+tail
        elif 'G26_FBL' in link.findtext('locationURI',''):
            links.remove(link)
    # NvM Flash backend needs the flash HAL in APP too.
    for name in (['stm32f4xx_hal_flash.c','stm32f4xx_hal_flash_ex.c'] if app else []):
        if any(x.findtext('name','').endswith(name) for x in links):continue
        link=ET.SubElement(links,'link');ET.SubElement(link,'name').text='Drivers/STM32F4xx_HAL_Driver/'+name
        ET.SubElement(link,'type').text='1';ET.SubElement(link,'locationURI').text=f'PARENT-{parents}-PROJECT_LOC/.build-deps/cube/Drivers/STM32F4xx_HAL_Driver/Src/'+name
    ET.indent(tree,space='\t');tree.write(file,encoding='UTF-8',xml_declaration=True)
    file=project/'.cproject';tree=ET.parse(file)
    settings=tree.getroot().find("storageModule[@moduleId='org.eclipse.cdt.core.settings']")
    configs=settings.findall('cconfiguration')
    # Retain default legacy configs; make the assumed oscillator explicit in variant names.
    if app and len(configs)==4:
        variants=configs[2:]
    else:
        variants=[c for c in configs if 'g26can' in c.get('id','')]
        if not variants:
            for original in configs[:2]:
                duplicate=copy.deepcopy(original)
                old=original.get('id');new=old+'.g26can'
                for node in duplicate.iter():
                    for key,value in list(node.attrib.items()):
                        node.set(key,value.replace(old,new))
                settings.append(duplicate);variants.append(duplicate)
    for i,config in enumerate(variants):
        name='CAN_16MHz_'+('Debug' if i==0 else 'Release')
        for node in config.findall("storageModule[@moduleId='org.eclipse.cdt.core.settings']")+config.findall('.//configuration'):
            node.set('name',name)
        for builder in config.findall('.//builder'):
            builder.set('buildPath','${workspace_loc:/'+project.name+'}/'+name)
        for tool in config.findall('.//tool'):
            if tool.get('superClass','').endswith('tool.c.compiler'):
                option=next((o for o in tool.findall('option') if o.get('valueType')=='definedSymbols'),None)
                if option is None:continue
                for define in ['G26_MCP2515_REWIRED_BOARD=1','G26_MCP_OSC_HZ=16000000U']:
                    if not any(x.get('value')==define for x in option):ET.SubElement(option,'listOptionValue',{'builtIn':'false','value':define})
    if not app:
        for config in tree.findall('.//configuration'):
            config.set('postbuildStep','python "${ProjDirPath}/../Modernization/tools/check_ram_closure.py" "${BuildArtifactFileName}"')
        for i,tool in enumerate(tree.findall('.//tool')):
            if tool.get('superClass','').endswith('tool.c.linker'):
                superClass='com.st.stm32cube.ide.mcu.gnu.managedbuild.tool.c.linker.option.otherflags'
                option=next((o for o in tool.findall('option') if o.get('superClass')==superClass),None)
                if option is None:option=ET.SubElement(tool,'option',{'id':superClass+'.g26'+str(i),'superClass':superClass,'valueType':'string','name':'Other flags'})
                option.set('value','-Wl,--emit-relocs')
    own=['Core','Appl_Main','ApplicationLayer','CommunicationServices','DiagnosticServices','IOServices','SystemServices','BSW','Drivers','Middlewares'] if app else ['Core','Drivers']
    dirs=sorted({f.parent.relative_to(project).as_posix() for f in project.rglob('*.h') if not {'Debug','Release'}.intersection(f.parts)})
    cube='../'*parents+'.build-deps/cube/'
    vendor=[cube+p for p in ['Drivers/STM32F4xx_HAL_Driver/Inc','Drivers/STM32F4xx_HAL_Driver/Inc/Legacy','Drivers/CMSIS/Include','Drivers/CMSIS/Device/ST/STM32F4xx/Include']]
    if app:vendor += [cube+'Middlewares/Third_Party/FreeRTOS/Source/'+d for d in ['include','CMSIS_RTOS_V2','portable/GCC/ARM_CM4F']]
    for entries in tree.findall('.//sourceEntries'):
        entries.clear()
        for name in own:ET.SubElement(entries,'entry',{'flags':'VALUE_WORKSPACE_PATH|RESOLVED','kind':'sourcePath','name':name})
    for option in tree.findall('.//option'):
        if option.get('valueType')=='includePath':
            option[:]=[]
            for path in ['../'+d for d in dirs]+['../'+v for v in vendor]:
                ET.SubElement(option,'listOptionValue',{'builtIn':'false','value':path})
    ET.indent(tree,space='\t');tree.write(file,encoding='UTF-8',xml_declaration=True)
print('PASS CubeIDE source entries, local sources and relative vendor links')
