"""Capture complete independent original entries and drawing owners compactly.

The reference-only recorder forwards the original bus and loop unchanged.
Every preceding trace row, full final RAM and runtime counter must stay exact.
"""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer
from check_qualification_message_cadence import digest, verify_trace
from compare_flight_traces import read_trace
from frame_delta import snapshots

BOUNDARIES = (0xC0EFD4, 0xC0EFEA, 0xC0F3C0, 0xC30764, 0xC0F182,
              0xC31226, 0xC0F18E, 0xC322EE, 0xC0F286, 0xC0F2DC)


def verify_owner_return(snapshot, registers, pending):
    """Require actual call/return PCs and the restored caller stack pointer."""
    pc, iteration = snapshot['boundary'], snapshot['iteration']
    if pc in (0xC0EFD4, 0xC0EFEA, 0xC0F3C0):
        assert not pending, 'Original body boundary interrupted an owner capture'
    if pc in (0xC30764, 0xC31226, 0xC322EE):
        owner = {0xC30764: 'panel', 0xC31226: 'radar', 0xC322EE: 'message'}[pc]
        stack = registers['registers'][15]
        returned = integer(snapshot['data'], stack, 4)
        assert returned in {'panel': (0xC0F182,), 'radar': (0xC0F18E,), 'message': (0xC0F286, 0xC0F2DC)}[owner]
        assert owner not in pending, 'Repeated original owner entry without return'
        pending[owner] = iteration, returned, stack + 4
    elif pc in (0xC0F182, 0xC0F18E, 0xC0F286, 0xC0F2DC):
        owner = 'panel' if pc == 0xC0F182 else 'radar' if pc == 0xC0F18E else 'message'
        assert owner in pending, 'Original owner return has no entry'
        assert pending.pop(owner) == (iteration, pc, registers['registers'][15]), 'Original owner returned to another caller'
        return 1
    return 0


