"""Compare observed original Paula averages through the linked native filter.

Startup only: the preceding recorded PCM and consumed-byte history must be
silent. The original frontend's fixed x2 finish and muted-click 2/3 mix are
explicit reference adapters; neither changes playable gain or game clocks.
"""
import argparse
import array
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

from check_original_audio_capture import reference_bytes, reference_file
from check_original_filter_response import pcm
from check_original_audio_mixer import mixer_frames
from build_original_audio_probe import ROOT, SOURCE


def sha(data):
    return hashlib.sha256(data).hexdigest()


def truncated(value, divisor):
    return (abs(value) // divisor) * (-1 if value < 0 else 1)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('capture', 'timeline-report', 'native-filter', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    args = parser.parse_args()
    snapshot = json.loads((args.capture / 'snapshot.json').read_text())
    timeline = json.loads(args.timeline_report.read_text())
    assert timeline['reference_context'] == 'startup'
    assert timeline['preserved_execution']['all_pcm_chunks_and_samples_exact']
    assert timeline['preserved_execution']['unchanged_complete_ram_state']
    assert timeline['timeline']['complete_actual_accumulators_and_logical_intervals_matching']
    descriptor = snapshot['audio_samples']['mixer_timeline']
    assert timeline['mixer_stream_sha256'] == descriptor['sha256']
    assert timeline['preserved_execution']['authority'] == snapshot['authority']
    assert timeline['observed_mixer_call_range'] == descriptor['observed_call_range']
    assert sha(reference_bytes(args.capture / 'audio_mixer.bin')) == descriptor['sha256']
    assert snapshot['core_options']['puae_sound_interpol'] == 'anti'
    assert snapshot['core_options']['puae_sound_filter'] == 'emulated'
    assert snapshot['core_options']['puae_sound_stereo_separation'] == '100%'
    assert snapshot['core_options']['puae_floppy_sound'] == '100'
    pinned = json.loads((ROOT / 'analysis/figures/native_pcm_filter_checkpoint.json').read_text())
    filter_sha = sha(args.native_filter.read_bytes())
    assert filter_sha in {pinned[key]['native_filter_sha256'] for key in pinned
                          if isinstance(pinned[key], dict) and 'native_filter_sha256' in pinned[key]}
    actual = pcm(args.capture, snapshot)
    first, last = descriptor['observed_call_range']
    assert last == descriptor['calls'], 'This startup comparison must retain the final replay call'
    first_sample = None
    averages, first_nonzero = array.array('h'), [None] * 4
    first_transition, first_nonzero_transition = [None] * 4, [None] * 4
    nonzero_output = [None] * 2
    output_index = 0
    previous_pcm_end = 0
    with reference_file(args.capture / 'audio_mixer.bin') as stream:
        for header, rows in mixer_frames(stream, descriptor['calls']):
            call = header[0]
            if call == first:
                first_sample = previous_pcm_end
                assert not any(actual[:4 * first_sample]), 'Unobserved prefix is not silent'
            for row in rows:
                assert first <= call <= last, 'Observed averages outside declared range'
                if row['kind'] >= 3:
                    channel = row['kind'] - 3
                    record = dict(call=call, service_cycle=row['service_cycle'],
                                  logical_cycle=row['logical_cycle'], value=row['values'][channel])
                    if first_transition[channel] is None: first_transition[channel] = record
                    if record['value'] and first_nonzero_transition[channel] is None:
                        first_nonzero_transition[channel] = record
                if row['kind'] != 2:
                    continue
                index = first_sample + output_index
                for channel, value in enumerate(row['values']):
                    if value and first_nonzero[channel] is None:
                        first_nonzero[channel] = dict(pcm_frame=index, call=call, value=value,
                            logical_cycle=row['logical_cycle'], service_cycle=row['service_cycle'])
                # audio.c:sample16si_anti_handler, FINISH_DATA(bits=15).
                pair = (2 * (row['values'][0] + row['values'][3]),
                        2 * (row['values'][1] + row['values'][2]))
                assert all(-32768 <= value <= 32767 for value in pair)
                averages.extend(pair)
                output_index += 1
            if first <= call <= last:
                assert len([r for r in rows if r['kind'] == 2]) == header[4] - previous_pcm_end
            previous_pcm_end = header[4]
    assert output_index == timeline['timeline']['exact_output_averages']
    assert first_sample + output_index == snapshot['recorded_audio']['sample_frames']
    assert all(row is None or row['call'] >= first for row in
               timeline['preserved_execution']['first_samples_by_channel'])
    assert any(averages), 'Audible startup required'
    args.out.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='mixer-filter-', dir=args.out.parent) as folder:
        work = Path(folder)
        source, output = work / 'paula.pcm', work / 'filtered.pcm'
        # Silent prefix is outside mixer-observation scope, verified above.
        source.write_bytes(bytes(4 * first_sample) + averages.tobytes())
        results, plain_sha = [], None
        for block in (2048, 13):
            subprocess.run([str(args.native_filter.resolve()), str(source), str(output),
                '44100', str(block), 'plain'], check=True, capture_output=True)
            plain = output.read_bytes()
            if plain_sha is None: plain_sha = sha(plain)
            else: assert sha(plain) == plain_sha, 'Native filter depends on block boundaries'
            samples = array.array('h', plain)
            expected = array.array('h', (truncated(value * 2, 3) for value in samples)).tobytes()
            # driveclick.c:driveclick_mix retains this gain with muted clicks.
            differences = [(index, x, y) for index, (x, y) in enumerate(zip(
                array.array('h', actual), array.array('h', expected))) if x != y]
            assert len(actual) == len(expected)
            results.append(dict(block=block, different_samples=len(differences),
                first_differences=differences[:16], expected_pcm_sha256=sha(expected)))
        for index, value in enumerate(array.array('h', actual)):
            if value and nonzero_output[index % 2] is None:
                nonzero_output[index % 2] = index // 2
        controls = {}
        for change in ('drop_first_nonzero_average', 'swap_stereo_averages'):
            changed = array.array('h', averages)
            if change == 'drop_first_nonzero_average':
                index = next(i for i, value in enumerate(changed) if value)
                changed[index] = 0
            else:
                for index in range(0, len(changed), 2):
                    changed[index], changed[index + 1] = changed[index + 1], changed[index]
            source.write_bytes(bytes(4 * first_sample) + changed.tobytes())
            subprocess.run([str(args.native_filter.resolve()), str(source), str(output),
                '44100', '2048', 'plain'], check=True, capture_output=True)
            altered = array.array('h', (truncated(value * 2, 3) for value in
                                      array.array('h', output.read_bytes())))
            different = sum(x != y for x, y in zip(array.array('h', actual), altered))
            assert different, f'Actual-average mutation accepted: {change}'
            controls[change] = dict(rejected=True, different_pcm_samples=different)
        report = dict(scope='All recorded startup PCM from observed original Paula averages through the linked native A500/LED filter, with explicit original frontend finish/muted-click mix. Silent preceding PCM is checked; native game scheduling/onset remains separate.',
            authority=snapshot['authority'], timeline_report_sha256=sha(args.timeline_report.read_bytes()),
            observed_mixer_call_range=[first, last], observed_pcm_range=[first_sample, previous_pcm_end],
            preceding_recorded_pcm_silent=True, preceding_consumed_bytes_absent=True,
            native_filter_sha256=filter_sha, native_filter_source_sha256=sha((ROOT / 'port/game/native/audio.c').read_bytes()),
            reference_sources_sha256={name:sha((SOURCE / 'sources/src' / name).read_bytes()) for name in ('audio.c', 'driveclick.c')},
            reference_finish_x2_and_muted_click_mix_2_over_3_explicit=True,
            first_transition_by_channel=first_transition, first_nonzero_transition_by_channel=first_nonzero_transition,
            first_nonzero_paula_average_by_channel=first_nonzero,
            first_nonzero_filtered_pcm_frames=nonzero_output,
            stereo_pcm_frames=len(actual)//4, actual_pcm_sha256=sha(actual),
            native_plain_filter_pcm_sha256=plain_sha, cases=results,
            actual_paula_average_mutations=controls,
            blocks_identical=True, native_runtime_changed=False, native_complete_waveform_accepted=False)
        args.out.write_text(json.dumps(report, indent=2) + '\n')
        assert all(row['different_samples'] == 0 for row in results), f'Original mixer/filter PCM rejected; see {args.out}'
    print(f'Every {len(actual)//4} startup PCM frame matches observed Paula averages through the native filter; native scheduling remains open')


if __name__ == '__main__':
    main()
