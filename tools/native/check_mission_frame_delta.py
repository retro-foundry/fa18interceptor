"""Capture complete actual native bodies compactly and check original execution.

The games start independently. Existing input-boundary traces, complete final
RAM, counters and earned pilot remain unchanged. No captured state feeds the
native runner. Only the external instruction oracle consumes decoded bodies.
"""
import argparse
import gzip
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT
from check_qualification_message_cadence import digest, verify_trace
from check_recorded_original_mission_trace import verified_update_mapping
from compare_flight_traces import read_trace
from frame_delta import bodies


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('runner', 'reference', 'source-evidence', 'source-updates', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    parser.add_argument('--first', type=int, help='Original flight observation; defaults to entire accepted flight')
    parser.add_argument('--count', type=int, help='Original observations, including real duplicate dispatch entries')
    parser.add_argument('--window', type=Path, help='Additionally require exact old entry/body RAM and metadata')
    args = parser.parse_args()
    assert (args.first is None) == (args.count is None)
    runner_hash = digest(args.runner.read_bytes())
    args.out.mkdir(parents=True, exist_ok=True)
    reference = json.loads((args.reference / 'report.json').read_text())
    original = json.loads((args.source_evidence / 'report.json').read_text())
    assert reference['whole_successful_flight']['strict_gameplay_matching']
    for path, expected in reference['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected, path
    mapping, update = verified_update_mapping(args.source_updates, args.source_evidence / 'driver.jsonl.gz', original)
    assert update == reference['source_update_evidence']
    trace_path = args.reference / 'native.jsonl.gz'
    assert digest(gzip.decompress(trace_path.read_bytes())) == reference['native_trace_sha256']
    header, rows = read_trace(trace_path)
    first = args.first if args.first is not None else reference['whole_successful_flight']['first']
    last = first + args.count - 1 if args.count is not None else reference['whole_successful_flight']['last']
    assert reference['whole_successful_flight']['first'] <= first <= last <= reference['whole_successful_flight']['last']
    nf, nl = mapping[first], mapping[last]
    assert set(mapping[i] for i in range(first, last + 1)) == set(range(nf, nl + 1))
    sealed_window = None
    if args.window:
        sealed_window = json.loads((args.window / 'report.json').read_text())
        assert (sealed_window['first'], sealed_window['last']) == (first, last)
    oracle = ROOT / 'build/recomp/native_frame_body_oracle.exe'
    with (args.out / 'oracle-build.log').open('w') as log:
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                        '--main', 'tools/native/native_frame_body_oracle.c'], cwd=ROOT, stdout=log,
                       stderr=subprocess.STDOUT, check=True)
    env = {k: v for k, v in os.environ.items() if not k.startswith(('FA18_FRAME_', 'FA18_TRACE_'))}
    identities, failures = [], []
    with tempfile.TemporaryDirectory(prefix='mission-frame-delta-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        pilot, stream, final = work / 'pilot', work / 'frames.delta', work / 'final.dat'
        def run(arguments, log_name):
            result = subprocess.run([str(args.runner.resolve()), '--headless', *arguments], cwd=ROOT,
                                    env=env, capture_output=True, text=True, timeout=300)
            (args.out / log_name).write_text(result.stdout + result.stderr)
            assert result.returncode == 0, log_name
            return json.loads(result.stdout)
        enlist = run(['--frames', '9000', '--replay', str(ROOT / 'tools/native/fixtures/region-pilot-enlist.e9k'),
                      '--save-dir', str(pilot)], 'enlist.log')
        assert enlist == reference['enlist_run'], 'Enlistment counters changed'
        initial = (pilot / 'config').read_bytes()
        assert len(initial) == 78 and initial[:4] == bytes(4)
        assert initial[18:28] == bytes(10) and initial[56:58] == bytes(2), 'Pilot was not newly enlisted'
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        summary = run(['--frames', str(reference['native_run']['frames']), '--replay', str(intro),
                       '--input', str((args.source_updates / 'update-consumed.fa18in').resolve()),
                       '--iterations', str(update['real_update_calls']), '--save-dir', str(pilot),
                       '--frame-delta', f'{nf}+{nl - nf + 1}', str(stream), '--data-out', str(final),
                       '--memory-report', str(args.out.resolve() / 'memory.json')], 'flight.log')
        assert summary == reference['native_run'], 'Diagnostic changed runtime counters'
        assert digest(final.read_bytes()) == reference['native_final_ram_sha256']
        assert (pilot / 'config').read_bytes().hex() == reference['final_saved_pilot']
        stream_bytes, stream_hash = stream.stat().st_size, digest(stream.read_bytes())
        old = {row['native_iteration']: row for row in sealed_window['rows']} if sealed_window else {}
        with (args.out / 'original-bodies.log').open('w') as log:
            for body in bodies(stream):
                entry, before, after = (body[key] for key in ('entry', 'before', 'after'))
                j = entry['iteration']
                assert nf <= j <= nl and j == nf + len(identities)
                assert entry['frame'] == rows[j]['frame']
                verify_trace(entry['data'], header, rows[j])
                if sealed_window:
                    expected = old[j]
                    assert entry['ram_sha256'] == expected['ram_sha256']['native']
                    for name, snapshot in (('before', before), ('after', after)):
                        assert snapshot['data'] == gzip.decompress((args.window / f'native.{j}.{name}.dat.gz').read_bytes())
                    assert (before['frame'], after['frame'], before['saved_tick']) == tuple(
                        expected['body_timing'][name] for name in ('before_tick', 'after_tick', 'saved_tick'))
                input_path, output_path = work / 'before.dat', work / 'after.dat'
                input_path.write_bytes(before['data']); output_path.write_bytes(after['data'])
                command = [str(oracle), str(input_path), str(output_path), str(before['frame']),
                           str(after['frame']), str(before['saved_tick'])]
                if after['boundary'] == 3:
                    command += [str(work / 'original-owner-exit.dat'), 'owner-exit']
                result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, text=True, timeout=20)
                log.write(f'Iteration {j}\n' + result.stdout + result.stderr)
                if result.returncode or '0 gameplay differences, 0 display bytes' not in result.stdout:
                    for name, snapshot in body.items():
                        (args.out / f'failed-{j}.{name}.dat.gz').write_bytes(gzip.compress(snapshot['data'], mtime=0))
                    failures.append(j)
                identities.append(dict(iteration=j, entry_sha256=entry['ram_sha256'],
                                       before_sha256=before['ram_sha256'], after_sha256=after['ram_sha256'],
                                       before_tick=before['frame'], after_tick=after['frame'],
                                       saved_tick=before['saved_tick'], original_body_matching=j not in failures))
                if len(identities) % 100 == 0:
                    print(f'{len(identities)}/{nl - nf + 1} complete bodies checked; failures={len(failures)}', flush=True)
        assert len(identities) == nl - nf + 1, 'Incomplete streamed flight range'
        retained = args.out / 'frames.delta.gz'
        with stream.open('rb') as source, gzip.GzipFile(filename=str(retained), mode='wb', mtime=0) as target:
            while chunk := source.read(1024 * 1024):
                target.write(chunk)
    assert digest(args.runner.read_bytes()) == runner_hash, 'Runner changed during verification'
    report = dict(first=first, last=last, native_first=nf, native_last=nl, bodies=len(identities),
                  runner_sha256=runner_hash, reference_report_sha256=digest((args.reference / 'report.json').read_bytes()),
                  native_trace_sha256=reference['native_trace_sha256'], native_final_ram_sha256=reference['native_final_ram_sha256'],
                  runtime_counters_preserved=True, final_ram_preserved=True, earned_save_preserved=True,
                  original_body_failures=failures, original_bodies_matching=not failures,
                  stream_bytes=stream_bytes, stream_sha256=stream_hash, compressed_bytes=retained.stat().st_size,
                  conventional_capture_bytes=3 * 0x100000 * len(identities), snapshots=3 * len(identities),
                  old_window_ram_and_metadata_identical=bool(sealed_window), identities=identities,
                  scope='Complete actual native input/begin/end RAM with exact old input trace and complete final flight preservation. Every decoded native body executes original instructions under the existing explicit frame-body exclusions and supplied original API timer contract. Independent source-versus-native cache histories and other full flights remain separate.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{len(identities)} complete bodies: {len(failures)} original execution failures; delta {stream_bytes} bytes versus {report["conventional_capture_bytes"]} separate raw bytes')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    return 1 if failures else 0


if __name__ == '__main__':
    raise SystemExit(main())
