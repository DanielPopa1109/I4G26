"""Check linked flash-busy helpers run wholly in startup-copied SRAM."""
from pathlib import Path
import argparse, json, re, struct, subprocess
ROOT=Path(__file__).resolve().parents[2]

def inspect(path):
    path=Path(path);data=path.read_bytes()
    h=struct.unpack_from('<16sHHIIIIIHHHHHH',data)
    assert data[:6]==b'\x7fELF\x01\x01' and h[2]==40
    sections=[struct.unpack_from('<IIIIIIIIII',data,h[6]+i*h[11]) for i in range(h[12])]
    symbols={}
    for table in sections:
        if table[1]!=2:continue
        s=sections[table[6]];strings=data[s[4]:s[4]+s[5]]
        for at in range(table[4],table[4]+table[5],table[9]):
            name,value,size,info,other,index=struct.unpack_from('<IIIBBH',data,at)
            if name:symbols[strings[name:strings.index(0,name)].decode()]=(value,size,info,index)
    app='eraseRam' in symbols
    names=['waitReady','unlock','flushCaches','eraseRam','programRam'] if app else ['wait_ready','unlock_flash','flush_caches','erase_sector','program_bytes']
    ranges=[]
    for name in names:
        value,size,info,index=symbols[name];value&=~1
        assert size and 0x20000000<=value<value+size<=0x2001FC00,(name,hex(value),size)
        assert symbols['_sdata'][0]<=value and value+size<=symbols['_edata'][0],name+' not copied by startup'
        ranges.append((value,value+size))
    def inside(value):return any(a<=value<b for a,b in ranges)
    relocations=0
    for section in sections:
        if section[1]!=9:continue
        table=sections[section[6]]
        for at in range(section[4],section[4]+section[5],section[9]):
            offset,info=struct.unpack_from('<II',data,at)
            if not inside(offset):continue
            symbol=struct.unpack_from('<IIIBBH',data,table[4]+(info>>8)*table[9])
            assert 0x20000000<=symbol[1]<0x2001FC00,'Flash dependency in busy helper'
            relocations+=1
    listing=subprocess.check_output([str(ROOT/'.build-deps/arm-gcc/bin/arm-none-eabi-objdump.exe'),'-d','-j','.data',str(path)],text=True)
    branches=0
    for line in listing.splitlines():
        address=re.match(r'\s*([0-9a-f]{8}):',line)
        if not address or not inside(int(address[1],16)):continue
        assert not re.search(r'\bblx?\s+r\d+',line),'Indirect call: '+line
        target=re.search(r'\b(?:bl|blx|b(?:\.[nw])?)\s+([0-9a-f]{8})\b',line)
        if target:
            assert inside(int(target[1],16)),'Busy helper branches outside SRAM closure: '+line
            branches+=1
    assert branches,'Missing flash helper disassembly'
    result={'elf':str(path),'result':'PASS','helpers':names,'branches_checked':branches,'relocations_checked':relocations}
    path.with_suffix('.flash-helpers.json').write_text(json.dumps(result,indent=2))
    return result

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('elf',type=Path)
    print(json.dumps(inspect(parser.parse_args().elf),indent=2))
