"""Capture a bounded, deterministic demonstration-flight run with Engine9000."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
BASE = ROOT / 'captures' / 'baseline_menu'


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--name', default='attract_run001')
    parser.add_argument('--frames', type=int, default=9000,
                        help='PAL video frames after the menu selection (default: 180 seconds)')
    args = parser.parse_args()
    if Path(args.name).name != args.name:
        raise ValueError('Run name must be one directory component')
    if args.frames < 600:
        raise ValueError('Use at least 600 frames so the selection reaches flight')
    capture = ROOT / 'captures' / args.name
    if capture.exists():
        raise FileExistsError(capture)
    recording = ROOT / 'local' / 'start_demo.e9k'
    command = ['python', str(ROOT / 'scripts' / 'engine9000_bridge.py'),
               '--restore', str(BASE / 'state.bin'), '--config', str(ROOT / 'local' / 'fa18.uae'),
               '--playback', str(recording), '--frames', str(args.frames), '--output', str(capture)]
    subprocess.run(command, check=True)
    shutil.copyfile(recording, capture / 'playback.e9k')
    shutil.copyfile(BASE / 'state.bin', capture / 'initial_state.bin')
    shutil.copyfile(ROOT / 'local' / 'fa18.uae', capture / 'config.uae')
    shutil.copyfile(ROOT / 'local' / 'toolchain.json', capture / 'toolchain.json')
    manifest = {
        'kind': 'attract_mode',
        'status': 'sealed',
        'frames': args.frames,
        'initial_frame': 0,
        'selection_events': 2,
        'human_input_events': 0,
        'recording': 'playback.e9k',
        'initial_state': 'initial_state.bin',
        'meaning': 'Recorded menu selection 1 starts Demonstration Flight; no flight controls are supplied.',
        'hashes': {name: sha(capture / name) for name in
                   ['initial_state.bin', 'config.uae', 'playback.e9k', 'toolchain.json', 'state.bin', 'chip.bin', 'slow.bin']},
    }
    (capture / 'run.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(json.dumps({'capture': str(capture), 'frames': args.frames,
                      'human_input_events': 0, 'video_sha256': json.loads((capture / 'snapshot.json').read_text())['video_sha256']}, indent=2))


if __name__ == '__main__':
    main()
