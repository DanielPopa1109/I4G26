"""Prove that the ELF closure gate rejects an injected resident-Flash call."""
from pathlib import Path
import importlib.util, struct, tempfile
root=Path(__file__).resolve().parents[2]
spec=importlib.util.spec_from_file_location('closure',root/'Modernization/tools/check_ram_closure.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
elf=root/'build/updater/fbl/G26_FBL.elf'
module.inspect(elf,disassemble=False)
data=bytearray(elf.read_bytes());header=struct.unpack_from('<16sHHIIIIIHHHHHH',data)
sections=[struct.unpack_from('<IIIIIIIIII',data,header[6]+i*header[11]) for i in range(header[12])]
names=sections[header[13]];names=data[names[4]:names[4]+names[5]]
for section in sections:
    name=names[section[0]:names.index(0,section[0])].decode()
    if name=='.rel.ram_blob':
        _,info=struct.unpack_from('<II',data,section[4]);table=sections[section[6]]
        struct.pack_into('<I',data,table[4]+(info>>8)*table[9]+4,0x08000221)
        break
else:raise RuntimeError('Missing retained SRAM relocations')
with tempfile.TemporaryDirectory(dir=root/'build') as folder:
    bad=Path(folder)/'injected-flash-reference.elf';bad.write_bytes(data)
    try:module.inspect(bad,disassemble=False)
    except AssertionError as error:
        if 'closure violated' not in str(error):raise
    else:raise AssertionError('Closure checker accepted an injected Flash dependency')
print('PASS ELF closure gate: injected erased-Flash dependency rejected')
