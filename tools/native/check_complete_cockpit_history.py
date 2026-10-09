"""Follow complete independent M3 RAM streams through actual drawing owners.

The two source dispatches without a body do not advance native drawing history.
Scene rows must match byte for byte. The cockpit history starts on identical
pages and uses original bitmap, radar and glyph writes in separate expected
buffers. A first unexplained byte fails the whole-flight gate; the rest of both
streams are still checked against their sealed identities and complete traces.
Reference CPU execution is external and never supplies native gameplay state.
"""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile

from assess_mission_radar_cadence import apply_paints, painted_pages, changed_paints, phase_controls
from check_gameplay_checkpoint import ROOT, integer, span
from check_mission_message_pages import predicted_pages
from check_original_frame_delta import BOUNDARIES, verify_owner_return
from check_qualification_message_cadence import digest, pages, verify_trace, instrument_panel_refresh
from check_recorded_original_mission_trace import verified_update_mapping
from compare_flight_traces import read_trace, GAME_FIELDS
from frame_delta import bodies, snapshots


def file_hash(path, compressed=False):
    result = hashlib.sha256()
    with (gzip.open if compressed else open)(path, 'rb') as file:
        while chunk := file.read(1024 * 1024):
            result.update(chunk)
    return result.hexdigest()


def original_groups(directory, report):
    metadata = gzip.decompress((directory / 'registers.jsonl.gz').read_bytes())
    assert digest(metadata) == report['metadata_sha256']
    lines = iter(metadata.splitlines())
    identities = iter(report['identities'])
    pending, group, count, returns = {}, {}, 0, 0
    for snapshot in snapshots(directory / 'frames.delta.gz', BOUNDARIES):
        registers, identity = json.loads(next(lines)), next(identities)
        i, pc = snapshot['iteration'], snapshot['boundary']
        assert (registers['iteration'], registers['frame'], registers['pc']) == (i, snapshot['frame'], pc)
        assert (identity['iteration'], identity['frame'], identity['pc'], identity['ram_sha256']) == (
            i, snapshot['frame'], pc, snapshot['ram_sha256']), 'Original sealed snapshot identity differs'
        assert len(registers['registers']) == 16
        returns += verify_owner_return(snapshot, registers, pending)
        count += 1
        snapshot['registers'] = registers
        if pc == 0xC0EFD4 and group:
            yield group
            group = {}
        assert pc not in group, 'Repeated original drawing boundary'
        group[pc] = snapshot
    assert not pending and next(lines, None) is None and next(identities, None) is None
    assert count == report['snapshots']
    assert returns == sum(report['boundary_counts'].get(f'{pc:06X}', 0) for pc in (0xC0F182, 0xC0F18E, 0xC0F286, 0xC0F2DC))
    if group:
        yield group


def verified_native(directory, report):
    identities = iter(report['identities'])
    count = 0
    for body in bodies(directory / 'frames.delta.gz'):
        identity = next(identities)
        assert identity['original_body_matching'] and body['entry']['iteration'] == identity['iteration']
        for name in ('entry', 'before', 'after'):
            assert body[name]['ram_sha256'] == identity[name + '_sha256'], 'Native sealed snapshot identity differs'
        assert (body['before']['frame'], body['after']['frame'], body['before']['saved_tick']) == (
            identity['before_tick'], identity['after_tick'], identity['saved_tick'])
        count += 1
        yield body
    assert next(identities, None) is None and count == report['bodies']


