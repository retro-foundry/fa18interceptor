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


from audio_mixer_stream import mixer_frames


from audio_mixer_stream import quotient, paired_observations, verify_timeline


from audio_mixer_stream import rejection_controls


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
    observed_range = descriptor.get('observed_call_range', [1, calls])
    def observations():
        with reference_file(args.capture / 'audio_mixer.bin') as mix, reference_file(args.capture / 'audio_samples.bin') as samples:
            yield from paired_observations(mix, samples, calls, observed_range)
    timeline = verify_timeline(observations(), observed_range[0])
    assert timeline['records'] == descriptor['records']
    rejected = rejection_controls(observations, observed_range[0])
    report = dict(preserved_execution=preserved, timeline=timeline,
        actual_record_mutations_rejected=rejected,
        mixer_stream_sha256=descriptor['sha256'], existing_original_telemetry_unchanged=True,
        reference_context=args.reference_context, existing_telemetry_files_unchanged=names,
        observed_mixer_call_range=observed_range,
        scope='All original logical intervals, actual accumulator budgets, emitted Paula averages and consumed bytes within the explicit mixer call range. Entire replay PCM/RAM/state/video remain strict. No native latency constant or complete native waveform acceptance.')
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({key: timeline[key] for key in ('records', 'exact_output_averages',
        'exact_consumed_transitions', 'complete_actual_accumulators_and_logical_intervals_matching')},
        sort_keys=True))


if __name__ == '__main__':
    main()
