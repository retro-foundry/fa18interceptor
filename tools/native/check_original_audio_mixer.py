"""Verify every original logical mixer interval against its actual averages.

This is a reference timeline, never a native clock or scheduling input.
Paired whole execution/PCM and existing sample telemetry stay mandatory.
"""
import argparse
import copy
from collections import Counter
import hashlib
import json
from pathlib import Path
import sys

from check_original_audio_samples import validate_samples, frames as sample_frames
from check_original_audio_capture import reference_file, reference_bytes
from check_original_audio_dma import exact
from build_original_audio_probe import ROOT

sys.path.insert(0, str(ROOT / 'scripts'))
from original_audio_samples import MIXER_MAGIC, MIXER, FRAME, FOOTER, CYCLE_UNIT, mixer_records, sample_records


def mixer_frames(file, calls):
    assert exact(file, len(MIXER_MAGIC)) == MIXER_MAGIC, 'Invalid mixer stream header'
    count, total, previous = 0, 0, None
    while True:
        tag = exact(file, 1)
        if tag == b'\0':
            assert FOOTER.unpack(exact(file, FOOTER.size)) == (calls, total)
            assert count == calls and not file.read(1), 'Incomplete mixer stream'
            return
        assert tag == b'\1'
        header = FRAME.unpack(exact(file, FRAME.size))
        call, records, before, after, pcm_end = header
        assert call == count + 1 and records <= 65536 and after >= before
        assert previous is None or previous == before, 'Mixer call cycle gap'
        payload = exact(file, records * MIXER.size)
        yield header, list(mixer_records(payload, before, after))
        count += 1; total += records; previous = after


