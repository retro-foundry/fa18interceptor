"""Reproduce an accepted complete flight after a native implementation change.

Only controls enter the game. Reference RAM is hashed externally. Exact complete
body-stream identity extends existing original-body/drawing evidence to this
runner without repeating the original instruction sweep or altering its report.
"""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def sha(path, compressed=False):
    with (gzip.open if compressed else open)(path, 'rb') as file:
        return hashlib.file_digest(file, 'sha256').hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('runner', 'reference', 'replay-evidence', 'body-evidence',
                 'prefix', 'drawing-evidence', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    paths = {name: getattr(args, name.replace('-', '_')) / 'report.json'
             for name in ('reference', 'replay-evidence', 'body-evidence',
                          'prefix', 'drawing-evidence')}
    reports = {name: json.loads(path.read_text()) for name, path in paths.items()}
    reference, replay, body, prefix, drawing = (reports[name] for name in paths)
    assert reference['mission_mode'] == replay['mission_mode'] == 5
    assert reference['whole_successful_flight']['strict_gameplay_matching']
    assert replay['native_mission_success'] and replay['native_prefix_execution_exact']
    assert prefix['canonical_complete_trace_and_ram_exact']
    assert body['original_bodies_matching'] and not body['original_body_failures']
    assert drawing['complete_plane_history_matching'] and drawing['first_unexplained'] is None
    old_runner = reference['runner_sha256']
    assert all(record['runner_sha256'] == old_runner for record in reports.values())
    assert reference['native_trace_sha256'] == body['native_trace_sha256'] == drawing['native_trace_sha256']
    assert reference['native_run'] == replay['native_run']
    assert reference['input_segment'] == replay['input_segment'] == body['input_segment']
    assert reference['event_report'] == replay['event_report'] == body['event_report']
    assert sha(args.reference / 'native.jsonl.gz', True) == reference['native_trace_sha256']
    assert sha(args.body_evidence / 'frames.delta.gz', True) == body['stream_sha256']
    assert sha(args.replay_evidence / 'native.dat.gz', True) == reference['native_final_ram_sha256']
    inputs = {**reference['input_hashes'], **replay['native_input_hashes']}
    for name, expected in inputs.items():
        assert sha(ROOT / name) == expected, name
    controls = args.replay_evidence / 'input.segment.fa18in'
    anchors = args.replay_evidence / 'input.anchors'
    physical = args.prefix / 'physical.e9k'
    assert sha(controls) == reference['input_segment']['input_sha256']
    assert sha(anchors) == replay['anchor_input_sha256']
    assert sha(physical) == prefix['physical_input_sha256']
    env = {key: value for key, value in os.environ.items() if not key.startswith(
        ('FA18_FRAME_', 'FA18_TRACE_', 'FA18_LOOP_', 'FA18_MISSION_', 'FA18_ORIGINAL_'))}
    env['FA18_TRACE_MESSAGE_FIELDS'] = '1'
    runner_hash = sha(args.runner)
    report = dict(runner_sha256=runner_hash, baseline_runner_sha256=old_runner,
                  evidence_sha256={name: sha(path) for name, path in paths.items()},
                  input_sha256={str(controls): sha(controls), str(anchors): sha(anchors),
                                str(physical): sha(physical)}, complete_preservation=False)
    with tempfile.TemporaryDirectory(prefix='ram-qualified-mission-', dir=ROOT / 'build') as temporary:
        work = Path(temporary)
        pilot = work / 'pilot'

        def run(arguments, name):
            result = subprocess.run([str(args.runner.resolve()), '--headless', *arguments],
                                    cwd=ROOT, env=env, capture_output=True, text=True, timeout=300)
            (args.out / (name + '.log')).write_text(result.stdout + result.stderr)
            if result.returncode:
                for path in work.iterdir():
                    if path.is_file() and path.suffix in ('.jsonl', '.dat', '.delta'):
                        (args.out / (path.name + '.gz')).write_bytes(gzip.compress(path.read_bytes(), mtime=0))
            assert result.returncode == 0, name
            return json.loads(result.stdout)

        enlist = run(['--frames', '9000', '--replay', str(ROOT / 'tools/native/fixtures/region-pilot-enlist.e9k'),
                      '--save-dir', str(pilot)], 'enlist')
        initial = (pilot / 'config').read_bytes()
        assert enlist == reference['enlist_run'] and initial.hex() == replay['enlisted_pilot']
        assert len(initial) == 78 and initial[:4] == bytes(4)
        assert initial[18:28] == bytes(10) and initial[56:58] == bytes(2)
        body_pilot = work / 'body-pilot'
        body_pilot.mkdir()
        # Actual ordinary enlisted save; no constructed progress or captured RAM.
        (body_pilot / 'config').write_bytes(initial)
        trace, stream, final = (work / name for name in ('native.jsonl', 'frames.delta', 'native.dat'))
        common = ['--frames', str(reference['native_run']['frames']),
                       '--replay', str(physical.resolve()), '--input', str(controls.resolve()),
                       '--iterations', str(reference['input_segment']['replay_source_end'])]
        trace_summary = run([*common, '--save-dir', str(pilot),
                       '--input-anchors', str(anchors.resolve()),
                       '--input-anchors-out', str(work / 'anchors.json'), '--flight-trace', str(trace),
                       '--data-out', str(work / 'trace-final.dat')], 'flight-trace')
        # The runner deliberately separates flight tracing from frame-delta
        # capture budgets. Reproduce both complete runs and compare all outputs.
        summary = run([*common, '--save-dir', str(body_pilot),
                       '--input-anchors', str(anchors.resolve()),
                       '--input-anchors-out', str(work / 'body-anchors.json'),
                       '--frame-delta', f'{body["native_first"]}+{body["bodies"]}', str(stream),
                       '--data-out', str(final), '--memory-report', str(args.out.resolve() / 'memory.json')], 'flight-bodies')
        actual = dict(native_run=summary, native_trace_sha256=sha(trace),
                      native_final_ram_sha256=sha(final), stream_sha256=sha(stream),
                      stream_bytes=stream.stat().st_size, final_saved_pilot=(pilot / 'config').read_bytes().hex(),
                      event_report=json.loads((work / 'anchors.json').read_text()))
        report['actual'] = actual
        report['capture_bytes'] = sum(path.stat().st_size for path in work.rglob('*') if path.is_file())
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
        try:
            assert report['capture_bytes'] <= 512 * 1024 * 1024
            assert summary == trace_summary
            assert sha(work / 'trace-final.dat') == actual['native_final_ram_sha256']
            assert (body_pilot / 'config').read_bytes() == (pilot / 'config').read_bytes()
            assert json.loads((work / 'body-anchors.json').read_text()) == actual['event_report']
            for key in ('native_run', 'native_trace_sha256', 'native_final_ram_sha256',
                        'final_saved_pilot', 'event_report'):
                assert actual[key] == reference[key], key
            for key in ('stream_sha256', 'stream_bytes'):
                assert actual[key] == body[key], key
            memory = json.loads((args.out / 'memory.json').read_text())
            assert memory['project_gameplay_heap_violations'] == memory['sdl_failures'] == memory['sdl_gameplay_pool_requests'] == 0
            assert sha(args.runner) == runner_hash
        except AssertionError:
            for path in (trace, stream, final):
                (args.out / (path.name + '.gz')).write_bytes(gzip.compress(path.read_bytes(), mtime=0))
            raise
        report.update(complete_preservation=True, bodies=body['bodies'], snapshots=body['snapshots'],
                      memory=memory, scope='Complete native trace, body entry/begin/end RAM and timing, final RAM, earned pilot, anchors and counters reproduce the accepted Mission Five recording. Original instruction and drawing qualification remains bound to its prior report; exact native preservation extends that recorded scope to this executable. No recorded RAM enters gameplay; other routes and complete sound parity remain open.')
        (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    print(f'{body["bodies"]} complete bodies and all recorded flight outputs preserved; zero project gameplay heap violations')


if __name__ == '__main__':
    main()
