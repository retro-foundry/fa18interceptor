"""Validate observed original sample use against unchanged recorded execution.

The observer DLL's declared identity is the only execution identity allowed to
differ. Every remaining snapshot field, PCM byte, event, DMA word, RAM/state
byte and video byte stays strict. This is not native waveform acceptance.
"""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import sys
import wave

from check_original_audio_capture import recorded_bytes, reference_file, reference_bytes
from check_original_audio_dma import frames as dma_frames, open_stream, FETCH, exact
from build_original_audio_probe import observer_source, SOURCE, ROOT

sys.path.insert(0, str(ROOT / 'scripts'))
from original_audio_samples import MAGIC, FRAME, SAMPLE, FOOTER, CYCLE_UNIT, CLOCK, sample_records


def sha(data):
    return hashlib.sha256(data).hexdigest()


def frames(file, calls):
    assert exact(file, len(MAGIC)) == MAGIC, 'Invalid consumed-sample stream header'
    next_call, previous_cycle, total = 1, None, 0
    while True:
        tag = exact(file, 1)
        if tag == b'\0':
            footer_calls, samples = FOOTER.unpack(exact(file, FOOTER.size))
            assert next_call == calls+1 and footer_calls == calls and samples == total and not file.read(1), 'Incomplete consumed-sample footer'
            return
        assert tag == b'\1'
        call, count, before, after, pcm_end = FRAME.unpack(exact(file, FRAME.size))
        assert call == next_call and call <= calls and count <= 65536 and after >= before, 'Lost sample call/coverage'
        assert previous_cycle is None or before == previous_cycle, 'Lost consumed-sample cycle boundary'
        payload = exact(file, count*SAMPLE.size)
        yield (call, count, before, after, pcm_end), payload
        next_call += 1
        previous_cycle = after
        total += count


def execution(report):
    return {key: ({k:v for k,v in value.items() if k != 'core_sha256'} if key == 'authority' else value)
        for key, value in report.items() if key not in ('audio_samples', 'wall_seconds')}


