"""Bounded runner-frame check for native autopilot steering calls.

Build record_autopilot_frame_main.c with record_autopilot_frame_machine.c,
structural_write_log.c and flight_record_actions_bus_budget.c replacements.
The frame fixture masks interrupts and stops at the source child return;
this checks call integration, not sealed gameplay or instruction timing.
"""
import argparse
import json
import os
from pathlib import Path
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', default='build/recomp/record_autopilot_frame.exe')
    parser.add_argument('--output', default='build/steering-calls/frame')
    args = parser.parse_args()
    root = Path(args.output)
    root.mkdir(parents=True, exist_ok=True)
    rows = []
    for case, entry in [(1, 'C2CA92'), (14, 'C2CBBC'), (23, 'C2CB82'),
                        (46, 'C2CB86'), (648, 'C2CAA0'), (1125, 'C2CA26')]:
        profiles = {}
        for mode in ['off', 'on']:
            stem = root / f'{case}-{mode}'
            command = [args.exe, '--state', 'captures/native/demo01/state.bin',
                       '--rom', 'local/system/kick13.rom', '--frames', '1',
                       '--ports', mode, '--profile', str(stem) + '.json',
                       '--ram-out', str(stem) + '.ram', '--rgb444', str(stem) + '.rgb',
                       '--index8', str(stem) + '.idx']
            result = subprocess.run(command, env=dict(os.environ, FA18_RECORD_ACTION_CASE=str(case)),
                                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=30)
            Path(str(stem) + '.log').write_bytes(result.stdout)
            if result.returncode:
                raise RuntimeError(f'case {case}, {mode}: {result.stdout.decode(errors="replace")}')
            profiles[mode] = json.loads(Path(str(stem) + '.json').read_text())
            stats = json.loads(result.stdout.decode().strip().splitlines()[-1])
            if stats['pc'] != 'C25C70' or stats['frames'] != 1:
                raise AssertionError(f'case {case}, {mode}: source child did not return')
        edges = profiles['on']['_native_edges']
        for caller, callee in [('C25B66', 'C2C392'), ('C2C392', entry)]:
            if not any(row['caller'] == caller and row['callee'] == callee and row['calls']
                       for row in edges):
                raise AssertionError(f'case {case}: native edge {caller} -> {callee} absent')
        if profiles['on'].get(entry, 0):
            raise AssertionError(f'case {case}: steering CPU entry dispatched')
        for extension in ['ram', 'rgb', 'idx']:
            before = Path(str(root / f'{case}-off') + '.' + extension).read_bytes()
            after = Path(str(root / f'{case}-on') + '.' + extension).read_bytes()
            if not before or before != after:
                raise AssertionError(f'case {case}: {extension} differs from source')
        rows.append({'case': case, 'entry': entry, 'native_edges': edges,
                     'ram_rgb_indices_identical': True, 'source_return': 'C25C70'})
    (root / 'results.json').write_text(json.dumps(rows, indent=2) + '\n')
    print('autopilot frame integration: 6/6 source-return/RAM/RGB/index pairs pass')


if __name__ == '__main__':
    main()
