"""Fetch isolated, ignored build dependencies; never installs system software."""
from pathlib import Path
import zipfile, subprocess, sys

root = Path(__file__).resolve().parents[2]
dest = root / '.build-deps'
dest.mkdir(exist_ok=True)
def fetch(url, name):
    path = dest / name
    if not path.exists():
        print('Downloading', name, flush=True)
        subprocess.run(['powershell.exe','-NoProfile','-Command',
            "Invoke-WebRequest -Uri '"+url+"' -OutFile '"+str(path)+"'"],check=True)
    return path
def unpack(url, name, target):
    output = dest / target
    if output.exists(): return
    archive = fetch(url, name)
    with zipfile.ZipFile(archive) as z:
        prefix = z.namelist()[0].split('/')[0] + '/'
        for info in z.infolist():
            rel = info.filename.removeprefix(prefix)
            if not rel or info.is_dir(): continue
            path = output / rel
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(z.read(info))
    print('Ready', target, flush=True)

unpack('https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-mingw-w64-i686-arm-none-eabi.zip',
       'arm-gcc.zip', 'arm-gcc')
subprocess.run([sys.executable,'-m','pip','install','--target',str(dest/'host'),'ziglang==0.13.0'],check=True)

subprocess.run([sys.executable,str(root/'Modernization/tools/bootstrap_minimal.py')],check=True)
