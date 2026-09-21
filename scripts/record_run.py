"""Open stock Engine9000 at the preserved menu and record a human play session."""
import argparse
import ctypes as C
from ctypes import wintypes as W
import datetime
import json
import os
from pathlib import Path
import shutil
import subprocess
import time
from make_gui_snapshot import package

ROOT = Path(__file__).resolve().parents[1]
ENGINE = ROOT / 'tools/engine9000/e9k-debugger'
BASE = ROOT / 'captures/baseline_menu'


def window(pid, timeout=15):
    user = C.windll.user32
    found = []
    callback_type = C.WINFUNCTYPE(W.BOOL, W.HWND, W.LPARAM)
    @callback_type
    def visit(hwnd, _):
        owner = W.DWORD()
        user.GetWindowThreadProcessId(hwnd, C.byref(owner))
        if owner.value == pid and user.IsWindowVisible(hwnd):
            title = C.create_unicode_buffer(512)
            user.GetWindowTextW(hwnd, title, 512)
            if 'ENGINE9000' in title.value.upper():
                found.append(hwnd)
        return True
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        user.EnumWindows(visit, 0)
        if found:
            return found[0]
        time.sleep(.1)
    raise RuntimeError(f'No Engine9000 window for PID {pid}')


def key(hwnd, vk):
    user = C.windll.user32
    scan = user.MapVirtualKeyW(vk, 0)
    user.PostMessageW.argtypes = [W.HWND, W.UINT, W.WPARAM, W.LPARAM]
    user.PostMessageW(hwnd, 0x100, vk, 1 | (scan << 16))
    time.sleep(.05)
    user.PostMessageW(hwnd, 0x101, vk, 1 | (scan << 16) | (3 << 30))


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--name', default=datetime.datetime.now().strftime('run_%Y%m%d_%H%M%S'))
    p.add_argument('--initial-state', type=Path, default=BASE / 'state.bin',
                   help='Compatible serialized core state to restore before recording')
    args = p.parse_args()
    if Path(args.name).name != args.name:
        raise ValueError('Run name must be one directory component')
    run = ROOT / 'captures' / args.name
    run.mkdir(parents=True, exist_ok=False)
    for sub in ['saves', 'appdata']:
        (run / sub).mkdir()
    # Preserve the original known-good baseline restore pairing. Function-key
    # coverage uses build_scripted_flight_run.py's native replay format.
    (run / 'appdata/e9k-debugger.cfg').write_text(
        'comp.config.hotkey.restart=unbound\n'
        'comp.config.hotkey.restore_state=F8\n')
    initial_state = args.initial_state.resolve()
    if not initial_state.is_file():
        raise FileNotFoundError(initial_state)
    shutil.copyfile(initial_state, run / 'initial_state.bin')
    shutil.copyfile(ROOT / 'local/toolchain.json', run / 'toolchain.json')
    config = ROOT / 'local/fa18.uae'
    shutil.copyfile(config, run / 'config.uae')
    (run / 'saves/fa18.uae.e9k-save').write_bytes(package(initial_state.read_bytes(), config.read_bytes(), 0))
    command = [str(ENGINE / 'e9k-debugger.exe'), '--amiga', '--uae', str(config),
               '--system-dir', str(ROOT / 'local/system'), '--save-dir', str(run / 'saves'),
               '--record', str(run / 'inputs.e9k'), '--window-size', '1400x900']
    # Engine9000 selects its preferences through APPDATA. Isolate only this child
    # process so another game's settings cannot silently alter this capture.
    child_env = os.environ.copy()
    child_env['APPDATA'] = str(run / 'appdata')
    with (run / 'stdout.log').open('w') as stdout, (run / 'stderr.log').open('w') as stderr:
        process = subprocess.Popen(command, cwd=ENGINE, env=child_env, stdout=stdout, stderr=stderr)
    manifest = {'pid': process.pid, 'command': command, 'initial_state': 'initial_state.bin',
                'initial_frame': 0, 'recording': 'inputs.e9k', 'status': 'starting',
                'baseline': str(initial_state.relative_to(ROOT)), 'created': datetime.datetime.now().isoformat()}
    (run / 'run.json').write_text(json.dumps(manifest, indent=2) + '\n')
    hwnd = window(process.pid)
    time.sleep(2)  # Let core initialization finish before its Restore hotkey.
    key(hwnd, 0x77)  # F8, the isolated Engine9000 Restore binding.
    time.sleep(.2)
    manifest.update(status='recording', hwnd=int(hwnd),
                    prelude_bytes=(run / 'inputs.e9k').stat().st_size)
    (run / 'run.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(json.dumps(manifest, indent=2))


if __name__ == '__main__':
    main()
