"""Assess observed selected-target radar blink phases without masking pages.

Actual original owner inputs/returns are checked separately from native body
integration. Phase reversal is an external oracle probe, never a runtime fix.
"""
import argparse
import copy
import gzip
import json
import os
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT, integer, span
from check_qualification_message_cadence import digest, pages


def points(row):
    return sorted([{k:v for k,v in point.items() if k!='phase'} for point in row['points']],
                  key=lambda point: point['record_offset'])


def verify(rows):
    phase_offset = None
    for i, row in enumerate(rows):
        source, native = row['source'], row['native']
        offset = (native['phase_before'] - source['phase_before']) % 256
        if phase_offset is None:
            phase_offset = offset
        assert offset == phase_offset
        for name, other in (('source', 'native'), ('native', 'source')):
            state, flipped = row[name], row[name + '_flipped_phase']
            assert state['complete_non_stack_ram_matching']
            assert state['prefix_calls'] == flipped['prefix_calls'] == row[other]['prefix_calls']
            assert state['prefix_calls'] in (0,1,2)
            assert state['phase_after'] == (state['phase_before'] + state['prefix_calls']) % 256
            assert flipped['phase_before'] == state['phase_before'] ^ 1
            assert flipped['phase_after'] == (flipped['phase_before'] + flipped['prefix_calls']) % 256
            assert state['selected_record'] == row[other]['selected_record']
            if i:
                assert state['phase_before'] == rows[i - 1][name]['phase_after']
            selected = state['selected_record']
            regular = [p for p in points(state) if p['record_offset'] != selected]
            assert regular == [p for p in points(flipped) if p['record_offset'] != selected]
            marker = [p for p in points(state) if p['record_offset'] == selected]
            alternate = [p for p in points(flipped) if p['record_offset'] == selected]
            eligible = bool(marker or alternate)
            for case in (state,flipped):
                assert all(p['phase']&1 for p in case['points'] if p['record_offset']==selected)
            if state['prefix_calls']==1:
                assert bool(marker) == (eligible and bool(state['phase_after'] & 1))
                assert bool(alternate) == (eligible and bool(flipped['phase_after'] & 1))
            elif state['prefix_calls']==2:
                assert bool(marker)==bool(alternate)
            else:
                assert not marker and not alternate
            # Equivalent blink phases must yield every same record/position/
            # pair/colour. The phase probe is not applied to playable memory.
            equivalent = row[other] if offset % 2 == 0 else row[other + '_flipped_phase']
            assert points(state) == points(equivalent)
    return phase_offset


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--window', type=Path, required=True)
    parser.add_argument('--original-bodies', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    window = json.loads((args.window / 'report.json').read_text())
    original = json.loads((args.original_bodies / 'report.json').read_text())
    assert window['source_trace_sha256'] == original['source_trace_sha256']
    captures = {row['iteration']: row['snapshots'] for row in original['captures']}
    assert sorted(captures) == list(range(window['first'], window['last'] + 1))
    assert original['last_observation'] >= window['last'] + 1
    body_oracle = ROOT / 'build/recomp/native_frame_body_oracle.exe'
    radar_oracle = ROOT / 'build/recomp/native_radar_cadence_oracle.exe'
    for executable, source in ((body_oracle, 'native_frame_body_oracle.c'),
                                (radar_oracle, 'native_radar_cadence_oracle.c')):
        with (args.out / (source + '.build.log')).open('w') as log:
            subprocess.run(['python', 'scripts/build_recomp.py', '--output',
                str(executable.relative_to(ROOT)), '--main', 'tools/native/' + source],
                cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True)
    env = {k: v for k, v in os.environ.items() if not k.startswith('FA18_FRAME_')}
    rows = []
    with tempfile.TemporaryDirectory(prefix='radar-cadence-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        for observed in window['rows']:
            i, j = observed['source_iteration'], observed['native_iteration']
            timing = observed['body_timing']
            before, after = work / 'native.before.dat', work / 'native.after.dat'
            for suffix, path in (('before', before), ('after', after)):
                path.write_bytes(gzip.decompress((args.window / f'native.{j}.{suffix}.dat.gz').read_bytes()))
            prefix = work / 'radar'
            log_path = args.out / f'native-body.{j}.log'
            with log_path.open('w') as log:
                result = subprocess.run([str(body_oracle), str(before), str(after),
                    str(timing['before_tick']), str(timing['after_tick']), str(timing['saved_tick'])],
                    cwd=ROOT, env=dict(env, FA18_FRAME_RADAR_DUMP=str(prefix)),
                    stdout=log, stderr=subprocess.STDOUT, timeout=20)
            assert result.returncode == 0, log_path
            assert '0 gameplay differences, 0 display bytes' in log_path.read_text()
            row = dict(iteration=i, native_iteration=j, source_owner_live_output_matching=True,
                       native_body_input_sha256=digest(before.read_bytes()),
                       native_body_output_sha256=digest(after.read_bytes()),
                       native_body_original_matching_with_existing_exclusions=True,
                       strict_page_differences=observed['page_differences'])
            for name in ('source', 'native'):
                if name == 'source':
                    boundary_data = []
                    for suffix in ('owner', 'owner-after'):
                        data = gzip.decompress((args.original_bodies / f'source-body.{i}.{suffix}.dat.gz').read_bytes())
                        assert digest(data) == captures[i][suffix]['ram_sha256']
                        boundary_data.append(data)
                    owner_input, observed_output = boundary_data
                else:
                    owner_input = (work / 'radar.before.dat').read_bytes()
                    observed_output = (work / 'radar.after.dat').read_bytes()
                    # C45883 is unchanged by the preceding mode-three scene.
                    assert integer(owner_input, 0xC45883, 1) == integer(before.read_bytes(), 0xC45883, 1)
                assert len(owner_input) == len(observed_output) == 0x100000
                row[name + '_owner_input_sha256'] = digest(owner_input)
                row[name + '_owner_output_sha256'] = digest(observed_output)
                for flip in (False, True):
                    data = bytearray(owner_input)
                    if flip:
                        data[0x80000 + 0x45883] ^= 1
                    fixture, output = work / 'owner.dat', work / 'output.dat'
                    fixture.write_bytes(data)
                    result = subprocess.run([str(radar_oracle), str(fixture), str(output)],
                        cwd=ROOT, env=env, capture_output=True, text=True, timeout=20)
                    assert result.returncode == 0, result.stderr
                    state = json.loads(result.stdout)
                    row[name + ('_flipped_phase' if flip else '')] = state
                    if not flip:
                        actual = output.read_bytes()
                        # Actual original returns can include asynchronous OS
                        # and real ABI stack changes. Check all complete pages,
                        # sixteen cores and the entire radar-owned list/cache.
                        assert pages(actual) == pages(observed_output)
                        for slot in range(16):
                            address = 0xC46184 + 512 * slot
                            assert span(actual, address, 164) == span(observed_output, address, 164)
                        assert span(actual, 0xC4E2BC, 0x4B0) == span(observed_output, 0xC4E2BC, 0x4B0)
                        assert state['phase_after'] == integer(observed_output, 0xC45883, 1)
                        assert span(actual, 0xC4586D, 3) == span(observed_output, 0xC4586D, 3)
                if name == 'source':
                    body = gzip.decompress((args.original_bodies / f'source-body.{i}.before.dat.gz').read_bytes())
                    assert digest(body) == captures[i]['before']['ram_sha256']
                    row['source_body_phase_before'] = integer(body, 0xC45883, 1)
                    assert row['source_body_phase_before'] == row['source']['phase_before']
            rows.append(row)
    phase_offset = verify(rows)
    rejections = {}
    for kind in ('counter_increment', 'premature_marker', 'marker_coordinate', 'other_marker'):
        changed = copy.deepcopy(rows)
        state = changed[0]['source']
        marker = next(p for p in state['points'] if p['record_offset'] == state['selected_record'])
        if kind == 'counter_increment':
            state['phase_after'] += 1
        elif kind == 'premature_marker':
            changed[0]['source_flipped_phase']['points'].append(copy.deepcopy(marker))
        elif kind == 'marker_coordinate':
            marker['x'] += 1
        else:
            state['points'] = [p for p in state['points'] if p['record_offset'] == state['selected_record']]
        try:
            verify(changed)
        except AssertionError:
            rejections[kind] = True
        else:
            raise AssertionError(f'{kind} mutation accepted')
    report = dict(first=window['first'], last=window['last'], boundaries=len(rows),
        runner_sha256=window['runner_sha256'], original_probe_sha256=original['probe_sha256'],
        original_unchanged_observations=original['unchanged_observations'],
        source_trace_sha256=window['source_trace_sha256'], native_trace_sha256=window['native_trace_sha256'],
        radar_original_non_stack_comparisons=4 * len(rows), native_complete_body_comparisons=len(rows),
        live_original_radar_returns_compared=len(rows), initial_phase_offset=phase_offset,
        equivalent_blink_phase_points_matching=True, mutation_rejections=rejections, rows=rows,
        scope='Selected-target markers obey original alternating-call blink in this window. '
              'Different incoming phases are assessed under the accepted rendering-cadence policy. '
              'Every strict page difference remains reported. Message text and other drawing differences '
              'are not accepted by this check. Native body checks retain their existing explicit exclusions; '
              'actual original owner returns are checked for all complete pages, cores and radar list/cache.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(f'{len(rows)} original/native radar transitions, {4*len(rows)} complete owner comparisons, '
          f'{len(rows)} native body comparisons; phase offset={phase_offset}; four mutations rejected')
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
