"""Check ordinary-key independent cameras and their retained depth-sort output.

Full original frame bodies execute from actual native before-states. The
component oracle separately covers matrix arithmetic, view, projection and
cached-only sorting. Independent complete-flight acceptance remains separate.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

from capture_workspace import CaptureWorkspace, retain_failure

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/view-depth-check')
    args = parser.parse_args()
    work = args.out.resolve()
    with CaptureWorkspace(work) as ram:
        oracles = {}
        for name in ('view_depth', 'frame_body'):
            oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
            build = subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output',
                str(oracle.relative_to(ROOT)), '--main', f'tools/native/native_{name}_oracle.c'],
                cwd=ROOT, capture_output=True, text=True)
            (work / f'{name}-build.log').write_text(build.stdout + build.stderr)
            assert build.returncode == 0, build.stderr or build.stdout
            oracles[name] = oracle
        inputs = work / 'input.fa18in'
        inputs.write_text('FA18_GAME_INPUT_V1\nend 3500 0\n')
        startup = [(1800, 1802, 32), (3000, 3002, 50), (4100, 4102, 13),
                   (5000, 5002, 50), (5400, 5402, 49)]
        cases = [('map', [(6200, 6202, 77)], 3200),
                 ('follow-left', [(5500, 5502, 291), (6100, 6140, 274),
                                  (6200, 6360, 44), (6600, 6602, 271), (6800, 6802, 260)], 3400),
                 ('follow-right', [(5500, 5502, 291), (6100, 6140, 274),
                                   (6200, 6360, 46), (6600, 6602, 271), (6800, 6802, 257)], 3400)]
        bodies, sorted_lists, cached_entries, far_entries, words = [], 0, 0, 0, set()
        component_checked = False
        with (work / 'frame-body-check.log').open('w') as log:
            for name, events, first in cases:
                keys = work / f'{name}.e9k'
                lines = [(tick, key, down) for begin, end, key in startup + events
                         for tick, down in ((begin, 1), (end, 0))]
                keys.write_text('E9K_INPUT_V1\n' + ''.join(
                    f'F {tick} K {key} 0 0 {down}\n' for tick, key, down in sorted(lines)))
                base = ram / name
                run = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '30000',
                    '--input', str(inputs), '--iterations', str(first + 16), '--replay', str(keys),
                    '--save-dir', str(ram / f'{name}-pilot'), '--frame-capture', f'{first}+16', str(base)],
                    cwd=ROOT, capture_output=True, text=True, timeout=35)
                assert run.returncode == 0, run.stderr or run.stdout
                stats = json.loads(run.stdout)
                assert stats['frame_capture_complete'] and not stats['frame_owner_exit'], stats
                assert stats['mode'] == 1 and stats['stage'] == 'C10DAE', stats
                assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
                for iteration in range(first, first + 16):
                    prefix = Path(str(base) + f'.{iteration}')
                    timing = json.loads(Path(str(prefix) + '.timing.json').read_text())
                    assert timing['iteration'] == iteration and not timing['owner_exit'], timing
                    before = Path(str(prefix) + '.before.dat').read_bytes()
                    assert before[0xc45785 - 0xc00000 + 0x80000], 'Independent view not reached'
                    if not component_checked:
                        component = subprocess.run([str(oracles['view_depth']), str(prefix) + '.before.dat'],
                            cwd=ROOT, capture_output=True, text=True, timeout=15)
                        (work / 'view-depth-check.log').write_text(component.stdout + component.stderr)
                        if component.returncode:
                            retain_failure(prefix, work)
                            raise RuntimeError(component.stderr or component.stdout)
                        component_checked = True
                    check = subprocess.run([str(oracles['frame_body']), str(prefix) + '.before.dat',
                        str(prefix) + '.after.dat', str(timing['before_tick']), str(timing['after_tick']),
                        str(timing['saved_tick']), str(prefix) + '.source.dat'],
                        cwd=ROOT, capture_output=True, text=True, timeout=15)
                    log.write(f'{name} iteration {iteration} PAL {timing["before_tick"]}..{timing["after_tick"]} '
                              f'saved {timing["saved_tick"]}\n' + check.stdout + check.stderr)
                    log.flush()
                    if check.returncode:
                        retain_failure(prefix, work)
                        raise RuntimeError(check.stderr or check.stdout)
                    depth = re.search(r'Context depth: (\d+) sorted lists, (\d+) cached entries, (\d+) fixed-far entries, incoming word=([0-9A-F]+)', check.stdout)
                    assert depth, check.stdout
                    sorted_lists += int(depth[1])
                    cached_entries += int(depth[2])
                    far_entries += int(depth[3])
                    words.add(depth[4])
                    bodies.append({'case': name, 'iteration': iteration,
                                   'before_tick': timing['before_tick'], 'after_tick': timing['after_tick'],
                                   'sorted_lists': int(depth[1]), 'cached_entries': int(depth[2]),
                                   'fixed_far_entries': int(depth[3]), 'incoming_word': depth[4]})
                    for path in ram.glob(prefix.name + '.*.dat'):
                        path.unlink()
                print(f'{name}: 16 consecutive actual runtime bodies match original RAM/drawing', flush=True)
        assert sorted_lists and cached_entries, 'Cached sorting not exercised in the actual camera bodies'
        assert words == {'0000', 'FFFF'}, f'Both signs of the context factor required: {words}'
        report = {'scenario': 'ordinary-key-independent-camera-depth', 'bodies': bodies,
                  'matrix_cases': 128, 'view_projection_cached_sort_cases': 512,
                  'source_sorted_lists': sorted_lists, 'source_cached_entries': cached_entries,
                  'source_fixed_far_entries': far_entries,
                  'incoming_words': sorted(words), 'native_flight_state_seeded': False,
                  'comparison_masks_changed': False, 'independent_full_flight_accepted': False,
                  'runner_sha256': hashlib.sha256(args.runner.read_bytes()).hexdigest(),
                  'adf_sha256': hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest()}
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'640 complete component cases and {len(bodies)} actual camera bodies pass; '
          f'{sorted_lists} source sort batches, {cached_entries} cached and {far_entries} fixed-far entries, both factor signs')


if __name__ == '__main__':
    main()
