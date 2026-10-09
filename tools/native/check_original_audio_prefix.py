"""Verify an independently stopped replay against a complete sound prefix.

Endpoint state comes from ordinary final serialization, after the last replay
call. It is not the old requested restore payload or the completed DMA grid's
last slot. No intermediate serialization is used.
"""
import argparse
import gzip
import hashlib
import io
import json
from pathlib import Path
import wave

from check_original_audio_dma import frames, open_stream
from check_original_audio_capture import recorded_bytes
from original_audio_events import audio_state


def sha(data):
    return hashlib.sha256(data).hexdigest()


def validate_prefix(prefix, complete, pcm_reference):
    small = json.loads((prefix / 'snapshot.json').read_text())
    full = json.loads((complete / 'snapshot.json').read_text())
    count = small['recorded_audio']['replay_calls']
    assert 0 < count < full['recorded_audio']['replay_calls']
    assert small['recorded_audio']['first_replay_call'] == full['recorded_audio']['first_replay_call'] == 1
    for key in ('core_sha256', 'config_sha256', 'initial_state_sha256', 'recording_sha256'):
        assert small['authority'][key] == full['authority'][key], key
    assert small['core_options'] == full['core_options']
    assert small['frame'] == small['hardware_frame'] == count
    state = recorded_bytes(prefix, 'state.bin')
    assert sha(state) == small['authority']['snapshot_sha256']
    for mapping in small['memory']:
        assert sha(recorded_bytes(prefix, mapping['file'])) == mapping['sha256']
    # Bind the complete reference and its exact requested PCM window.
    with pcm_reference.open('rb') as file:
        assert hashlib.file_digest(file, 'sha256').hexdigest() == full['recorded_audio']['wav_sha256']
    with wave.open(str(pcm_reference)) as file:
        assert (file.getnchannels(), file.getsampwidth(), file.getframerate()) == (2, 2, 44100)
        pcm = file.readframes(small['recorded_audio']['sample_frames'])
    assert len(pcm) == small['recorded_audio']['sample_frames'] * 4
    assert sha(pcm) == small['recorded_audio']['pcm_sha256']
    # Reconstruct only the canonical standard WAV header to allow lossless
    # duplicate-WAV removal; never alter, shift or resample any PCM byte.
    expected = io.BytesIO()
    with wave.open(expected, 'wb') as file:
        file.setparams((2, 2, 44100, 0, 'NONE', 'not compressed'))
        file.writeframes(pcm)
    assert sha(expected.getvalue()) == small['recorded_audio']['wav_sha256']
    actual_wav = prefix / small['recorded_audio']['file']
    if actual_wav.exists():
        assert actual_wav.read_bytes() == expected.getvalue()
    chunk_log = (prefix / 'audio_chunks.jsonl').read_bytes()
    full_chunks = (complete / 'audio_chunks.jsonl').read_bytes().splitlines(keepends=True)
    assert chunk_log == b''.join(full_chunks[:count])
    for capture, descriptor in ((prefix, small), (complete, full)):
        assert sha((capture / 'audio_events.jsonl').read_bytes()) == descriptor['audio_events']['sha256']
        with open_stream(capture / descriptor['audio_dma']['file']) as file:
            assert hashlib.file_digest(file, 'sha256').hexdigest() == descriptor['audio_dma']['sha256']
    full_events = []
    last_boundary = None
    with (complete / 'audio_events.jsonl').open('rb') as events:
        for line in events:
            row = json.loads(line)
            if row['call'] > count:
                break
            full_events.append(line)
            if row['kind'] == 'boundary':
                last_boundary = row
    assert (prefix / 'audio_events.jsonl').read_bytes() == b''.join(full_events)
    assert last_boundary and last_boundary['call'] == count
    dma_calls, dma_words = 0, 0
    with open_stream(prefix / 'audio_dma.bin') as a, open_stream(complete / 'audio_dma.bin') as b:
        whole = frames(b, 1, full['audio_dma']['calls'])
        for entry, payload in frames(a, 1, count):
            expected_entry, expected_payload = next(whole)
            assert entry == expected_entry and payload == expected_payload
            dma_calls += 1
            dma_words += entry[8]
        # Complete stream framing is checked too, including its terminal footer.
        for entry, payload in whole:
            pass
    assert dma_calls == count and dma_words == small['audio_dma']['fetched_words']
    hardware = audio_state(state)
    return dict(scope='All stopped-replay PCM, chunks, events and actual DMA words equal '
        'the complete recording prefix. Final serialized hardware describes the actual '
        'post-call endpoint; requested restore hardware is an earlier context.',
        calls=count, stereo_frames=len(pcm)//4, actual_dma_words=dma_words,
        authority=small['authority'], complete_authority=full['authority'],
        prefix_snapshot_sha256=sha((prefix / 'snapshot.json').read_bytes()),
        complete_snapshot_sha256=sha((complete / 'snapshot.json').read_bytes()),
        endpoint_hardware=hardware, endpoint_voice_boundary=last_boundary,
        prefix_pcm_sha256=sha(pcm), prefix_events_sha256=small['audio_events']['sha256'],
        prefix_dma_sha256=small['audio_dma']['sha256'],
        completed_dma_grid_is_not_live_endpoint=True,
        native_waveform_accepted=False)


def retain(prefix, pcm_reference):
    rows = []
    for name in ('state.bin', 'chip.bin', 'slow.bin', 'audio_dma.bin'):
        raw = prefix / name
        if not raw.exists():
            continue
        data = raw.read_bytes()
        compressed = prefix / (name[:-4] + '.dat.gz' if name != 'audio_dma.bin' else name + '.gz')
        compressed.write_bytes(gzip.compress(data, mtime=0))
        assert gzip.decompress(compressed.read_bytes()) == data
        rows.append(dict(file=compressed.name, decoded_sha256=sha(data), decoded_bytes=len(data),
                         retained_sha256=sha(compressed.read_bytes()), retained_bytes=compressed.stat().st_size))
        raw.unlink()
    wav = prefix / 'original.wav'
    if wav.exists():
        wav.unlink()  # validate_prefix already checked every byte before this call.
    return dict(files=rows, pcm_reference=str(pcm_reference), duplicate_wav_removed=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--prefix', type=Path, required=True)
    parser.add_argument('--complete', type=Path, required=True)
    parser.add_argument('--pcm-reference', type=Path, required=True)
    parser.add_argument('--retain-compressed', action='store_true')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    result = validate_prefix(args.prefix, args.complete, args.pcm_reference)
    if args.retain_compressed:
        result['retention'] = retain(args.prefix, args.pcm_reference)
        assert validate_prefix(args.prefix, args.complete, args.pcm_reference)['endpoint_hardware'] == result['endpoint_hardware']
    args.out.write_text(json.dumps(result, indent=2) + '\n')
    print(f"All {result['calls']} replay calls / {result['actual_dma_words']} DMA words / "
          f"{result['stereo_frames']} stereo frames equal the complete recorded prefix")


if __name__ == '__main__':
    main()
