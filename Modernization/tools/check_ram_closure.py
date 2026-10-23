"""Reject an ELF if any SRAM updater relocation resolves outside its SRAM.

Checks ELF32/ARM directly without pyelftools. Also verifies vector placement,
load bounds, BSS/stack separation and disassembled branch destinations.
"""
from pathlib import Path
import argparse, json, re, struct, subprocess
ROOT=Path(__file__).resolve().parents[2]
def inspect(path,disassemble=True):
    data=Path(path).read_bytes()
    assert data[:6]==b'\x7fELF\x01\x01','Expected ELF32 little endian'
    h=struct.unpack_from('<16sHHIIIIIHHHHHH',data)
    assert h[2]==40,'Expected ARM ELF, not another CPU'
    sections=[struct.unpack_from('<IIIIIIIIII',data,h[6]+i*h[11]) for i in range(h[12])]
    stringSection=sections[h[13]];names=data[stringSection[4]:stringSection[4]+stringSection[5]]
    def string(blob,off):return blob[off:blob.index(0,off)].decode()
    named={string(names,s[0]):(i,s) for i,s in enumerate(sections)}
    index,blob=named['.ram_blob'];_,bss=named['.ram_blob_bss']
    assert blob[3]==0x20000000 and blob[3]%512==0
    assert blob[3]+blob[5]<=bss[3] and bss[3]+bss[5]<=0x20018000
    words=struct.unpack_from('<112I',data,blob[4])
    assert words[0]==0x20019000
    for vector in words[1:]:assert vector&1 and blob[3]+448<=vector&~1<blob[3]+blob[5],hex(vector)
    symbols={};count=0;errors=[]
    for s in sections:
        if s[1]!=9 or s[7]!=index:continue
        table=sections[s[6]];strings=sections[table[6]];strings=data[strings[4]:strings[4]+strings[5]]
        for at in range(s[4],s[4]+s[5],s[9]):
            offset,info=struct.unpack_from('<II',data,at)
            sym=struct.unpack_from('<IIIBBH',data,table[4]+(info>>8)*table[9])
            name=string(strings,sym[0]) if sym[0] else '<section>'
            value=sym[1];count+=1
            if not 0x20000000<=value<0x20018000:errors.append(f'{offset:08X}: {name} -> {value:08X}')
    assert count>=112,'ELF must retain updater relocations (--emit-relocs)'
    assert not errors,'SRAM execution closure violated:\n'+'\n'.join(errors)
    if disassemble:
        command=ROOT/'.build-deps/arm-gcc/bin/arm-none-eabi-objdump.exe'
        listing=subprocess.check_output([str(command),'-d','-j','.ram_blob',str(path)],text=True)
        for line in listing.splitlines():
            assert not re.search(r'\bblx\s+r\d+',line),'Indirect call in SRAM closure: '+line
            match=re.search(r'\b(?:bl|blx|b\.w)\s+([0-9a-f]{8})\b',line)
            if match:assert blob[3]<=int(match[1],16)<blob[3]+blob[5],line
        Path(path).with_suffix('.ram.disassembly.txt').write_text(listing)
    # Load image must reside wholly in sectors 0-3.
    loads=[]
    for i in range(h[10]):
        ph=struct.unpack_from('<IIIIIIII',data,h[5]+i*h[9])
        if ph[0]==1 and ph[2]<=blob[3]<ph[2]+ph[5]:loads.append(ph[3]+blob[3]-ph[2])
    assert len(loads)==1 and 0x08000000<=loads[0] and loads[0]+blob[5]<=0x08010000
    return {'elf':str(path),'relocations_checked':count,'sram_code_data_bytes':blob[5],
        'sram_bss_bytes':bss[5],'sram_end':hex(bss[3]+bss[5]),'load_address':hex(loads[0]),'result':'PASS'}
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('elf',type=Path);args=p.parse_args()
    result=inspect(args.elf);args.elf.with_suffix('.ram-closure.json').write_text(json.dumps(result,indent=2))
    print(json.dumps(result,indent=2))
