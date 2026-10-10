"""Validate actual reference word lifecycles and the warm zero-word boundary.

Requires complete ordinary PCM/sample/DMA execution preservation first. No
serialized endpoint is substituted for a live pointer/length transition.
"""
import argparse
import hashlib
import json
from pathlib import Path
import sys

from check_original_audio_samples import validate_samples, open_stream, exact, frames as sample_frames
from build_original_audio_probe import ROOT
sys.path.insert(0, str(ROOT / 'scripts'))
from original_audio_samples import WORD_MAGIC, WORD, FRAME, FOOTER, CYCLE_UNIT


def records(path, descriptor, boundaries=None):
    assert descriptor['record_bytes'] == WORD.size and descriptor['ring_records'] == 65536
    with open_stream(path) as file:
        assert hashlib.file_digest(file, 'sha256').hexdigest() == descriptor['sha256']
        file.seek(0)
        assert exact(file, len(WORD_MAGIC)) == WORD_MAGIC
        call, total, maximum, previous_boundary = 0, 0, 0, None
        while True:
            tag = exact(file, 1)
            if tag == b'\0':
                end_call, end_total = FOOTER.unpack(exact(file, FOOTER.size))
                assert end_call == call == descriptor['calls'] and end_total == total == descriptor['records'] and not file.read(1), 'incomplete word-state footer'
                assert maximum == descriptor['max_records_per_call']
                return
            assert tag == b'\1'
            at, count, before, after, pcm_end = FRAME.unpack(exact(file, FRAME.size))
            assert at == call+1 and count <= 65536 and after >= before
            if boundaries is not None:
                assert boundaries.get(at) == (before,after,pcm_end), 'word/sample replay boundary differs'
            assert previous_boundary is None or before == previous_boundary
            previous = before * CYCLE_UNIT
            for row in WORD.iter_unpack(exact(file, count * WORD.size)):
                cycle, pt, lc, address, length, remaining, dat, dat2, hpos, vpos, channel, state, kind, flags, value = row
                assert previous <= cycle < (after+1)*CYCLE_UNIT and channel < 4 and kind in (1,2,3,4)
                assert hpos < 288 and vpos < 1000 and flags < 16
                previous = cycle
                yield dict(call=at, cycle=cycle, pt=pt, lc=lc, address=address, length=length,
                    remaining=remaining, dat=dat, dat2=dat2, beam=[hpos,vpos],
                    channel=channel, state=state, kind=kind, flags=flags, value=value, pcm_end=pcm_end)
            call = at
            total += count
            maximum = max(maximum, count)
            previous_boundary = after


def sample_boundaries(capture, snapshot):
    descriptor = snapshot['audio_samples']
    with open_stream(capture / descriptor['file']) as file:
        return {row[0]: row[2:] for row,payload in sample_frames(file, descriptor['calls'])}


def check_lifecycles(rows):
    pending = [None] * 4
    pointers = [None] * 4
    checked = 0
    for row in rows:
        channel = row['channel']
        if row['kind'] == 1:
            assert pointers[channel] is None, 'pointer read has no delivered word'
            assert row['state'] in (2,3) and row['remaining'] > 0
            assert row['pt'] == row['address'] and row['value'] == (row['remaining'] == 1)
            pointers[channel] = row
        elif row['kind'] == 2:
            assert pending[channel] is None, 'new word arrived before the previous counter event'
            getter = pointers[channel]
            assert getter is not None, 'incoming DMA word has no actual pointer read'
            assert row['address'] == getter['address'] and row['cycle'] == getter['cycle']
            assert row['remaining'] == getter['remaining'] and row['state'] in (2,3)
            assert row['pt'] == (getter['lc'] if getter['value'] else getter['pt']+2)
            pointers[channel] = None
            pending[channel] = row
        elif row['kind'] == 3:
            before = pending[channel]
            assert before is not None, 'processed word has no actual arrival'
            assert before['value'] == row['value'] and before['address'] == row['address']
            assert row['state'] in (2,3)
            expected = row['length'] if before['remaining'] == 1 else (before['remaining']-1) & 65535
            assert row['remaining'] == expected, ('actual counter event differs', row)
            checked += 1
            pending[channel] = None
    assert not any(pending) and not any(pointers), 'recording ends with an unaccounted word'
    return checked


def assess_words(rows):
    checked = check_lifecycles(rows)
    budgets = []
    for channel in (0,1):
        reads = []
        for row in rows:
            if row['channel'] == channel and row['kind'] == 1:
                reads.append(row)
                if row['value']:
                    break
        assert reads and reads[-1]['value'] == 1, 'first loop reset is missing'
        first = reads[0]
        projected = first['pt'] + 2*(first['remaining']-1)
        assert len(reads) == first['remaining']
        assert all(r['pt']+2*(r['remaining']-1) == projected for r in reads)
        assert all(r['lc'] == first['lc'] and r['length'] == first['length'] for r in reads)
        budgets.append(dict(channel=channel, first_actual_read=first,
            reads_through_first_reset=len(reads), projected_last_read=projected,
            declared_last_word=first['lc']+2*first['length']-2, actual_reset_read=reads[-1]))
    subject = [r for r in rows if r['channel'] == 1 and r['address'] == 0x25a72 and r['kind'] in (1,2,3)]
    assert len(subject) == 3 and [r['kind'] for r in subject] == [1,2,3]
    getter, incoming, processed = subject
    assert getter['remaining'] == incoming['remaining'] == 1
    assert getter['pt'] == getter['address'] and getter['value'] == 1
    assert incoming['pt'] == getter['lc'] and incoming['value'] == 0
    assert processed['remaining'] == processed['length']
    assert all(r['call'] == 94 for r in subject)
    assert processed['cycle']-incoming['cycle'] == CYCLE_UNIT
    preceding = [r for r in rows if r['channel'] == 1 and r['kind'] == 3 and r['cycle'] < getter['cycle']][-3:]
    following = next(r for r in rows if r['channel'] == 1 and r['kind'] == 1 and r['cycle'] > getter['cycle'])
    assert following['address'] == getter['lc']
    return dict(actual_running_counter_events_checked=checked,
        initial_live_pointer_budgets=budgets, zero_word_lifecycle=subject,
        preceding_counter_events=preceding, following_pointer_read=following)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture', type=Path, required=True)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--dma-reference', type=Path,
                        help='Explicit complete original fetch witness for buffered words outside this stopped grid')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    validation = validate_samples(args.capture, args.baseline, dma_reference=args.dma_reference)
    snapshot = json.loads((args.capture / 'snapshot.json').read_text())
    descriptor = snapshot['audio_samples']['word_states']
    assert descriptor['record_bytes'] == WORD.size == 38
    assert descriptor['calls'] == validation['replay_calls'] == 94
    rows = list(records(args.capture / descriptor['file'], descriptor, sample_boundaries(args.capture, snapshot)))
    assessment = assess_words(rows)
    report = dict(accepted_evidence=True, preserved_execution=validation,
        word_state_descriptor=descriptor, **assessment,
        source_rules=['audio_getpt', 'AUDxDAT_addr', 'event_audxdat_func'],
        scope='Actual warm-reference last-word request, delivered zero and remaining-length reload. Full ordinary PCM/DMA/sample execution preserved. This does not accept native waveform timing or infer a cold-start padding/delay rule.')
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2)+'\n')
    print(f"{assessment['actual_running_counter_events_checked']} actual running counter events checked; warm zero-word reload traced")


if __name__ == '__main__':
    main()
