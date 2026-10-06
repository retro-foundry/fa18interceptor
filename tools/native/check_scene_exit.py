"""Check C0DA38's native frontend exit against the complete original frame prefix.

Disk/input create native game state; controlled branch/message inputs exist only
in the validation entry. No original recording is replayed. Copper fade excluded.
"""
import argparse
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path,
                        default=ROOT / 'build/native-cmake/native/Release/fa18_native_scene_exit_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/scene-exit-check')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    prefix = args.out.resolve() / 'frame'
    result = subprocess.run([str(args.runner.resolve()), str(ROOT / 'local/media/fa18.adf'),
                             str(ROOT / 'captures/native/demo01/input.fa18in'),
                             str(args.out.resolve() / 'pilot'), str(prefix)],
                            cwd=ROOT, check=True, capture_output=True, text=True, timeout=25)
    stats = json.loads(result.stdout)
    assert stats['owner_exit'] and stats['before_tick'] == stats['after_tick'], stats
    (args.out / 'capture.json').write_text(json.dumps(stats, indent=2) + '\n')
    for name in ('native_model_oracle', 'native_frame_body_oracle'):
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', f'build/recomp/{name}.exe',
                        '--main', f'tools/native/{name}.c'], cwd=ROOT, check=True)
        if name == 'native_model_oracle':
            command = [str(ROOT / f'build/recomp/{name}.exe'), str(prefix) + '.before.dat']
        else:
            command = [str(ROOT / f'build/recomp/{name}.exe'), str(prefix) + '.before.dat',
                       str(prefix) + '.after.dat', str(stats['before_tick']), str(stats['after_tick']),
                       str(stats['saved_tick']), str(prefix) + '.source.dat', 'owner-exit']
        subprocess.run(command, cwd=ROOT, check=True, timeout=20)
    print('Native frontend exits before HUD/timer/counter/message, resumes display, and matches the original frame prefix')


if __name__ == '__main__':
    main()