def verify_owner_stream(directory):
    report = json.loads((directory / 'report.json').read_text())
    raw = gzip.decompress((directory / 'registers.jsonl.gz').read_bytes())
    assert digest(raw) == report['metadata_sha256']
    lines = iter(raw.splitlines())
    identities = {(r['iteration'], r['pc']): r for r in report['identities']}
    pending, count, returns, wrong_caller = {}, 0, 0, False
    for snapshot in snapshots(directory / 'frames.delta.gz', BOUNDARIES):
        registers = json.loads(next(lines))
        i, pc = snapshot['iteration'], snapshot['boundary']
        assert (registers['iteration'], registers['frame'], registers['pc']) == (i, snapshot['frame'], pc)
        assert snapshot['ram_sha256'] == identities[i, pc]['ram_sha256']
        if pc in (0xC0F182, 0xC0F18E, 0xC0F286, 0xC0F2DC) and not wrong_caller:
            bad = dict(registers, registers=list(registers['registers']))
            bad['registers'][15] += 4
            try:
                verify_owner_return(snapshot, bad, dict(pending))
            except AssertionError as error:
                assert str(error) == 'Original owner returned to another caller'
                wrong_caller = True
            else:
                raise AssertionError('Wrong caller stack accepted')
        returns += verify_owner_return(snapshot, registers, pending)
        count += 1
    assert not pending and next(lines, None) is None and count == report['snapshots']
    assert wrong_caller
    assert returns == sum(report['boundary_counts'].get(f'{pc:06X}', 0) for pc in (0xC0F182, 0xC0F18E, 0xC0F286, 0xC0F2DC))
    return dict(snapshots=count, actual_owner_returns_matching=returns, wrong_caller_stack_rejected=True,
                source_report_sha256=digest((directory / 'report.json').read_bytes()),
                metadata_sha256=report['metadata_sha256'])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('source-evidence', 'reference', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    parser.add_argument('--first', type=int)
    parser.add_argument('--count', type=int)
    parser.add_argument('--window-radar', type=Path)
    parser.add_argument('--window-message', type=Path)
    parser.add_argument('--verify-existing', type=Path, help='Verify all actual caller returns in a preserved stream without rerunning')
    parser.add_argument('--previous-stream', type=Path, help='Require every old RAM/register boundary to remain byte-identical')
    args = parser.parse_args()
    assert (args.first is None) == (args.count is None)
    reference = json.loads((args.reference / 'report.json').read_text())
    source = json.loads((args.source_evidence / 'report.json').read_text())
    assert reference['baseline_source_trace_sha256'] == source['driver_trace_sha256']
    recorded = args.reference / 'source.jsonl.gz'
    assert digest(gzip.decompress(recorded.read_bytes())) == reference['source_trace_sha256']
    header, rows = read_trace(recorded)
    for path, expected in reference['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected, path
    input_path = args.source_evidence / 'input.fa18in'
    assert digest(input_path.read_bytes()) == source['generated_input_sha256']
    first = args.first if args.first is not None else reference['whole_successful_flight']['first']
    last = first + args.count - 1 if args.count is not None else reference['whole_successful_flight']['last']
    assert 1 <= first <= last <= max(rows)
    if args.verify_existing:
        captured = json.loads((args.verify_existing / 'report.json').read_text())
        assert captured['source_trace_sha256'] == reference['source_trace_sha256']
        assert captured['complete_trace_preserved'] and captured['final_ram_preserved'] and captured['runtime_counters_preserved']
        assert (captured['first'], captured['last']) == (first, last)
        result = verify_owner_stream(args.verify_existing)
        args.out.mkdir(parents=True, exist_ok=True)
        (args.out / 'report.json').write_text(json.dumps(result, indent=2) + '\n')
        print(f"{result['actual_owner_returns_matching']} actual original caller returns verified; wrong caller rejected")
        return
    probe_sources = ('tools/native/original_frame_delta_probe.c', 'tools/native/original_frame_delta_loop.c',
                     'port/native/frame_delta.c', 'port/native/frame_delta.h')
    source_hashes = {path: digest((ROOT / path).read_bytes()) for path in probe_sources}
    args.out.mkdir(parents=True, exist_ok=True)
    probe = ROOT / 'build/recomp/fa18_original_frame_delta_probe.exe'
    with (args.out / 'build.log').open('w') as log:
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(probe.relative_to(ROOT)),
            '--replace-source', 'port/machine/bus.c=tools/native/original_frame_delta_probe.c',
            '--replace-source', 'port/recomp/loop_input.c=tools/native/original_frame_delta_loop.c'],
            cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True)
    probe_hash = digest(probe.read_bytes())
    env = {k: v for k, v in os.environ.items() if not k.startswith(
        ('FA18_LOOP_', 'FA18_BOUNDARY_', 'FA18_UPDATE_ENTRY_', 'FA18_ORIGINAL_', 'FA18_TRACE_'))}
    identities, counts = [], {f'{pc:06X}': 0 for pc in BOUNDARIES}
    windows = {}
    for name, directory in (('radar', args.window_radar), ('message', args.window_message)):
        if directory:
            report = json.loads((directory / 'report.json').read_text())
            assert report['source_trace_sha256'] == source['driver_trace_sha256']
            windows[name] = directory, {r['iteration']: r['snapshots'] for r in report['captures']}
    checked_old = 0
    previous_snapshots = previous_lines = None
    checked_previous = 0
    if args.previous_stream:
        previous_report = json.loads((args.previous_stream / 'report.json').read_text())
        assert previous_report['source_trace_sha256'] == reference['source_trace_sha256']
        previous_metadata = gzip.decompress((args.previous_stream / 'registers.jsonl.gz').read_bytes())
        assert digest(previous_metadata) == previous_report['metadata_sha256']
        previous_hash = hashlib.sha256()
        with gzip.open(args.previous_stream / 'frames.delta.gz', 'rb') as old_stream:
            while chunk := old_stream.read(1024 * 1024):
                previous_hash.update(chunk)
        assert previous_hash.hexdigest() == previous_report['stream_sha256']
        previous_lines = iter(previous_metadata.splitlines())
        previous_snapshots = iter(snapshots(args.previous_stream / 'frames.delta.gz', BOUNDARIES))
        previous_markers = {int(pc, 16) for pc, hits in previous_report['boundary_counts'].items() if hits}
    with tempfile.TemporaryDirectory(prefix='original-frame-delta-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        stream, metadata, trace, final = [work / name for name in ('frames.delta', 'registers.jsonl', 'trace.jsonl', 'final.dat')]
        end = input_path.read_text().splitlines()[-1].split()
        assert end[0] == 'end'
        with (args.out / 'source.log').open('w') as log:
            result = subprocess.run([str(probe), '--state', str(ROOT / 'captures/native/qual_carrier_success/state.bin'),
                '--rom', str(ROOT / 'local/system/kick13.rom'), '--ports', 'off', '--input', str(input_path.resolve()),
                '--frames', end[2], '--ram-out', str(final)], cwd=ROOT,
                env=dict(env, FA18_ORIGINAL_DELTA_RANGE=f'{first}+{last-first+1}',
                    FA18_ORIGINAL_DELTA_PATH=str(stream), FA18_ORIGINAL_DELTA_REGISTERS=str(metadata),
                    FA18_LOOP_TRACE=str(trace), FA18_LOOP_TRACE_BUDGET_MIB='256',
                    FA18_TRACE_MESSAGE_FIELDS='1', FA18_TRACE_DRAWING_BANDS='1'),
                stdout=log, stderr=subprocess.STDOUT, timeout=900)
        assert result.returncode == 0, 'Original probe failed'
        actual_run, = [json.loads(line) for line in (args.out / 'source.log').read_text().splitlines() if line.startswith('{')]
        assert actual_run == reference['source_run'], 'Original runtime counters changed'
        assert digest(final.read_bytes()) == source['driver_final_ram_sha256'] == reference['source_final_ram_sha256']
        assert trace.read_bytes() == gzip.decompress(recorded.read_bytes()), 'Original trace changed'
        assert stream.stat().st_size + metadata.stat().st_size + trace.stat().st_size <= 512 * 1024 * 1024
        entries, old_seen, pending = [], set(), {}
        owner_returns = 0
        with metadata.open() as registers_file:
            for snapshot in snapshots(stream, BOUNDARIES):
                registers = json.loads(next(registers_file))
                i, pc, data = snapshot['iteration'], snapshot['boundary'], snapshot['data']
                assert first <= i <= last and (i, pc) not in old_seen
                old_seen.add((i, pc))
                assert (registers['iteration'], registers['frame'], registers['pc']) == (i, snapshot['frame'], pc)
                assert len(registers['registers']) == 16
                owner_returns += verify_owner_return(snapshot, registers, pending)
                if pc == 0xC0EFD4:
                    assert snapshot['frame'] == rows[i]['frame']
                    verify_trace(data, header, rows[i]); entries.append(i)
                if previous_snapshots is not None and pc in previous_markers:
                    old, old_registers = next(previous_snapshots), json.loads(next(previous_lines))
                    assert (snapshot['iteration'], snapshot['frame'], snapshot['boundary'], snapshot['ram_sha256']) == (
                        old['iteration'], old['frame'], old['boundary'], old['ram_sha256'])
                    assert data == old['data'] and registers == old_registers, 'Previous actual RAM/registers changed'
                    checked_previous += 1
                if pc in (0xC30764, 0xC31226, 0xC322EE):
                    returned = integer(data, registers['registers'][15], 4)
                    assert returned in {0xC30764: (0xC0F182,), 0xC31226: (0xC0F18E,), 0xC322EE: (0xC0F286, 0xC0F2DC)}[pc]
                    registers['stack_return'] = returned
                for owner, (path, captures) in windows.items():
                    suffix = {0xC0EFEA: 'before', 0xC0F3C0: 'after',
                        (0xC31226 if owner == 'radar' else 0xC322EE): 'owner',
                        (0xC0F18E if owner == 'radar' else 0xC0F286): 'owner-after'}.get(pc)
                    if suffix is not None and i in captures:
                        expected = captures[i][suffix]
                        assert snapshot['ram_sha256'] == expected['ram_sha256']
                        assert registers == expected['registers']
                        assert data == gzip.decompress((path / f'source-body.{i}.{suffix}.dat.gz').read_bytes())
                        checked_old += 1
                counts[f'{pc:06X}'] += 1
                identities.append(dict(iteration=i, pc=pc, frame=snapshot['frame'], ram_sha256=snapshot['ram_sha256']))
            assert not registers_file.read(), 'Metadata follows stream terminal count'
        assert not pending, 'Original stream ended inside an owner'
        if previous_snapshots is not None:
            assert next(previous_snapshots, None) is None and next(previous_lines, None) is None
            assert checked_previous == previous_report['snapshots']
        assert entries == list(range(first, last + 1)), 'Incomplete original input observations'
        if windows:
            assert checked_old == sum(4 * len(captures) for _, captures in windows.values())
        stream_hash = digest(stream.read_bytes())
        metadata_hash = digest(metadata.read_bytes())
        stream_bytes, metadata_bytes, trace_bytes = (p.stat().st_size for p in (stream, metadata, trace))
        for path in (stream, metadata):
            destination = args.out / (path.name + '.gz')
            with path.open('rb') as source_file, gzip.GzipFile(filename=str(destination), mode='wb', mtime=0) as target:
                while chunk := source_file.read(1024 * 1024):
                    target.write(chunk)
    assert digest(probe.read_bytes()) == probe_hash, 'Probe changed during capture'
    assert {path: digest((ROOT / path).read_bytes()) for path in probe_sources} == source_hashes, 'Probe source changed during capture'
    report = dict(first=first, last=last, observations=len(entries), snapshots=len(identities), boundary_counts=counts,
        probe_sha256=probe_hash, probe_source_sha256=digest((ROOT / 'tools/native/original_frame_delta_probe.c').read_bytes()),
        loop_source_sha256=digest((ROOT / 'tools/native/original_frame_delta_loop.c').read_bytes()),
        capture_sources_sha256=source_hashes,
        source_trace_sha256=reference['source_trace_sha256'], source_input_sha256=source['generated_input_sha256'],
        unchanged_observations=len(rows), complete_trace_preserved=True, final_ram_preserved=True,
        runtime_counters_preserved=True, old_window_snapshots_identical=checked_old,
        previous_stream_snapshots_identical=checked_previous,
        actual_owner_returns_matching=owner_returns,
        stream_sha256=stream_hash, metadata_sha256=metadata_hash,
        stream_bytes=stream_bytes, metadata_bytes=metadata_bytes, trace_bytes=trace_bytes,
        retained_bytes=sum((args.out / name).stat().st_size for name in ('frames.delta.gz', 'registers.jsonl.gz')),
        identities=identities, scope='Complete independent original input and actual drawing-owner RAM/registers. Source replay, all trace rows, counters and final RAM remain exact. No original RAM supplies native gameplay. This captures evidence; drawing-history and complete sound acceptance remain separate.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{len(entries)} original observations / {len(identities)} snapshots; complete replay exact; {checked_old} previous snapshots identical')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
