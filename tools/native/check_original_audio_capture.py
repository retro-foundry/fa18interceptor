"""Verify complete original PCM coverage and read-only emulator recording.

All chunks are checked; no alignment, trimming, resampling or filtering is
applied. This validates the reference recording, not native sound acceptance.
"""
import argparse
import gzip
import hashlib
import json
from pathlib import Path
import wave


def sha(data):
    return hashlib.sha256(data).hexdigest()


def recorded_bytes(folder, name):
    path = folder / name
    if path.exists():
        return path.read_bytes()
    return gzip.decompress((folder / (path.stem + '.dat.gz')).read_bytes())


def validate(capture, baseline, pcm_reference=None):
    report = json.loads((capture / 'snapshot.json').read_text())
    prior = json.loads((baseline / 'snapshot.json').read_text())
    # Wall-clock duration and the new audio descriptor are host observations.
    assert {k:v for k,v in report.items() if k not in ('recorded_audio','audio_events','wall_seconds')} == {
        k:v for k,v in prior.items() if k != 'wall_seconds'}, 'Recording changed original execution'
    for name in ('state.bin', 'chip.bin', 'slow.bin'):
        assert recorded_bytes(capture, name) == recorded_bytes(baseline, name), name
    assert (capture / 'screen.png').read_bytes() == (baseline / 'screen.png').read_bytes()
    audio = report['recorded_audio']
    first_call = audio.get('first_replay_call',1)
    total_calls = audio.get('replay_calls',report['frame'] - first_call + 1)
    pcm_hash = hashlib.sha256()
    covered, calls, chunks = 0, set(), 0
    wav_path = pcm_reference if pcm_reference is not None else capture / audio['file']
    with wave.open(str(wav_path)) as wav, (capture / 'audio_chunks.jsonl').open() as log:
        assert (wav.getnchannels(),wav.getsampwidth(),wav.getframerate()) == (2,2,audio['sample_rate'])
        for line in log:
            row = json.loads(line)
            assert row['first_sample'] == covered and row['frames'] > 0
            data = wav.readframes(row['frames'])
            assert len(data) == 4*row['frames'] and sha(data) == row['pcm_sha256']
            assert first_call <= row['call'] < first_call + total_calls
            covered += row['frames'];calls.add(row['call']);chunks += 1
            pcm_hash.update(data)
        assert covered == wav.getnframes() == audio['sample_frames'] and not wav.readframes(1)
    assert calls == set(range(first_call,first_call+total_calls)), 'One or more original replay calls have no recorded PCM'
    assert pcm_hash.hexdigest() == audio['pcm_sha256']
    with wav_path.open('rb') as file:
        assert hashlib.file_digest(file,'sha256').hexdigest() == audio['wav_sha256']
    return {'scope':'Complete original batch PCM coverage and unchanged emulator execution; native onset/handoffs/filter parity remains open',
        'capture':str(capture), 'baseline':str(baseline), 'pcm_file_checked':str(wav_path),
        'replay_calls':len(calls), 'chunks':chunks,
        'recorded_audio':audio,'authority':report['authority'], 'core_options':report['core_options'],
        'final_original_audio_hash':report['audio_sha256'], 'registers_unchanged':True,
        'complete_ram_state_unchanged':True,'video_unchanged':True, 'no_samples_removed_or_rewritten':True}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture',type=Path,required=True)
    parser.add_argument('--baseline',type=Path,required=True)
    parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--pcm-reference',type=Path,
                        help='Explicit reusable WAV; every block and complete hash must still match the capture')
    args = parser.parse_args()
    result = validate(args.capture,args.baseline,args.pcm_reference)
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,indent=2)+'\n')
    print(f"All {result['replay_calls']} original calls / {result['recorded_audio']['sample_frames']} stereo samples recorded; complete execution unchanged")


if __name__ == '__main__':
    main()
