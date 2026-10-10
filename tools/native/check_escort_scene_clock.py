"""Check actual escort initialization and its original microsecond dependency.

Native starts independently and earns the full preceding pilot progress.
Only the external instruction oracle receives controlled clock probes.
"""
import argparse
import gzip
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer, span
from check_recorded_original_mission_trace import digest, source_event_plan, verified_update_mapping
from compare_flight_traces import number, read_trace


def fields(data):
    return dict(mode=integer(data, 0xC458A6, 1), stage=f'{integer(data, 0xC1820C, 4):06X}',
        sample_seconds=integer(data, 0xC45AF2, 4), sample_microseconds=integer(data, 0xC45AF6, 4),
        bits=integer(data, 0xC45AF8, 2), sequence=integer(data, 0xC4582B, 1),
        shift_x=integer(data, 0xC45B18, 2), shift_z=integer(data, 0xC45B1A, 2),
        variant=integer(data, 0xC45B1C, 2), record_stream=f'{integer(data, 0xC4573A, 4):06X}')


def core_differences(expected, actual):
    return {str(slot): [i for i, (x, y) in enumerate(zip(
        span(expected, 0xC46184 + slot * 512, 164), span(actual, 0xC46184 + slot * 512, 164))) if x != y]
        for slot in range(16)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('runner', 'reference', 'source-evidence', 'source-updates', 'source-body', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    reference = json.loads((args.reference / 'report.json').read_text())
    original = json.loads((args.source_evidence / 'report.json').read_text())
    body = json.loads((args.source_body / 'report.json').read_text())
    assert original['unmodified_replay_exact'] and original['mission_mode'] == 4
    mapping, update = verified_update_mapping(args.source_updates, args.source_evidence / 'driver.jsonl.gz', original)
    assert reference['source_update_evidence'] == update
    _, source_rows = read_trace(args.source_evidence / 'driver.jsonl.gz')
    native_trace = gzip.decompress((args.reference / 'native.jsonl.gz').read_bytes())
    assert digest(native_trace) == reference['native_trace_sha256']
    _, native_rows = read_trace(args.reference / 'native.jsonl.gz')
    def transition(rows):
        return next(i for i, row in rows.items() if number(row, 'mode') == 4 and number(row, 'stage') == 0xC103E4)
    source_iteration, native_iteration = transition(source_rows) - 1, transition(native_rows) - 1
    assert body['iteration'] == source_iteration and len(body['captures']) == 1
    assert body['unchanged_observations'] >= source_iteration + 1
    assert body['source_trace_sha256'] == original['driver_trace_sha256']
    assert body['source_input_sha256'] == digest((args.source_evidence / 'input.fa18in').read_bytes())
    snapshots = {}
    for suffix, snapshot in body['captures'][0]['snapshots'].items():
        data = gzip.decompress((args.source_body / f'source-body.{source_iteration}.{suffix}.dat.gz').read_bytes())
        assert len(data) == 0x100000 and digest(data) == snapshot['ram_sha256']
        snapshots[suffix] = data
    assert body['captures'][0]['snapshots']['owner']['registers']['pc'] == 0xC28722
    assert body['captures'][0]['snapshots']['owner']['registers']['stack_return'] == 0xC0FF82
    plan = source_event_plan(args.source_evidence / 'driver.jsonl.gz', mapping, 4)
    assert plan == reference['event_plan']
    assert digest((args.reference / 'input.anchors').read_bytes()) == reference['anchor_input_sha256']
    for path, expected in reference['native_input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected
    runner_hash = digest(args.runner.read_bytes())
    oracle = ROOT / 'build/recomp/native_mode_entry_oracle.exe'
    with (args.out / 'oracle-build.log').open('w') as log:
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
            '--main', 'tools/native/native_mode_entry_oracle.c'], cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True)
    env = {k: v for k, v in os.environ.items() if not k.startswith(('FA18_LOOP_', 'FA18_ORIGINAL_', 'FA18_MODE_'))}
    with tempfile.TemporaryDirectory(prefix='escort-scene-clock-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        pilot = work / 'pilot'
        def run(arguments, log):
            result = subprocess.run([str(args.runner.resolve()), '--headless', *arguments], cwd=ROOT,
                env=env, capture_output=True, text=True, timeout=120)
            (args.out / log).write_text(result.stdout + result.stderr)
            assert result.returncode == 0, log
            return json.loads(result.stdout)
        enlisted = run(['--frames', '9000', '--replay', str(ROOT / 'tools/native/fixtures/region-pilot-enlist.e9k'),
                       '--save-dir', str(pilot)], 'enlist.log')
        assert enlisted == reference['enlist_run'] and (pilot / 'config').read_bytes().hex() == reference['enlisted_pilot']
        intro = work / 'intro.e9k'
        intro.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        prefix = work / 'native'
        stats = run(['--frames', '100000', '--replay', str(intro), '--save-dir', str(pilot),
            '--input', str((args.source_updates / 'update-consumed.fa18in').resolve()),
            '--input-anchors', str((args.reference / 'input.anchors').resolve()),
            '--input-anchors-out', str((args.out / 'anchors.json').resolve()),
            '--frame-capture', str(native_iteration), str(prefix), '--data-out', str(work / 'final.dat'),
            '--memory-report', str((args.out / 'memory.json').resolve())], 'native.log')
        assert stats['frame_capture_complete']
        capture_fields = {'frame_capture_complete', 'frame_before_tick', 'frame_after_tick', 'frame_saved_tick'}
        assert all(stats[k] == v for k, v in reference['native_run'].items() if k not in capture_fields)
        assert digest((work / 'final.dat').read_bytes()) == reference['native_final_ram_sha256']
        assert (pilot / 'config').read_bytes().hex() == reference['final_saved_pilot']
        assert json.loads((args.out / 'anchors.json').read_text()) == reference['event_report']
        memory = json.loads((args.out / 'memory.json').read_text())
        assert memory['project_gameplay_heap_violations'] == memory['sdl_failures'] == 0
        native = {}
        for suffix in ('entry', 'before', 'after'):
            data = Path(str(prefix) + '.' + suffix + '.dat').read_bytes()
            assert len(data) == 0x100000
            retained = args.out / f'native.{suffix}.dat.gz'
            retained.write_bytes(gzip.compress(data, mtime=0))
            assert gzip.decompress(retained.read_bytes()) == data
            native[suffix] = data
        assert fields(native['entry'])['stage'] == 'C0FECE' and fields(native['before'])['stage'] == 'C103E4'
        original_fraction = fields(snapshots['owner'])['sample_microseconds']
        native_fraction = fields(native['entry'])['sample_microseconds']
        cases = [('baseline', native_fraction), ('original_clock', original_fraction)]
        cases += [(f'wrong_clock_bit_{bit}', original_fraction ^ (1 << bit)) for bit in (1, 2, 4)]
        outputs = {}
        for name, fraction in cases:
            probe = bytearray(native['entry'])
            offset = 0x80000 + 0x45AF6
            probe[offset:offset + 4] = fraction.to_bytes(4, 'big')
            before = work / 'probe.dat'
            after = Path(str(prefix) + '.before.dat')
            produced = work / 'original.dat'
            before.write_bytes(probe)
            result = subprocess.run([str(oracle), str(before), str(after), str(stats['frame_before_tick'])],
                cwd=ROOT, env=dict(env, FA18_MODE_SOURCE_OUTPUT=str(produced)), capture_output=True, text=True, timeout=30)
            (args.out / f'{name}.log').write_text(result.stdout + result.stderr)
            assert (result.returncode == 0) == (name == 'baseline')
            if name == 'baseline':
                assert '0 compared RAM differences' in result.stdout
            data = produced.read_bytes()
            assert len(data) == 0x100000
            changes = core_differences(snapshots['owner-after'], data)
            complete = sum(not change for change in changes.values())
            if name == 'original_clock':
                assert complete == 16
                (args.out / 'original-clock-probe.dat.gz').write_bytes(gzip.compress(data, mtime=0))
            else:
                assert complete < 16
            outputs[name] = dict(microseconds=fraction, returncode=result.returncode,
                ram_sha256=digest(data), fields=fields(data), original_complete_cores_matching=complete,
                original_core_differences=changes, strict_comparison_stdout=result.stdout.strip())
    assert digest(args.runner.read_bytes()) == runner_hash
    report = dict(source_iteration=source_iteration, native_iteration=native_iteration,
        source_trace_sha256=original['driver_trace_sha256'], source_body_report_sha256=digest((args.source_body / 'report.json').read_bytes()),
        reference_report_sha256=digest((args.reference / 'report.json').read_bytes()), runner_sha256=runner_hash,
        oracle_sha256=digest(oracle.read_bytes()), oracle_source_sha256=digest((ROOT / 'tools/native/native_mode_entry_oracle.c').read_bytes()),
        native_clock_source_sha256=digest((ROOT / 'port/game/native/clock.c').read_bytes()),
        native_run=stats, native_ram_sha256={k: digest(v) for k, v in native.items()},
        source_fields={k: fields(v) for k, v in snapshots.items()}, native_fields={k: fields(v) for k, v in native.items()},
        cases=outputs, memory=memory, full_replay_counters_preserved=True, final_ram_preserved=True,
        final_ram_sha256=reference['native_final_ram_sha256'], earned_save_preserved=True,
        native_timer_low_five_bits=[((tick * 20000) % 1000000) & 31 for tick in range(50)],
        scope='Actual native C0FECE setup matches original instructions under its own inputs. A reference-only clock probe reproduces all 16 actual original cores. No clock or reference RAM feeds native gameplay; complete flight parity and timer-resolution policy remain open.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    print('Native setup matches original instructions; original-clock reference probe matches all 16 original cores; three wrong-clock probes reject')


if __name__ == '__main__':
    main()
