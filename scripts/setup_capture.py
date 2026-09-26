"""Prepare private media/config and pin the local Engine9000/Ghidra inputs."""
import hashlib
import json
import os
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ENGINE = ROOT / 'tools/engine9000/e9k-debugger'
ENGINE_SOURCE_REMOTE = 'git@github.com:retro-foundry/engine9000-public.git'
ENGINE_SOURCE_BRANCH = 'fa18-interceptor'
ENGINE_SOURCE_COMMIT = 'ace4c3a9553e7005ed32c6fee8817126a53a8887'
ROM = Path(os.environ['FA18_KICKSTART_ROM']) if 'FA18_KICKSTART_ROM' in os.environ else None


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    disk = ROOT / 'FA-18 Interceptor (1988)(Electronic Arts)[cr A-Ha].adf'
    if ROM is None:
        raise RuntimeError('FA18_KICKSTART_ROM must name a Kickstart 1.3 ROM image')
    for path in [disk, ROM, ENGINE / 'e9k-debugger.exe', ENGINE / 'system/ami9000.dll']:
        if not path.is_file():
            raise FileNotFoundError(path)
    for sub in ['local/media', 'local/system', 'local/saves', 'local/appdata', 'analysis']:
        (ROOT / sub).mkdir(parents=True, exist_ok=True)
    for src, dst in [(disk, ROOT / 'local/media/fa18.adf'), (ROM, ROOT / 'local/system/kick13.rom')]:
        if dst.exists() and sha(dst) != sha(src):
            raise RuntimeError(f'Private copy differs; refusing to overwrite: {dst}')
        if not dst.exists():
            shutil.copyfile(src, dst)
    config = f'''config_description=FA18 evidence A500 PAL OCS 68000 512K chip 512K slow
config_hardware=true
config_host=true
kickstart_rom_file={(ROOT / 'local/system/kick13.rom').as_posix()}
floppy0={(ROOT / 'local/media/fa18.adf').as_posix()}
floppy0type=0
floppy0writeprotected=true
nr_floppies=1
floppy_speed=100
chipset=ocs
ntsc=false
chipset_refreshrate=50
cpu_type=68000
cpu_model=68000
cpu_compatible=true
cpu_cycle_exact=true
cpu_speed=real
chipmem_size=1
bogomem_size=2
fastmem_size=0
z3mem_size=0
immediate_blits=false
rtc=none
sound_output=exact
gfx_framerate=1
puae_model=auto
puae_kickstart=kick13.rom
puae_cpu_compatibility=exact
puae_floppy_speed=100
puae_floppy_write_protection=enabled
puae_joyport=joystick
'''
    (ROOT / 'local/fa18.uae').write_text(config, encoding='ascii')
    files = [disk, ROM, ENGINE / 'e9k-debugger.exe', ENGINE / 'system/ami9000.dll', ROOT / 'local/fa18.uae']
    manifest = {'engine_release': 'v0.62-alpha',
                'engine_source_repository': ENGINE_SOURCE_REMOTE,
                'engine_source_branch': ENGINE_SOURCE_BRANCH,
                'engine_source_commit': ENGINE_SOURCE_COMMIT,
                'engine_upstream_commit': 'f9ca09b449866cba22ee9891757e8e6982f68600',
                'hardware': 'A500 PAL OCS 68000, 512 KiB Chip + 512 KiB slow, RTC absent',
                'files': [{'path': str(p), 'size': p.stat().st_size, 'sha256': sha(p)} for p in files]}
    (ROOT / 'local/toolchain.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(json.dumps(manifest, indent=2))


if __name__ == '__main__':
    main()
