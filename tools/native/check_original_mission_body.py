"""Export an actual original frame body without changing its bus or input.

Every preceding observation must reproduce the independently recorded flight.
Only the requested before/after RAM and registers are retained, compressed.
"""
import argparse
import gzip
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer
from check_qualification_message_cadence import digest
from compare_flight_traces import read_trace


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--iteration', type=int, required=True)
    parser.add_argument('--count', type=int, default=1)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--owner', type=lambda value: int(value, 16))
    parser.add_argument('--owner-return', type=lambda value: int(value, 16))
    args = parser.parse_args()
    assert (args.owner is None) == (args.owner_return is None)
    assert 1 <= args.count <= 128
    last=args.iteration+args.count-1
    args.out.mkdir(parents=True, exist_ok=True)
    evidence = json.loads((args.source_evidence / 'report.json').read_text())
    recorded = args.source_evidence / 'driver.jsonl.gz'
    assert digest(gzip.decompress(recorded.read_bytes())) == evidence['driver_trace_sha256']
    header, rows = read_trace(recorded)
    assert all(i in rows for i in range(args.iteration,last+2))
    for path, expected in evidence['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected, path
    probe = ROOT / 'build/recomp/fa18_original_frame_probe.exe'
    probe_source=ROOT / 'tools/native/original_frame_probe.c'
    probe_source_hash=digest(probe_source.read_bytes())
    with (args.out / 'build.log').open('w') as log:
        subprocess.run(['python', 'scripts/build_recomp.py', '--output',
            str(probe.relative_to(ROOT)), '--replace-source',
            'port/machine/bus.c=tools/native/original_frame_probe.c'], cwd=ROOT,
            stdout=log, stderr=subprocess.STDOUT, check=True)
    env = {k: v for k, v in os.environ.items() if not k.startswith(
        ('FA18_LOOP_', 'FA18_BOUNDARY_', 'FA18_UPDATE_ENTRY_', 'FA18_ORIGINAL_'))}
    with tempfile.TemporaryDirectory(prefix='original-mission-body-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        prefix = work / 'body'
        trace = work / 'trace.jsonl'
        if args.owner is not None:
            env.update(FA18_ORIGINAL_BODY_OWNER=f'{args.owner:X}',
                       FA18_ORIGINAL_BODY_OWNER_RETURN=f'{args.owner_return:X}')
        command = [str(probe), '--state',
            str(ROOT / 'captures/native/qual_carrier_success/state.bin'), '--rom',
            str(ROOT / 'local/system/kick13.rom'), '--ports', 'off', '--input',
            str((args.source_evidence / 'input.fa18in').resolve()), '--frames',
            str(rows[last + 1]['frame'] + 1)]
        with (args.out / 'source.log').open('w') as log:
            result = subprocess.run(command, cwd=ROOT, env=dict(env,
                FA18_ORIGINAL_BODY_ITERATION=str(args.iteration),
                FA18_ORIGINAL_BODY_COUNT=str(args.count),
                FA18_ORIGINAL_BODY_PREFIX=str(prefix), FA18_LOOP_TRACE=str(trace)),
                stdout=log, stderr=subprocess.STDOUT, timeout=300)
        assert result.returncode == 0
        actual_header, actual = read_trace(trace)
        assert actual_header == header
        assert max(actual) >= last + 1
        assert all(row == rows[i] for i, row in actual.items()), 'original trace changed'
        captures = []
        boundaries = [('before', 0xC0EFEA), ('after', 0xC0F3C0)]
        if args.owner is not None:
            boundaries.extend([('owner', args.owner), ('owner-after', args.owner_return)])
        for i in range(args.iteration,last+1):
            snapshots = {}
            for suffix, pc in boundaries:
                name=f'body.{i}' if args.count>1 else 'body'
                registers = json.loads((work / f'{name}.{suffix}.json').read_text())
                assert registers['iteration'] == i and registers['pc'] == pc
                data = (work / f'{name}.{suffix}.dat').read_bytes()
                assert len(data) == 0x100000
                if suffix == 'owner':
                    # A later caller label may include another drawing child.
                    # Require the requested end to be this call's actual return.
                    stack_return = integer(data, registers['registers'][15], 4)
                    assert stack_return == args.owner_return, (
                        f'Owner {args.owner:06X} returns to {stack_return:06X}, '
                        f'not requested {args.owner_return:06X}')
                    registers['stack_return'] = stack_return
                path = args.out / f'source-body.{i}.{suffix}.dat.gz'
                path.write_bytes(gzip.compress(data, mtime=0))
                (args.out / f'source-body.{i}.{suffix}.json').write_text(
                    json.dumps(registers, indent=2) + '\n')
                snapshots[suffix] = dict(ram_sha256=digest(data), registers=registers)
            captures.append(dict(iteration=i,snapshots=snapshots))
        report = dict(iteration=args.iteration, probe_sha256=digest(probe.read_bytes()),
            probe_source_sha256=probe_source_hash,
            source_trace_sha256=evidence['driver_trace_sha256'],
            source_input_sha256=digest((args.source_evidence / 'input.fa18in').read_bytes()),
            unchanged_observations=len(actual), last_observation=max(actual), captures=captures,
            scope='Actual original instruction boundaries, registers and complete RAM. '
                  'Every observation reproduces the original recording; original bus operations '
                  'and controls are forwarded unchanged. No native gameplay is seeded from RAM.')
        assert digest(probe_source.read_bytes()) == probe_source_hash, 'probe source changed during capture'
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'Original bodies {args.iteration}..{last}: {len(actual)} unchanged observations; '
          'before/after RAM and actual registers retained compressed')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
