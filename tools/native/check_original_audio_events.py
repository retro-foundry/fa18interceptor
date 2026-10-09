"""Check complete original audio-event coverage against its recorded PCM.

Voice addresses and all 64 record bytes are retained, without remapping slots
or shifting time. This validates reference telemetry, not native sound parity.
"""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import sys

from check_original_audio_capture import recorded_bytes, validate

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
from original_audio_events import audio_state


def validate_events(capture, baseline, pcm_reference=None):
    result = validate(capture, baseline, pcm_reference)
    report = json.loads((capture / 'snapshot.json').read_text())
    descriptor = report['audio_events']
    path = capture / descriptor['file']
    with path.open('rb') as log:
        assert hashlib.file_digest(log, 'sha256').hexdigest() == descriptor['sha256']
    sample_ends = {}
    for line in (capture / 'audio_chunks.jsonl').read_text().splitlines():
        row = json.loads(line)
        sample_ends[row['call']] = row['first_sample'] + row['frames']
    first = report['recorded_audio']['first_replay_call']
    next_call, rows, boundaries = first, 0, 0
    register_counts, source_counts = Counter(), Counter()
    slot_counts = [0] * 4
    first_slot_changes = []
    previous = None
    initial = None
    buffer_loads = [0] * 4
    with path.open() as log:
        for line in log:
            row = json.loads(line)
            rows += 1
            kind = row['kind']
            if kind == 'write':
                assert initial is not None and row['call'] == next_call
                address = row['address']
                assert (0xdff0a0 <= address <= 0xdff0da or
                        address in (0xdff096, 0xdff09a, 0xdff09c, 0xdff09e))
                assert 0 <= row['value'] <= 65535
                register_counts[f'{address:06X}'] += 1
                source_counts[f"{row['source']:06X}"] += 1
                if 0xdff0a0 <= address <= 0xdff0da and (address - 0xdff0a0) % 16 == 2:
                    buffer_loads[(address - 0xdff0a0) // 16] += 1
                continue
            if kind == 'initial':
                assert rows == 1 and row['call'] == first - 1 and row['sample_frames'] == 0
                initial = row
            else:
                assert kind == 'boundary' and initial is not None
                assert row['call'] == next_call and row['sample_frames'] == sample_ends[next_call]
                boundaries += 1
                next_call += 1
            assert len(row['voices']) == 4 and len(bytes.fromhex(row['master_volume'])) == 4
            addresses = []
            for channel, voice in enumerate(row['voices']):
                address = voice['address']
                addresses.append(address)
                if address:
                    assert len(bytes.fromhex(voice['record'])) == 64
                    assert address + 64 <= 0x80000 or 0xc00000 <= address <= 0xc80000 - 64
                else:
                    assert voice['record'] is None
                if previous is not None and address != previous[channel]:
                    slot_counts[channel] += 1
                    if len(first_slot_changes) < 16:
                        first_slot_changes.append({'call': row['call'], 'channel': channel,
                            'before': f'{previous[channel]:06X}', 'after': f'{address:06X}'})
            previous = addresses
    assert rows == descriptor['rows'] and boundaries == result['replay_calls']
    final = audio_state(recorded_bytes(capture, 'state.bin'))
    assert final == descriptor['final_hardware']
    result.update(audio_events=descriptor, event_rows=rows, boundary_rows=boundaries,
        register_writes=dict(sorted(register_counts.items())),
        source_writes=dict(sorted(source_counts.items())),
        buffer_pointer_low_writes=buffer_loads, voice_slot_changes=slot_counts,
        first_slot_changes=first_slot_changes,
        initial_hardware={key: initial[key] for key in final},
        filter_scope='LED pin on/off only at sealed initial and ordinary final endpoints; within-frame duty and intermediate CIA writes unproven',
        native_sound_acceptance=False)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture', type=Path, required=True)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--pcm-reference', type=Path,
                        help='Explicit reusable WAV, still checked against every capture block/hash')
    args = parser.parse_args()
    result = validate_events(args.capture, args.baseline, args.pcm_reference)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, indent=2) + '\n')
    print(f"All {result['boundary_rows']} original PCM/voice boundaries and {result['event_rows']} event rows checked; native sound acceptance remains open")


if __name__ == '__main__':
    main()