def quotient(area, time):
    return (abs(area) // time) * (-1 if area < 0 else 1) if time else 0


def verify_timeline(frames):
    areas = times = None
    logical = None
    counts, durations, leads = Counter(), Counter(), {}
    sample_previous, sample_gaps, discarded = {}, {}, 0
    emitted = consumed = 0
    for call, rows, samples, pcm_frames in frames:
        samples = iter(samples)
        output_count = 0
        for row in rows:
            kind = row['kind']
            assert row['service_cycle'] >= row['logical_cycle'], 'Mixer logical event follows service time'
            if kind == 0:
                assert areas is None and call == 1 and not counts, 'Repeated/missing initial mixer state'
                areas, times = list(row['values']), list(row['times'])
                logical = row['logical_cycle']
            else:
                assert areas is not None, 'Mixer has no initial accumulators'
                assert row['logical_cycle'] >= logical, 'Mixer logical clock reversed'
                assert row['times'] == times, 'Actual accumulator budget differs'
                if kind == 1:
                    assert row['duration'] == (row['logical_cycle'] - logical) // CYCLE_UNIT, 'Mixer interval differs from logical boundary'
                    discarded += row['logical_cycle'] - logical - CYCLE_UNIT * row['duration']
                    for channel in range(4):
                        area = areas[channel] + row['values'][channel] * row['duration']
                        areas[channel] = ((area + 0x80000000) & 0xffffffff) - 0x80000000
                        times[channel] += row['duration']
                    durations[row['duration']] += 1
                elif kind == 2:
                    assert row['values'] == [quotient(area, time) for area, time in zip(areas, times)], 'Original output average differs'
                    areas, times = [0] * 4, [0] * 4
                    emitted += 1; output_count += 1
                else:
                    sample = next(samples, None)
                    channel = kind - 3
                    assert sample is not None and (row['service_cycle'], channel, row['values'][channel]) == (sample[0], sample[4], sample[3]), 'Mixer transition differs from actual consumed byte'
                    if channel in sample_previous:
                        sample_gaps.setdefault(channel, Counter())[row['logical_cycle'] - sample_previous[channel]] += 1
                    sample_previous[channel] = row['logical_cycle']
                    consumed += 1
                logical = row['logical_cycle']
            counts[kind] += 1
            lead = row['service_cycle'] - row['logical_cycle']
            summary = leads.setdefault(kind, dict(min=lead, max=lead, zero=0, records=0))
            summary['min'] = min(summary['min'], lead); summary['max'] = max(summary['max'], lead)
            summary['zero'] += lead == 0; summary['records'] += 1
        assert next(samples, None) is None, 'Unobserved consumed sample'
        assert output_count == pcm_frames, 'Mixer output coverage differs from recorded PCM'
    assert counts[0] == 1 and emitted and consumed
    return dict(records=sum(counts.values()), records_by_kind=dict(counts),
        exact_output_averages=emitted, exact_consumed_transitions=consumed,
        duration_counts=dict(durations), service_leads_by_kind=leads,
        logical_sample_gaps_by_channel={channel: dict(gaps) for channel, gaps in sample_gaps.items()},
        discarded_prehandler_subcycle_units=discarded,
        final_accumulators=areas, final_accumulator_times=times,
        complete_actual_accumulators_and_logical_intervals_matching=True)


def rejection_controls(observations):
    """Damage real records, preserving the complete unchanged remainder."""
    rejected = {}
    for change in ('initial_area', 'interval', 'logical_time', 'average', 'budget', 'sample', 'missing_sample'):
        changed = False
        def altered():
            nonlocal changed
            for call, rows, samples, pcm_frames in observations():
                rows = copy.deepcopy(rows)
                for index, row in enumerate(rows):
                    eligible = row['kind'] == (0 if change == 'initial_area' else
                        1 if change in ('interval', 'logical_time') else
                        2 if change in ('average', 'budget') else 3)
                    if changed or not eligible:
                        continue
                    if change == 'initial_area': row['values'][0] += 0x100000
                    elif change == 'interval': row['duration'] += 1
                    elif change == 'logical_time': row['logical_cycle'] -= CYCLE_UNIT
                    elif change == 'average': row['values'][0] += 1
                    elif change == 'budget': row['times'][0] += 1
                    elif change == 'sample': row['values'][0] += 1
                    else: del rows[index]
                    changed = True
                    break
                yield call, rows, samples, pcm_frames
        try:
            verify_timeline(altered())
        except AssertionError:
            assert changed, 'Control never changed an actual record'
            rejected[change] = True
        else:
            raise AssertionError(f'Mixer mutation accepted: {change}')
    return rejected


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('capture', 'baseline', 'prior-samples', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    parser.add_argument('--dma-reference', type=Path)
    parser.add_argument('--reference-context', choices=('demo', 'startup'), default='demo')
    args = parser.parse_args()
    preserved = validate_samples(args.capture, args.baseline, dma_reference=args.dma_reference,
        reference_context=args.reference_context)
    snapshot = json.loads((args.capture / 'snapshot.json').read_text())
    prior = json.loads((args.prior_samples / 'snapshot.json').read_text())
    descriptor = snapshot['audio_samples']['mixer_timeline']
    assert descriptor['record_bytes'] == MIXER.size == 53
    assert descriptor['ring_records'] == 65536
    names = ['audio_samples.bin']
    if 'word_states' in prior['audio_samples']:
        assert 'word_states' in snapshot['audio_samples'], 'Existing word observer missing'
        names.append('audio_word_states.bin')
    for name in names:
        assert reference_bytes(args.capture / name) == reference_bytes(args.prior_samples / name), 'Mixer observer changed existing telemetry'
    if 'initial_live_state' in prior['audio_samples']:
        assert snapshot['audio_samples']['initial_live_state'] == prior['audio_samples']['initial_live_state']
    raw = reference_bytes(args.capture / 'audio_mixer.bin')
    assert hashlib.sha256(raw).hexdigest() == descriptor['sha256']
    calls = descriptor['calls']
    def observations():
        with reference_file(args.capture / 'audio_mixer.bin') as mix, reference_file(args.capture / 'audio_samples.bin') as samples:
            other = iter(sample_frames(samples, calls))
            pcm_end = 0
            for header, rows in mixer_frames(mix, calls):
                sample_header, payload = next(other)
                assert header[0] == sample_header[0] and header[2:] == sample_header[2:]
                yield header[0], rows, list(sample_records(payload, header[2], header[3])), header[4] - pcm_end
                pcm_end = header[4]
            assert next(other, None) is None
    timeline = verify_timeline(observations())
    assert timeline['records'] == descriptor['records']
    rejected = rejection_controls(observations)
    report = dict(preserved_execution=preserved, timeline=timeline,
        actual_record_mutations_rejected=rejected,
        mixer_stream_sha256=descriptor['sha256'], existing_original_telemetry_unchanged=True,
        reference_context=args.reference_context, existing_telemetry_files_unchanged=names,
        scope='All observed original logical intervals, actual accumulator budgets, emitted Paula averages and consumed bytes. Paired PCM/RAM/state/video remain strict. No native latency constant or complete native waveform acceptance.')
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({key: timeline[key] for key in ('records', 'exact_output_averages',
        'exact_consumed_transitions', 'complete_actual_accumulators_and_logical_intervals_matching')},
        sort_keys=True))


if __name__ == '__main__':
    main()