def validate_samples(capture, baseline, pcm_reference=None, dma_reference=None, reference_context='demo'):
    observed = json.loads((capture / 'snapshot.json').read_text())
    original = json.loads((baseline / 'snapshot.json').read_text())
    descriptor = observed['audio_samples']
    manifest = descriptor['observer']
    manifest_path = Path(descriptor.get('observer_manifest_file', str(Path(manifest['compile'][-2]).parent / 'build.json')))
    assert json.loads(manifest_path.read_text()) == manifest
    assert sha(manifest_path.read_bytes()) == descriptor['observer_manifest_sha256']
    assert reference_context in ('demo', 'startup'), 'unknown reference context'
    assert dma_reference is None or reference_context == 'demo', 'Wider DMA witness is only pinned for the Demo context'
    if reference_context == 'startup':
        trusted = json.loads((ROOT / 'analysis/figures/native_original_voice_layout_checkpoint.json').read_text())['separate_launch']
        assert original['recorded_audio']['replay_calls'] == trusted['replay_calls'], 'different startup recording coverage'
    else:
        trusted = json.loads((ROOT / 'analysis/figures/native_original_audio_dma_checkpoint.json').read_text())
    assert {k:v for k,v in original['authority'].items() if k != 'snapshot_sha256'} == {
        k:v for k,v in trusted['authority'].items() if k != 'snapshot_sha256'}
    assert observed['authority']['core_sha256'] == manifest['core_sha256']
    assert original['authority']['core_sha256'] == manifest['baseline_core_sha256']
    assert observed['authority']['core_sha256'] != original['authority']['core_sha256'], 'Isolated observer identity required'
    assert descriptor['record_bytes'] == manifest['record_bytes'] == SAMPLE.size == 20
    assert descriptor['ring_records'] == manifest['ring_records'] == 65536
    assert descriptor['boundary_cycle_unit'] == CYCLE_UNIT == 512
    assert sha((SOURCE / 'sources/src/audio.c').read_bytes()) == manifest['source_sha256']
    probe = ROOT / 'tools/native/original_audio_sample_probe.inc'
    assert sha(probe.read_bytes()) == manifest['observer_include_sha256']
    generated_path = Path(manifest['compile'][-2])
    assert sha(generated_path.read_bytes()) == manifest['probe_source_sha256']
    word_probe = None
    if 'word_state_include_sha256' in manifest:
        word_path = ROOT / 'tools/native/original_audio_word_probe.inc'
        assert sha(word_path.read_bytes()) == manifest['word_state_include_sha256']
        word_probe = word_path.read_text()
    live_probe = None
    if 'live_state_include_sha256' in manifest:
        live_path = ROOT / 'tools/native/original_audio_live_probe.inc'
        assert sha(live_path.read_bytes()) == manifest['live_state_include_sha256']
        live_probe = live_path.read_text()
    mixer_probe = None
    if 'mixer_include_sha256' in manifest:
        mixer_path = ROOT / 'tools/native/original_audio_mixer_probe.inc'
        assert sha(mixer_path.read_bytes()) == manifest['mixer_include_sha256']
        mixer_probe = mixer_path.read_text()
    clock_probe = None
    if 'mixer_clock_include_sha256' in manifest:
        clock_path = ROOT / 'tools/native/original_audio_clock_probe.inc'
        assert mixer_probe is not None and manifest['mixer_clock_record_bytes'] == CLOCK.size
        assert sha(clock_path.read_bytes()) == manifest['mixer_clock_include_sha256']
        clock_probe = clock_path.read_text()
    assert generated_path.read_text() == observer_source((SOURCE / 'sources/src/audio.c').read_text(), probe.read_text(), word_probe, live_probe, mixer_probe, clock_probe)
    assert execution(observed) == execution(original), 'Observer changed original execution'
    for name in ('state.bin', 'chip.bin', 'slow.bin'):
        assert recorded_bytes(capture, name) == recorded_bytes(baseline, name), name
    assert (capture / 'screen.png').read_bytes() == (baseline / 'screen.png').read_bytes()
    for name in ('audio_events.jsonl', 'audio_chunks.jsonl'):
        assert reference_bytes(capture / name) == reference_bytes(baseline / name), name
    assert sha(reference_bytes(capture / 'audio_events.jsonl')) == observed['audio_events']['sha256']
    wav_path = pcm_reference or (capture / observed['recorded_audio']['file'])
    with reference_file(wav_path) as file:
        assert hashlib.file_digest(file, 'sha256').hexdigest() == observed['recorded_audio']['wav_sha256']
    covered, calls = 0, 0
    pcm_hash = hashlib.sha256()
    ends = {}
    with reference_file(wav_path) as source, wave.open(source) as wav, reference_file(capture / 'audio_chunks.jsonl') as chunks:
        assert (wav.getnchannels(), wav.getsampwidth(), wav.getframerate()) == (2, 2, 44100)
        for line in chunks:
            row = json.loads(line)
            assert row['first_sample'] == covered and row['call'] == calls+1 and row['frames'] > 0
            data = wav.readframes(row['frames'])
            assert len(data) == 4*row['frames'] and sha(data) == row['pcm_sha256']
            covered += row['frames']
            calls += 1
            pcm_hash.update(data)
            ends[calls] = covered
        assert not wav.readframes(1) and covered == observed['recorded_audio']['sample_frames']
    assert pcm_hash.hexdigest() == observed['recorded_audio']['pcm_sha256']
    assert calls == descriptor['calls'] == observed['recorded_audio']['replay_calls']
    dma_available = 'audio_dma' in observed
    assert dma_available == ('audio_dma' in original), 'Different DMA capture contracts'
    assert dma_available or reference_context == 'startup', 'Demo comparison requires complete DMA evidence'
    fetched = [dict() for _ in range(4)] if dma_available else None
    if dma_available:
        with open_stream(capture / 'audio_dma.bin') as a, open_stream(baseline / 'audio_dma.bin') as b:
            assert hashlib.file_digest(a, 'sha256').hexdigest() == observed['audio_dma']['sha256']
            assert hashlib.file_digest(b, 'sha256').hexdigest() == original['audio_dma']['sha256']
            a.seek(0); b.seek(0)
            prior = dma_frames(b, 1, calls)
            for row, payload in dma_frames(a, 1, calls):
                assert (row, payload) == next(prior), 'Observer changed actual DMA fetches'
                assert row[-1] == ends[row[0]]
                for index, address, value, register, hpos, vpos, channel in FETCH.iter_unpack(payload):
                    if address in fetched[channel]:
                        assert fetched[channel][address] == value, 'Changing sample RAM needs a time-specific witness'
                    fetched[channel][address] = value
            assert next(prior, None) is None
    if dma_reference is not None:
        assert dma_available, 'Wider fetch witness requires paired DMA capture'
        witness = json.loads((dma_reference / 'snapshot.json').read_text())
        assert witness['authority'] == trusted['authority'] and witness['audio_dma']['sha256'] == trusted['audio_dma']['sha256']
        assert {k:v for k,v in witness['authority'].items() if k != 'snapshot_sha256'} == {
            k:v for k,v in original['authority'].items() if k != 'snapshot_sha256'}
        assert witness['core_options'] == original['core_options']
        assert witness['recorded_audio']['replay_calls'] >= calls
        with open_stream(dma_reference / 'audio_dma.bin') as file:
            assert hashlib.file_digest(file, 'sha256').hexdigest() == witness['audio_dma']['sha256']
            file.seek(0)
            for row, payload in dma_frames(file, 1, witness['audio_dma']['calls']):
                for index, address, value, register, hpos, vpos, channel in FETCH.iter_unpack(payload):
                    if address in fetched[channel]:
                        assert fetched[channel][address] == value, 'Changing sample RAM needs a time-specific witness'
                    fetched[channel][address] = value
    with open_stream(capture / descriptor['file']) as file:
        assert hashlib.file_digest(file, 'sha256').hexdigest() == descriptor['sha256']
    channels, unknown, different = [0]*4, [], []
    first = [None]*4
    zero_word = []
    channel2_zero = 0
    frame_counts = Counter()
    total = 0
    with open_stream(capture / descriptor['file']) as file:
        for (call, count, before, after, pcm_end), payload in frames(file, calls):
            assert pcm_end == ends[call], 'Sample observer/PCM boundary differs'
            frame_counts[count] += 1
            for cycle, address, word, value, channel, state, hpos, vpos in sample_records(payload, before, after):
                row = dict(index=total, call=call, cycle=cycle, address=address, word=word,
                           value=value, channel=channel, state=state, beam=[hpos, vpos])
                if first[channel] is None:
                    first[channel] = row
                channels[channel] += 1
                if address == 0xffffffff:
                    unknown.append(row)
                elif fetched is not None and fetched[channel].get(address) != word:
                    different.append(row)
                if channel == 1 and address == 0x25a72:
                    zero_word.append(row)
                if channel == 2 and address == 0:
                    channel2_zero += 1
                total += 1
    assert not different, 'Consumed word differs from actual retained DMA words'
    assert total == descriptor['consumed_bytes'] and channels == descriptor['bytes_by_channel']
    assert len(unknown) == descriptor['unknown_initial_provenance_bytes']
    if reference_context == 'demo':
        assert [row['state'] for row in zero_word] == [2, 3]
        assert all(row['call'] == 94 and row['word'] == row['value'] == 0 for row in zero_word)
        assert zero_word[1]['cycle']-zero_word[0]['cycle'] == 358*CYCLE_UNIT
        assert channel2_zero == 0, 'Original channel-2 startup prefetch reached a sample'
    report = dict(scope='All observed original consumed sample bytes and actual word provenance; complete PCM, '
        'DMA, event log and execution preserved. Service timestamps are not an independent mixer-time reconstruction. '
        'Native onset/handoff/waveform remains open.',
        replay_calls=calls, stereo_pcm_frames=covered, consumed_bytes=total,
        bytes_by_channel=channels, actual_fetch_word_differences=0 if dma_available else None,
        explicit_initial_unknown_provenance=unknown, first_samples_by_channel=first,
        authority=observed['authority'], unmodified_authority=original['authority'],
        observer_manifest_sha256=descriptor['observer_manifest_sha256'],
        sample_stream_sha256=descriptor['sha256'], sample_stream_record_bytes=SAMPLE.size,
        pcm_sha256=observed['recorded_audio']['pcm_sha256'], dma_sha256=observed['audio_dma']['sha256'] if dma_available else None,
        event_log_sha256=observed['audio_events']['sha256'], source_sha256=manifest['source_sha256'],
        fetch_content_witness=str(dma_reference or baseline) if dma_available else None,
        fetch_witness_scope='Same-channel address/word contents from actual DMA. Consumed-byte ordering is observed '
            'directly; this lookup does not infer DMA delivery time. A wider recording is explicit when supplied.',
        unchanged_snapshot_fields=True, unchanged_complete_ram_state=True, unchanged_video=True,
        all_pcm_chunks_and_samples_exact=True, actual_dma_stream_exact=dma_available,
        max_samples_per_call=max(frame_counts), native_runtime_changed=False, native_waveform_accepted=False)
    if reference_context == 'demo':
        report.update(zero_word_samples=zero_word,
            zero_word_classification='Both bytes emitted during existing channel-1 playback',
            channel2_zero_prefetch_emitted_bytes=channel2_zero)
    else:
        report['reference_context'] = reference_context
        if not dma_available:
            report['scope'] = ('All observed original consumed sample bytes; complete PCM, event log and execution preserved. '
                'No separate DMA fetch-coverage witness. Service timestamps are not an independent mixer-time reconstruction. '
                'Native onset/handoff/waveform remains open.')
            report['fetch_witness_scope'] = 'No separate DMA stream; consumed word/byte/address/state records are observed directly.'
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture', type=Path, required=True)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--pcm-reference', type=Path)
    parser.add_argument('--dma-reference', type=Path,
                        help='Explicit wider original fetch recording for words outside the stopped completed grids')
    parser.add_argument('--reference-context', choices=('demo', 'startup'), default='demo',
                        help='Pinned original Demo or independent 5000-call startup recording; never interchangeable')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    report = validate_samples(args.capture, args.baseline, args.pcm_reference, args.dma_reference, args.reference_context)
    args.out.write_text(json.dumps(report, indent=2)+'\n')
    print(f"All {report['replay_calls']} source calls / {report['consumed_bytes']} consumed bytes: "
          'PCM/execution exact; actual consumed-byte provenance retained')


if __name__ == '__main__':
    main()
