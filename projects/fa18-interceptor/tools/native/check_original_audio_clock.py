"""Predict actual original output boundaries from observed source countdowns.

Clock snapshots are reference inputs only. No phase search, inferred countdown,
native frequency change or waveform alignment is performed.
"""
import argparse
import copy
from collections import Counter
import hashlib
import json
import math
from pathlib import Path
import struct

from check_original_audio_capture import reference_file, reference_bytes
from check_original_audio_mixer import mixer_frames
from original_audio_samples import CLOCK, clock_record


def bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


def value(encoded):
    return struct.unpack('<f', struct.pack('<I', encoded))[0]


def f32(number):
    return value(bits(number))


def source_interval(row):
    # custom.c:compute_vsynctime and retrodep/sounddep/sound.c:update_sound.
    # This explicit non-interlaced/non-line-toggle profile must match every
    # actual observed interval; unsupported profiles are rejected.
    lines = f32(row['nominal_lines'] + row['long_field'])
    clock = f32(f32(lines * row['short_line_clocks']) * value(row['fake_refresh_bits']))
    interval = f32(f32(f32(clock * 512) * value(row['sync_multiplier_bits'])) / row['output_rate'])
    assert bits(interval) == row['scaled_original_bits'] == row['scaled_current_bits'], 'Source clock profile differs'
    return interval, clock


def verified_clock(frames, snapshots, bounds):
    first, last = bounds
    assert len(snapshots) == last - first + 2, 'Clock coverage differs'
    expected = [(first, 'before')] + [(call, 'after') for call in range(first, last + 1)]
    assert [(row['call'], row['boundary']) for row in snapshots] == expected, 'Clock snapshot order differs'
    names = ('service_cycle', 'last_cycle', 'scaled_original_bits', 'scaled_current_bits',
             'next_output_bits', 'sync_multiplier_bits', 'output_rate', 'nominal_lines',
             'short_line_clocks', 'long_field', 'fake_refresh_bits', 'configured_refresh_bits')
    for row in snapshots:
        assert clock_record(CLOCK.pack(*(row[name] for name in names))) == {name: row[name] for name in names}
        source_interval(row)
    initial = snapshots[0]
    interval, clock = source_interval(initial)
    countdown, logical = value(initial['next_output_bits']), initial['last_cycle']
    outputs, calls, previous_output = 0, 0, None
    gaps = Counter()
    for header, rows in frames:
        call, _, before, after, _ = header
        if not first <= call <= last:
            assert not rows
            continue
        assert call == first + calls
        if not calls:
            assert before * 512 <= initial['service_cycle'] < (before + 1) * 512
        for row in rows:
            kind, at = row['kind'], row['logical_cycle']
            if kind == 0:
                assert not calls and logical == at, 'Initial audio last-cycle differs'
            elif kind == 1:
                elapsed = at - logical
                assert 0 <= elapsed <= math.floor(countdown + 0.5), (call, 'Output boundary skipped', elapsed, countdown)
                countdown = f32(countdown - elapsed)
                logical = at
            else:
                assert at == logical, (call, 'Output/byte has no clock owner')
                if kind == 2:
                    assert -0.5 <= countdown < 0.5, (call, 'Premature original output', countdown)
                    countdown = f32(countdown + interval)
                    if previous_output is not None: gaps[at - previous_output] += 1
                    previous_output = at; outputs += 1
        endpoint = snapshots[calls + 1]
        assert after * 512 <= endpoint['service_cycle'] < (after + 1) * 512
        assert endpoint['last_cycle'] == logical, (call, 'Final audio last-cycle differs')
        assert endpoint['next_output_bits'] == bits(countdown), (call, 'Final output countdown differs')
        assert source_interval(endpoint) == (interval, clock), 'Output clock changed in the observed window'
        calls += 1
    assert calls == last - first + 1 and outputs
    return dict(observed_calls=calls, exact_predicted_output_boundaries=outputs,
                exact_observed_clock_snapshots=len(snapshots), source_clock_hz=clock,
                source_output_interval_raw=interval, output_raw_gaps=dict(gaps),
                final_countdown_bits=bits(countdown), final_audio_last_cycle=logical,
                actual_source_countdown_and_all_output_boundaries_matching=True)


def controls(observations, snapshots, bounds):
    rejected = {}
    for change in ('initial_countdown', 'interval', 'geometry', 'endpoint_countdown', 'output_time', 'lost_output'):
        changed = copy.deepcopy(snapshots)
        if change == 'initial_countdown': changed[0]['next_output_bits'] = bits(value(changed[0]['next_output_bits']) + 512)
        elif change == 'interval': changed[0]['scaled_current_bits'] += 1
        elif change == 'geometry': changed[0]['long_field'] ^= 1
        elif change == 'endpoint_countdown': changed[1]['next_output_bits'] ^= 1
        def damaged():
            done = False
            for header, rows in observations():
                if change in ('output_time', 'lost_output') and not done:
                    rows = copy.deepcopy(rows)
                    for index, row in enumerate(rows):
                        if row['kind'] == 2:
                            if change == 'output_time': row['logical_cycle'] += 1
                            else: del rows[index]
                            done = True; break
                yield header, rows
        try:
            verified_clock(damaged(), changed, bounds)
        except AssertionError:
            rejected[change] = True
        else:
            raise AssertionError('Accepted damaged original clock: ' + change)
    return rejected


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('capture', 'baseline-mixer', 'timeline-report', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    args = parser.parse_args()
    snapshot = json.loads((args.capture / 'snapshot.json').read_text())
    descriptor = snapshot['audio_samples']['mixer_timeline']
    timeline = json.loads(args.timeline_report.read_text())
    digest = hashlib.sha256(reference_bytes(args.capture / 'audio_mixer.bin')).hexdigest()
    assert digest == descriptor['sha256'] == timeline['mixer_stream_sha256']
    assert timeline['timeline']['complete_actual_accumulators_and_logical_intervals_matching']
    assert timeline['preserved_execution']['authority'] == snapshot['authority']
    assert reference_bytes(args.capture / 'audio_mixer.bin') == reference_bytes(args.baseline_mixer / 'audio_mixer.bin'), 'Clock observation changed the complete mixer stream'
    bounds = descriptor.get('observed_call_range', [1, descriptor['calls']])
    def observations():
        with reference_file(args.capture / 'audio_mixer.bin') as file:
            yield from mixer_frames(file, descriptor['calls'])
    result = verified_clock(observations(), descriptor['clock_snapshots'], bounds)
    assert result['exact_predicted_output_boundaries'] == timeline['timeline']['exact_output_averages']
    report = dict(clock=result, actual_record_mutations_rejected=controls(observations, descriptor['clock_snapshots'], bounds),
                  mixer_stream_sha256=digest, unchanged_complete_mixer_stream=True,
                  clock_free_baseline=str(args.baseline_mixer),
                  timeline_report_sha256=hashlib.sha256(args.timeline_report.read_bytes()).hexdigest(),
                  observer_manifest_sha256=snapshot['audio_samples']['observer_manifest_sha256'],
                  scope='Observed original clock inputs and exact initial countdown predict every output boundary and final countdown in the bounded complete mixer window. Existing complete execution/PCM preservation remains prerequisite. Native playback and its frequency are unchanged; no inferred phase or waveform alignment.')
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(result, sort_keys=True))


if __name__ == '__main__':
    main()
