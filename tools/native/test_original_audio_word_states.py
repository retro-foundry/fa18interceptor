"""Reject damaged real warm-reference word telemetry without replaying the game."""
import argparse
from copy import deepcopy
import hashlib
import json
from pathlib import Path
import tempfile

from check_original_audio_word_states import records, assess_words, sample_boundaries, WORD_MAGIC, FRAME, FOOTER, WORD
from check_original_audio_samples import open_stream


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    snapshot = json.loads((args.capture / 'snapshot.json').read_text())
    descriptor = snapshot['audio_samples']['word_states']
    path = args.capture / descriptor['file']
    boundaries = sample_boundaries(args.capture, snapshot)
    rows = list(records(path, descriptor, boundaries))
    accepted = json.loads((args.capture / 'comparison.json').read_text())
    assert accepted['accepted_evidence'] and accepted['word_state_descriptor'] == descriptor
    assessment = assess_words(rows)
    assert all(accepted[key] == value for key, value in assessment.items())
    guards = []

    def reject(name, operation):
        try:
            operation()
        except (AssertionError, ValueError) as error:
            guards.append(dict(name=name, rejected=True, reason=str(error)))
        else:
            raise AssertionError('Damaged real telemetry accepted: '+name)

    with open_stream(path) as file:
        payload = file.read()
    offset = len(WORD_MAGIC)+1+FRAME.size
    first = list(WORD.unpack(payload[offset:offset+WORD.size]))
    malformed = dict(truncated_footer=payload[:-1], trailing_data=payload+b'x',
        missing_call=payload[:len(WORD_MAGIC)+1]+FRAME.pack(2,*FRAME.unpack(payload[len(WORD_MAGIC)+1:offset])[1:])+payload[offset:],
        false_footer_count=payload[:-FOOTER.size]+FOOTER.pack(descriptor['calls'],descriptor['records']-1))
    frame = list(FRAME.unpack(payload[len(WORD_MAGIC)+1:offset]))
    frame[-1] += 1
    malformed['changed_pcm_boundary'] = payload[:len(WORD_MAGIC)+1]+FRAME.pack(*frame)+payload[offset:]
    for name, field, value in (('invalid_channel',10,4),('invalid_beam',8,288),('invalid_kind',12,0),('invalid_flags',13,16)):
        changed = first.copy(); changed[field] = value
        malformed[name] = payload[:offset]+WORD.pack(*changed)+payload[offset+WORD.size:]
    with tempfile.TemporaryDirectory() as temporary:
        damaged = Path(temporary) / 'audio_word_states.bin'
        for name, data in malformed.items():
            damaged.write_bytes(data)
            local = dict(descriptor, sha256=hashlib.sha256(data).hexdigest())
            reject(name, lambda: list(records(damaged, local, boundaries)))
    subject = [i for i,r in enumerate(rows) if r['channel']==1 and r['address']==0x25a72 and r['kind'] in (1,2,3)]
    getter, incoming, processed = subject
    for name, index, field, value in (
        ('wrong_pointer_reset',getter,'value',0),
        ('wrong_pointer_destination',incoming,'pt',rows[incoming]['pt']+2),
        ('changed_delivered_address',incoming,'address',0x25a70),
        ('changed_counter_reload',processed,'remaining',16384),
        ('changed_counter_state',processed,'state',0),
        ('changed_zero_word',incoming,'value',1)):
        changed = deepcopy(rows); changed[index][field] = value
        reject(name, lambda: assess_words(changed))
    for name, index in (('missing_pointer_read',getter),('missing_counter_event',processed)):
        changed = rows[:index]+rows[index+1:]
        reject(name, lambda: assess_words(changed))
    report = dict(real_stream_sha256=descriptor['sha256'], actual_records=len(rows),
        actual_running_counter_events=assessment['actual_running_counter_events_checked'],
        rejection_controls=guards, controls_passed=len(guards),
        scope='Mutations of the complete real 94-call word stream and live word lifecycle; local binary digests are recomputed. No synthetic original execution.')
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2)+'\n')
    print(f'{len(guards)} damaged-real-word controls rejected; original assessment unchanged')


if __name__ == '__main__':
    main()