def history_difference(live, expected):
    """Locate any unexplained bit across all eight complete planes."""
    differences = []
    for plane in range(8):
        actual = bytes(a ^ b for a, b in zip(live['source'][plane], live['native'][plane]))
        predicted = bytes(a ^ b for a, b in zip(expected['source'][plane], expected['native'][plane]))
        changed = [k for k, (a, b) in enumerate(zip(actual, predicted)) if a != b]
        if changed:
            k = changed[0]
            differences.append(dict(plane=plane, bytes=len(changed), first_byte=k, x=k % 40 * 8,
                y=k // 40, actual_xor=actual[k], predicted_xor=predicted[k]))
    return differences


def align_history(expected, previous_draw, entries, first):
    live = {name: pages(data) for name, data in entries.items()}
    for name, data in entries.items():
        draw = integer(data, 0xC4566C, 2)
        if first:
            expected[name] = [bytearray(page) for page in live[name]]
        elif draw != previous_draw[name]:
            expected[name] = expected[name][4:] + expected[name][:4]
        previous_draw[name] = draw
    if first:
        assert live['source'] == live['native'], 'History needs common complete initial pages'
    # This is a separate, strict scene check, not a pixel mask: every scene
    # byte must match. Copying that shared value into both expected buffers
    # preserves its effect on any subsequent owner writes at the boundary.
    for plane in range(8):
        assert live['source'][plane][:5120] == live['native'][plane][:5120], 'Scene rows differ'
        for name in ('source', 'native'):
            expected[name][plane][:5120] = live[name][plane][:5120]
    return live


def apply_owners(expected, role, panel, radar, message, omitted=None):
    refresh = instrument_panel_refresh(panel, expected[role], apply=not (role == 'source' and omitted == 'panel'))
    if not (role == 'source' and omitted == 'radar'):
        apply_paints(expected[role], radar['paints'])
    if not (role == 'source' and omitted == 'message'):
        expected[role] = predicted_pages(message['before'], message['after'], message['state']['text_drawn'], base=expected[role])
    return refresh


def run_oracle(executable, arguments, env):
    result = subprocess.run([str(executable), *map(str, arguments)], cwd=ROOT, env=env,
                            capture_output=True, text=True, timeout=20)
    assert result.returncode == 0, result.stdout + result.stderr
    return result.stdout


def owner_states(work, executables, env, before, after, kind, source_registers=None):
    fixture, output = work / 'owner.dat', work / 'output.dat'
    fixture.write_bytes(before)
    state = json.loads(run_oracle(executables[kind], (fixture, output), env))
    actual = output.read_bytes()
    assert state['complete_non_stack_ram_matching']
    assert pages(actual) == pages(after), f'Complete live {kind} owner pages differ'
    if kind == 'panel':
        expected = [bytearray(page) for page in pages(before)]
        instrument_panel_refresh(before, expected)
        assert expected == pages(after), 'Complete live bitmap copy differs'
        assert span(actual, 0xC45836, 1) == span(after, 0xC45836, 1)
        return state
    if kind == 'radar':
        for slot in range(16):
            address = 0xC46184 + 512 * slot
            assert span(actual, address, 164) == span(after, address, 164)
        for address, size in ((0xC4E2BC, 0x4B0), (0xC4586D, 3)):
            assert span(actual, address, size) == span(after, address, size)
        assert state['phase_after'] == integer(after, 0xC45883, 1)
        assert painted_pages(before, state['paints']) == pages(after), 'Ordered radar paints differ'
        flipped = bytearray(before)
        flipped[0xC45883 - 0xC00000 + 0x80000] ^= 1
        fixture.write_bytes(flipped)
        alternate = json.loads(run_oracle(executables[kind], (fixture, output), env))
        assert alternate['complete_non_stack_ram_matching']
        return state, alternate
    for address, size in ((0xC4580A, 26), (0xC45860, 3), (0xC4583C, 1),
                          (0xC45887, 1), (0xC459C4, 2), (0xC45ADE, 8)):
        assert span(actual, address, size) == span(after, address, size)
    assert predicted_pages(before, after, state['text_drawn']) == pages(after), 'Message glyph writes differ'
    if source_registers:
        incoming, returned = source_registers
        expected = incoming['registers'][4] & 255 if state['return_preserved'] else state['return_value']
        assert returned['registers'][4] & 255 == expected, 'Actual original text return differs'
    return state


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('source-delta', 'native-delta', 'reference', 'source-evidence', 'source-updates', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    code_paths = ('check_complete_cockpit_history.py', 'frame_delta.py', 'check_original_frame_delta.py',
        'check_qualification_message_cadence.py', 'check_mission_message_pages.py',
        'assess_mission_radar_cadence.py', 'check_recorded_original_mission_trace.py', 'compare_flight_traces.py',
        'native_frame_body_oracle.c', 'native_radar_cadence_oracle.c', 'native_message_cadence_oracle.c',
        'native_panel_frame_oracle.c', 'native_hud_oracle.c', 'native_records_oracle.c')
    code_hashes = {name: digest((Path(__file__).parent / name).read_bytes()) for name in code_paths}
    inputs = {name: json.loads((getattr(args, name) / 'report.json').read_text())
              for name in ('source_delta', 'native_delta', 'reference', 'source_evidence')}
    source, native, reference, original = (inputs[name] for name in ('source_delta', 'native_delta', 'reference', 'source_evidence'))
    assert source['complete_trace_preserved'] and source['final_ram_preserved'] and source['runtime_counters_preserved']
    assert native['original_bodies_matching'] and native['final_ram_preserved'] and native['earned_save_preserved']
    assert native['runner_sha256'] == reference['runner_sha256']
    assert native['native_final_ram_sha256'] == reference['native_final_ram_sha256']
    assert reference['whole_successful_flight']['strict_gameplay_matching']
    assert source['source_trace_sha256'] == reference['source_trace_sha256']
    assert source['boundary_counts'].get('C30764', 0) == source['boundary_counts']['C31226'] > 0
    assert native['native_trace_sha256'] == reference['native_trace_sha256']
    assert source['source_input_sha256'] == original['generated_input_sha256'] == digest((args.source_evidence / 'input.fa18in').read_bytes())
    assert file_hash(args.source_evidence / 'driver.jsonl.gz', True) == original['driver_trace_sha256'] == reference['baseline_source_trace_sha256']
    mapping, updates = verified_update_mapping(args.source_updates, args.source_evidence / 'driver.jsonl.gz', original)
    assert updates == reference['source_update_evidence']
    first, last = source['first'], source['last']
    assert first == native['first'] and last == native['last']
    assert set(mapping[i] for i in range(first, last + 1)) == set(range(native['native_first'], native['native_last'] + 1))
    sealed = {str(getattr(args, name) / 'report.json'): digest((getattr(args, name) / 'report.json').read_bytes()) for name in inputs}
    for path, expected in reference['input_hashes'].items():
        assert digest((ROOT / path).read_bytes()) == expected, path
    for directory, report in ((args.source_delta, source), (args.native_delta, native)):
        assert file_hash(directory / 'frames.delta.gz', True) == report['stream_sha256']
    headers, traces = {}, {}
    for name in ('source', 'native'):
        path = args.reference / f'{name}.jsonl.gz'
        assert file_hash(path, True) == reference[name + '_trace_sha256']
        headers[name], traces[name] = read_trace(path)
    assert headers['source'] == headers['native']
    executables = {name: ROOT / f'build/recomp/native_{name}_oracle.exe'
                   for name in ('frame_body', 'radar_cadence', 'message_cadence', 'panel_frame')}
    for name, executable in executables.items():
        with (args.out / f'{name}.build.log').open('w') as log:
            subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(executable.relative_to(ROOT)),
                '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True)
    executables = dict(body=executables['frame_body'], radar=executables['radar_cadence'],
        message=executables['message_cadence'], panel=executables['panel_frame'])
    executable_hashes = {str(path): digest(path.read_bytes()) for path in executables.values()}
    env = {k: v for k, v in os.environ.items() if not k.startswith(('FA18_FRAME_', 'FA18_TRACE_'))}
    expected, previous, history, radar_rows = {}, {}, [], []
    mutations = {name: ({}, {}) for name in ('panel', 'radar', 'message')}
    rejected, paint_rejected, glyph_rejected = {}, {}, {}
    failed, no_body, owner_counts = None, [], dict(source=0, native=0)
    current, native_iterator, count, executed = None, verified_native(args.native_delta, native), 0, 0
    previous_group = None
    with tempfile.TemporaryDirectory(prefix='complete-cockpit-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        for group in original_groups(args.source_delta, source):
            entry = group[0xC0EFD4]
            i, j = entry['iteration'], mapping[entry['iteration']]
            assert i == first + count
            if current is None or current['entry']['iteration'] != j:
                current = next(native_iterator)
                assert current['entry']['iteration'] == j
                verify_trace(current['entry']['data'], headers['native'], traces['native'][j])
            assert entry['frame'] == traces['source'][i]['frame'] and current['entry']['frame'] == traces['native'][j]['frame']
            verify_trace(entry['data'], headers['source'], traces['source'][i])
            assert traces['source'][i]['records'] == traces['native'][j]['records'], 'Complete gameplay cores differ'
            assert all(traces['source'][i]['fields'][field] == traces['native'][j]['fields'][field]
                       for field in GAME_FIELDS if field in traces['source'][i]['fields']), 'Gameplay/camera fields differ'
            entries = dict(source=entry['data'], native=current['entry']['data'])
            # Continue checking complete trace and scene evidence after failure,
            # without restarting a model at a conveniently matching later page.
            live = {name: pages(data) for name, data in entries.items()}
            assert all(live['source'][p][:5120] == live['native'][p][:5120] for p in range(8)), (i, 'Complete scene band differs')
            count += 1
            if 0xC0EFEA not in group:
                assert set(group) == {0xC0EFD4} and mapping[i] == mapping[i + 1]
                no_body.append(i)
            if failed is not None:
                continue
            align_history(expected, previous, entries, not history)
            difference = history_difference(live, expected)
            if difference:
                failed = dict(iteration=i, native_iteration=j, differences=difference)
                for name, data in entries.items():
                    (args.out / f'failed-{i}.{name}.dat.gz').write_bytes(gzip.compress(data, mtime=0))
                if previous_group:
                    context = {}
                    for pc, snapshot in previous_group.items():
                        (args.out / f'failed-previous-source-{pc:06X}.dat.gz').write_bytes(gzip.compress(snapshot['data'], mtime=0))
                        context[f'{pc:06X}'] = {key: value for key, value in snapshot.items() if key != 'data'}
                    (args.out / 'failed-previous-source.json').write_text(json.dumps(context, indent=2) + '\n')
                    for path in work.glob('*.dat'):
                        (args.out / ('failed-previous-native-' + path.name + '.gz')).write_bytes(gzip.compress(path.read_bytes(), mtime=0))
                print(f'Whole-flight history FAILED at {i}: {difference}', flush=True)
                continue
            for name, (model, draw) in mutations.items():
                align_history(model, draw, entries, not history)
                if name not in rejected and history_difference(live, model):
                    rejected[name] = i
            history.append(dict(iteration=i, native_iteration=j, compared_bytes=64000,
                difference_bytes=[sum(a != b for a, b in zip(x, y)) for x, y in zip(live['source'], live['native'])]))
            if 0xC0EFEA not in group:
                continue
            previous_group = group
            before, after = work / 'before.dat', work / 'after.dat'
            before.write_bytes(current['before']['data']); after.write_bytes(current['after']['data'])
            has_radar = 0xC31226 in group
            has_message = 0xC322EE in group
            has_panel = 0xC30764 in group
            assert has_radar == has_message == has_panel, 'Unassessed alternate HUD owner path'
            command = [before, after, current['before']['frame'], current['after']['frame'], current['before']['saved_tick']]
            if current['after']['boundary'] == 3:
                command += [work / 'owner-exit.dat', 'owner-exit']
            dump_env = dict(env, FA18_FRAME_RADAR_DUMP=str(work / 'radar'))
            # The original owner gate is driven by POST_INPUT_AUX. Require the
            # same actual gate here, rather than guessing whether a capture is
            # absent because of a source/native dispatch timing difference.
            assert bool(integer(current['before']['data'], 0xC45795, 1)) == has_radar
            if has_message:
                dump_env['FA18_FRAME_MESSAGE_DUMP'] = str(work / 'message')
                dump_env['FA18_FRAME_PANEL_DUMP'] = str(work / 'panel')
            for kind in ('radar', 'message', 'panel'):
                for suffix in ('before', 'after'):
                    (work / f'{kind}.{suffix}.dat').unlink(missing_ok=True)
            stdout = run_oracle(executables['body'], command, dump_env)
            assert '0 gameplay differences, 0 display bytes' in stdout
            executed += 1
            if not has_radar:
                assert not (work / 'radar.before.dat').exists() and not (work / 'radar.after.dat').exists()
                continue
            row = dict(iteration=i, native_iteration=j)
            refreshed = {}
            for role in ('source', 'native'):
                if role == 'source':
                    panel_in, panel_out = group[0xC30764]['data'], group[0xC0F182]['data']
                    radar_in, radar_out = group[0xC31226]['data'], group[0xC0F18E]['data']
                    message_in = group[0xC322EE]['data']
                    return_pc, = [pc for pc in (0xC0F286, 0xC0F2DC) if pc in group]
                    message_out = group[return_pc]['data']
                    registers = group[0xC322EE]['registers'], group[return_pc]['registers']
                else:
                    panel_in, panel_out = [(work / f'panel.{suffix}.dat').read_bytes() for suffix in ('before', 'after')]
                    radar_in, radar_out = [(work / f'radar.{suffix}.dat').read_bytes() for suffix in ('before', 'after')]
                    message_in, message_out = [(work / f'message.{suffix}.dat').read_bytes() for suffix in ('before', 'after')]
                    registers = None
                owner_states(work, executables, env, panel_in, panel_out, 'panel')
                radar, flipped = owner_states(work, executables, env, radar_in, radar_out, 'radar')
                message = owner_states(work, executables, env, message_in, message_out, 'message', registers)
                owner_counts[role] += 1
                row[role], row[role + '_flipped_phase'] = radar, flipped
                for kind in ('lost_erase', 'wrong_marker_colour', 'wrong_head_slope'):
                    changed = changed_paints(radar['paints'], kind)
                    if changed is not None and painted_pages(radar_in, changed) != pages(radar_out):
                        paint_rejected[kind] = True
                for kind in ('wrong_plane', 'wrong_glyph', 'lost_cell_clear'):
                    if predicted_pages(message_in, message_out, message['text_drawn'], kind) != pages(message_out):
                        glyph_rejected[kind] = True
                message_state = dict(before=message_in, after=message_out, state=message)
                refreshed[role] = apply_owners(expected, role, panel_in, radar, message_state)
                for omitted, (model, _) in mutations.items():
                    apply_owners(model, role, panel_in, radar, message_state, omitted)
            assert refreshed['source'] == refreshed['native'], 'Instrument images or redraw requests differ'
            radar_rows.append(row)
            if count % 100 == 0:
                print(f'{count}/{last-first+1} independent entries / {executed} bodies / {len(radar_rows)} paired HUD owners predicted', flush=True)
        assert next(native_iterator, None) is None, 'Unconsumed native bodies'
    assert count == last - first + 1
    phase_rejected, inactive_phase = phase_controls(radar_rows)
    assert {path: digest(Path(path).read_bytes()) for path in sealed} == sealed, 'Evidence reports changed during verification'
    assert {path: digest(Path(path).read_bytes()) for path in executable_hashes} == executable_hashes, 'Reference executable changed'
    assert {name: digest((Path(__file__).parent / name).read_bytes()) for name in code_paths} == code_hashes, 'Verifier changed during execution'
    controls = len(rejected) == 3 and len(paint_rejected) == 3 and len(glyph_rejected) == 3
    matching = failed is None and controls and len(history) == count
    report = dict(first=first, last=last, observations=count, complete_trace_and_core_observations=count,
        strict_scene_bytes=count * 8 * 5120, history_observations=len(history), history_bytes=64000 * len(history),
        complete_plane_history_matching=matching, first_unexplained=failed, no_body_dispatches=no_body,
        replayed_original_bodies_matching=executed, sealed_native_body_identities_matching=native['bodies'],
        sealed_source_snapshots_matching=source['snapshots'], live_owner_predictions=owner_counts, complete_plane_history=history,
        phase_mutation_rejections=phase_rejected, unobservable_phase_mutations=inactive_phase,
        paint_mutation_rejections=paint_rejected, glyph_mutation_rejections=glyph_rejected,
        source_omission_first_rejections=rejected, evidence_report_sha256=sealed,
        verifier_source_sha256=code_hashes,
        oracle_sha256=executable_hashes, runner_sha256=reference['runner_sha256'],
        source_trace_sha256=reference['source_trace_sha256'], native_trace_sha256=reference['native_trace_sha256'],
        scope='Complete independent streams and all cores/scene rows are checked. Known cockpit owner writes predict every eight-plane XOR byte only up to the first unexplained observation. Any unresolved history or missing negative control fails the whole-flight gate. No alignment offsets, pixel masks, game changes or reference RAM in gameplay.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{count} complete stream entries verified; {len(history)} histories predicted; full drawing matching={matching}', flush=True)
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)
    raise SystemExit(0 if matching else 1)


if __name__ == '__main__':
    main()
