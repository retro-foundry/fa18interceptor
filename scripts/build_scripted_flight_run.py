"""Build and execute a native Engine9000 in-flight keyboard coverage replay."""
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
NAME = 'run013_scripted'
SOURCE_STATE = ROOT / 'build/free_flight_600/state.bin'


def key_events(frame, key):
    # Engine9000's E9K_INPUT_V1 format is K <Libretro key> <character> <mods> <down>.
    return [f'F {frame} K {key} 0 0 1', f'F {frame + 3} K {key} 0 0 0']


def main():
    run = ROOT / 'captures' / NAME
    if run.exists():
        raise FileExistsError(f'{run} already exists')
    run.mkdir()
    shutil.copyfile(SOURCE_STATE, run / 'initial_state.bin')
    shutil.copyfile(ROOT / 'local/fa18.uae', run / 'config.uae')

    events = []
    frame = 120
    # RETROK_F1 through RETROK_F10 are 282--291 in Engine9000's libretro.h.
    for key in range(282, 292):
        events.extend(key_events(frame, key))
        frame += 90
    # Repeat the last function key after a clean no-input interval.
    events.extend(key_events(frame + 120, 291))
    events.extend(key_events(frame + 300, ord('h')))
    events.extend(key_events(frame + 390, ord('m')))
    events.extend(key_events(frame + 480, ord('r')))
    events.extend(key_events(frame + 570, ord('t')))
    events.extend(key_events(frame + 660, ord('a')))
    events.extend(key_events(frame + 750, ord('j')))
    events.extend(key_events(frame + 840, ord('g')))
    events.extend(key_events(frame + 930, ord(',')))
    events.extend(key_events(frame + 1020, ord('.')))
    events.extend(key_events(frame + 1110, 32))
    events.extend(key_events(frame + 1200, 13))
    final_frame = frame + 1500
    playback = 'E9K_INPUT_V1\n' + '\n'.join(events) + '\n'
    (run / 'playback.e9k').write_text(playback, encoding='ascii')
    manifest = {
        'status': 'scripted',
        'input_method': 'native E9K_INPUT_V1 replay',
        'initial_state': str(SOURCE_STATE.relative_to(ROOT)),
        'initial_frame': 600,
        'events': len(events),
        'last_input_frame': frame + 1200,
        'final_frame': final_frame,
        'hashes': {name: hashlib.sha256((run / name).read_bytes()).hexdigest()
                   for name in ('initial_state.bin', 'config.uae', 'playback.e9k')},
    }
    (run / 'run.json').write_text(json.dumps(manifest, indent=2) + '\n')
    subprocess.run([sys.executable, str(ROOT / 'scripts/engine9000_bridge.py'),
                    '--restore', str(run / 'initial_state.bin'),
                    '--config', str(run / 'config.uae'),
                    '--playback', str(run / 'playback.e9k'),
                    '--frames', str(final_frame), '--output', str(run / 'end_state')],
                   cwd=ROOT, check=True)
    print(json.dumps(manifest, indent=2))


if __name__ == '__main__':
    main()
