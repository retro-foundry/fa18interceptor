"""Decode complete legacy Engine9000 logical mixer streams."""

from collections import Counter
from original_audio_samples import CYCLE_UNIT, sample_records
from audio_sample_stream import frames as sample_frames
import copy
from audio_dma_stream import exact
from original_audio_samples import MIXER_MAGIC, MIXER, FRAME, FOOTER, mixer_records

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


def paired_observations(mix, samples, calls, observed_range):
    assert len(observed_range) == 2 and 1 <= observed_range[0] <= observed_range[1] <= calls
    other = iter(sample_frames(samples, calls))
    pcm_end = 0
    for header, rows in mixer_frames(mix, calls):
        sample_header, payload = next(other)
        assert header[0] == sample_header[0] and header[2:] == sample_header[2:]
        consumed = list(sample_records(payload, header[2], header[3]))
        if observed_range[0] <= header[0] <= observed_range[1]:
            yield header[0], rows, consumed, header[4] - pcm_end
        else:
            assert not rows, 'Mixer records outside the declared call range'
        pcm_end = header[4]
    assert next(other, None) is None


def verify_timeline(frames, first_call=1):
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
                assert areas is None and call == first_call and not counts, 'Repeated/missing initial mixer state'
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




def rejection_controls(observations, first_call=1):
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
            verify_timeline(altered(), first_call)
        except AssertionError:
            assert changed, 'Control never changed an actual record'
            rejected[change] = True
        else:
            raise AssertionError(f'Mixer mutation accepted: {change}')
    return rejected


