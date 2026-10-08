"""Compare scene/menu retained sorting against complete original callers.

The native runtime receives ordinary menu keys and actual earned pilot saves.
Original comparisons start from observed native before-states; this does not
claim independent original whole-flight acceptance.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys

from capture_workspace import CaptureWorkspace, retain_failure
from tour_pilot_fixture import load_tour_pilot

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', type=Path, default=ROOT / 'build/native-cmake/native/Release/fa18_native_context_sort_test.exe')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-flight/context-sort-check')
    args = parser.parse_args()
    work = args.out.resolve()
    cases, stages = [], set()
    negative_checked = False
    with CaptureWorkspace(work) as ram:
        oracle = ROOT / 'build/recomp/native_mode_entry_oracle.exe'
        build = subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output',
            str(oracle.relative_to(ROOT)), '--main', 'tools/native/native_mode_entry_oracle.c'],
            cwd=ROOT, capture_output=True, text=True)
        (work / 'source-build.log').write_text(build.stdout + build.stderr)
        if build.returncode:
            raise RuntimeError(build.stderr or build.stdout)
        for mode in (2, 3, 4, 5, 6, 7, 8, 125, 127):
            prefix, pilot = ram / f'mode{mode}', ram / f'pilot{mode}'
            pilot.mkdir()
            config = load_tour_pilot(mode) if mode >= 5 and mode <= 8 else None
            if config:
                (pilot / 'config').write_bytes(config)
            run = subprocess.run([str(args.test.resolve()), str(ROOT / 'local/media/fa18.adf'),
                str(pilot), str(prefix), str(mode)], cwd=ROOT, capture_output=True,
                text=True, timeout=60)
            (work / f'native-{mode}.log').write_text(run.stdout + run.stderr)
            if run.returncode:
                raise RuntimeError(run.stderr or run.stdout)
            entries = [json.loads(line) for line in run.stdout.splitlines()]
            assert entries, mode
            with (work / f'source-{mode}.log').open('w') as log:
                for item in entries:
                    base = f'{prefix}.{item["entry"]}'
                    command = [str(oracle), base + '.before.dat', base + '.after.dat',
                               str(item['tick']), *map(str, item['keys'])]
                    env = dict(os.environ, FA18_MODE_STAGE_TRACE='1',
                               FA18_MODE_RETAINED_BEFORE=str(item['retained_before']),
                               FA18_MODE_RETAINED_AFTER=str(item['retained_after']))
                    env.pop('FA18_MODE_CONTEXT_FACTOR', None)
                    check = subprocess.run(command, cwd=ROOT, env=env,
                                           capture_output=True, text=True, timeout=15)
                    log.write(check.stdout + check.stderr)
                    log.flush()
                    if check.returncode:
                        retain_failure(base, work)
                        raise RuntimeError(check.stderr or check.stdout)
                    result = re.search(r'Stage retained sort: (\d+) lists, (\d+) cached entries, (\d+) fixed-far entries, source=([0-9A-F]+) native=([0-9A-F]+)', check.stdout)
                    assert result and result[4] == result[5], check.stdout
                    lists = int(result[1])
                    if lists and not negative_checked:
                        env['FA18_MODE_RETAINED_AFTER'] = str(item['retained_after'] ^ 1)
                        negative = subprocess.run(command, cwd=ROOT, env=env,
                                                  capture_output=True, text=True, timeout=15)
                        assert negative.returncode != 0, 'Changed retained output was accepted'
                        (work / 'negative-control.log').write_text(negative.stdout + negative.stderr)
                        negative_checked = True
                        env['FA18_MODE_RETAINED_AFTER'] = str(item['retained_after'])
                    probes = []
                    if lists:
                        for factor in ('0xa9f01357', '0xffff2468'):
                            env['FA18_MODE_CONTEXT_FACTOR'] = factor
                            probe = subprocess.run(command, cwd=ROOT, env=env,
                                                   capture_output=True, text=True, timeout=15)
                            log.write(f'Original-only incoming-factor probe {factor}\n' + probe.stdout + probe.stderr)
                            log.flush()
                            if probe.returncode:
                                retain_failure(base, work)
                                raise RuntimeError(f'Incoming factor still matters: {probe.stderr or probe.stdout}')
                            probes.append(factor)
                    stages.add(item['stage'])
                    record = dict(item, mode=mode, source_lists=lists,
                                  cached_entries=int(result[2]), fixed_far_entries=int(result[3]),
                                  original_only_factor_probes=probes,
                                  initial_config_sha256=hashlib.sha256(config).hexdigest() if config else 'ADF pilot',
                                  before_sha256=hashlib.sha256(Path(base + '.before.dat').read_bytes()).hexdigest(),
                                  after_sha256=hashlib.sha256(Path(base + '.after.dat').read_bytes()).hexdigest())
                    cases.append(record)
                    for suffix in ('before', 'after'):
                        Path(base + f'.{suffix}.dat').unlink()
            print(f'Mode {mode}: {len(entries)} scene/menu intervals match original RAM and retained sorting', flush=True)
        assert negative_checked and {'C0FECE', 'C0F992'} <= stages, stages
        assert sum(case['source_lists'] for case in cases) >= 27, 'Expected scene/menu sorting was not reached'
        for mode in (2, 3, 4, 5, 6, 7, 8, 127):
            assert any(case['mode'] == mode and case['source_lists'] >= 3 for case in cases), mode
        report = {'scope': 'actual native scene/menu before-state original caller comparisons',
                  'runtime_gameplay_changed': False, 'negative_control_rejected': negative_checked,
                  'test_sha256': hashlib.sha256(args.test.resolve().read_bytes()).hexdigest(),
                  'adf_sha256': hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest(),
                  'cases': cases}
        (work / 'comparison.json').write_text(json.dumps(report, indent=2) + '\n')


if __name__ == '__main__':
    main()
