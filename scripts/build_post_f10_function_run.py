"""Create a deterministic, post-F10 Engine9000 function-key replay."""
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'build/run003_post_f10_first/state.bin'
CONFIG = ROOT / 'captures/run003/config.uae'
NAME = 'run014_post_f10_functions'
START_FRAME = 3899


def event_pair(frame, key):
    return [f'F {frame} K {key} 0 16 1', f'F {frame + 5} K {key} 0 16 0']


def main():
    run = ROOT / 'captures' / NAME
    if run.exists():
        raise FileExistsError(run)
    run.mkdir()
    shutil.copyfile(SOURCE, run / 'initial_state.bin')
    shutil.copyfile(CONFIG, run / 'config.uae')
    events = []
    frame = START_FRAME + 120
    for key in range(282, 292):
        events.extend(event_pair(frame, key))
        frame += 90
    events.extend(event_pair(frame + 180, 291))
    final_frame = frame + 600
    playback = 'E9K_INPUT_V1\n' + '\n'.join(events) + '\n'
    (run / 'playback.e9k').write_text(playback, encoding='ascii')
    manifest = {
        'status': 'scripted',
        'input_method': 'native E9K_INPUT_V1 replay',
        'initial_state': str(SOURCE.relative_to(ROOT)),
        'initial_frame': START_FRAME,
        'events': len(events),
        'last_input_frame': frame + 185,
        'final_frame': final_frame,
        'hashes': {name: hashlib.sha256((run / name).read_bytes()).hexdigest()
                   for name in ('initial_state.bin', 'config.uae', 'playback.e9k')},
    }
    (run / 'run.json').write_text(json.dumps(manifest, indent=2) + '\n')
    subprocess.run([sys.executable, str(ROOT / 'scripts/engine9000_bridge.py'),
                    '--restore', str(run / 'initial_state.bin'), '--config', str(run / 'config.uae'),
                    '--playback', str(run / 'playback.e9k'), '--start-frame', str(START_FRAME),
                    '--frames', str(final_frame - START_FRAME), '--output', str(run / 'end_state')],
                   cwd=ROOT, check=True)
    print(json.dumps(manifest, indent=2))


if __name__ == '__main__':
    main()
