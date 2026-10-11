"""Bind actual original restore assignments to the first live word budget.

Snapshots are read from the isolated original DLL, without serialization or
audio advancement. Complete paired original PCM/execution must pass first.
This never supplies a timing or padding rule to native playback.
"""
import argparse
from copy import deepcopy
import hashlib
import json
from pathlib import Path
import sys

from build_original_audio_probe import ROOT
from check_original_audio_word_states import records, sample_boundaries, assess_words
from check_original_audio_samples import validate_samples

sys.path.insert(0, str(ROOT / 'scripts'))
from original_audio_events import audio_state
from original_audio_samples import CYCLE_UNIT


def assess_restore(requested, live, words, first_boundary):
    assert len(live) == 8
    fields = dict(pt='cursor', lc='buffer', length='length_words', remaining='remaining_words',
                  dat='data_word', dat2='data_word', state='state', volume='volume',
                  interrupt_pending='interrupt_pending', next_cycles='next_cycles', drhpos='drhpos')
    restored, before = live[:4], live[4:]
    for channel, (saved, actual, current) in enumerate(zip(requested['channels'], restored, before)):
        assert (actual['channel'], actual['phase']) == (channel, 0)
        assert (current['channel'], current['phase']) == (channel, 1)
        for field, saved_field in fields.items():
            assert actual[field] == saved[saved_field], ('Original restore assignment differs', channel, field)
        assert actual['period_cycles'] == (saved['period'] * CYCLE_UNIT if saved['period'] else 0xffffffff)
        assert actual['flags'] & 3 == saved['flags'] & 3
        assert current['cycle'] >= actual['cycle']
        assert current['cycle'] // CYCLE_UNIT == first_boundary
    assert len({row['cycle'] for row in before}) == 1, 'Read-only snapshot changes time'
    budgets = []
    for channel in (0, 1):
        actual, current = restored[channel], before[channel]
        first = next(row for row in words if row['channel'] == channel and row['kind'] == 1)
        assert actual['lc'] == current['lc'] == first['lc']
        assert actual['length'] == current['length'] == first['length']
        assert 0 < first['remaining'] <= current['remaining'] <= actual['remaining']
        boundary = actual['pt'] + 2 * (actual['remaining'] - 1)
        assert current['pt'] + 2 * (current['remaining'] - 1) == boundary
        assert first['pt'] + 2 * (first['remaining'] - 1) == boundary
        declared = actual['lc'] + 2 * actual['length'] - 2
        budgets.append(dict(channel=channel, original_restore_assignment=actual,
            before_first_ordinary_call=current, first_actual_pointer_read=first,
            projected_last_word=boundary, declared_last_word=declared,
            extra_words_already_in_requested_state=(boundary-declared)//2,
            pointer_words_between_restore_and_observer=(current['pt']-actual['pt'])//2,
            pointer_words_between_observer_and_first_read=(first['pt']-current['pt'])//2,
            intervening_execution_not_observed=True))
    assert [row['extra_words_already_in_requested_state'] for row in budgets] == [0, 1]
    return dict(all_four_original_restore_assignments_match_requested_state=True,
                snapshot_read_does_not_advance_original_time=True, initial_word_budgets=budgets)


def controls(requested, live, words, boundary):
    rejected = []
    for name, index, field, value in (
        ('changed-original-restore-pointer', 1, 'pt', live[1]['pt']-2),
        ('changed-original-restore-count', 1, 'remaining', live[1]['remaining']-1),
        ('changed-pre-replay-pointer', 5, 'pt', live[5]['pt']-2),
        ('changed-pre-replay-count', 5, 'remaining', live[5]['remaining']-1),
        ('advanced-snapshot-time', 5, 'cycle', live[5]['cycle']+CYCLE_UNIT),
        ('swapped-snapshot-role', 5, 'phase', 0)):
        changed = deepcopy(live)
        changed[index][field] = value
        try:
            assess_restore(requested, changed, words, boundary)
        except AssertionError:
            rejected.append(name)
        else:
            raise AssertionError('Accepted changed actual restore evidence: ' + name)
    return rejected


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture', type=Path, required=True)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--prior-words', type=Path, required=True)
    parser.add_argument('--dma-reference', type=Path, required=True)
    parser.add_argument('--restore', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    validation = validate_samples(args.capture, args.baseline, dma_reference=args.dma_reference)
    snapshot = json.loads((args.capture / 'snapshot.json').read_text())
    descriptor = snapshot['audio_samples']
    restored = args.restore.read_bytes()
    assert hashlib.sha256(restored).hexdigest() == validation['authority']['initial_state_sha256']
    word_descriptor = descriptor['word_states']
    boundaries = sample_boundaries(args.capture, snapshot)
    words = list(records(args.capture / word_descriptor['file'], word_descriptor, boundaries))
    word_assessment = assess_words(words)
    prior = json.loads((args.prior_words / 'comparison.json').read_text())
    assert prior['accepted_evidence']
    sealed = json.loads((ROOT / 'analysis/figures/native_original_audio_word_lifecycle_checkpoint.json').read_text())
    assert prior == sealed['word_lifecycle'], 'Prior accepted word evidence changed'
    assert word_descriptor == prior['word_state_descriptor'], 'Actual word trace changed'
    assert {key: prior[key] for key in word_assessment} == word_assessment
    live = descriptor['initial_live_state']
    requested, boundary = audio_state(restored), boundaries[1][0]
    assessment = assess_restore(requested, live, words, boundary)
    rejected = controls(requested, live, words, boundary)
    report = dict(accepted_evidence=True, preserved_execution=validation,
        actual_word_stream_unchanged=True, word_state_descriptor=word_descriptor,
        word_assessment=word_assessment, requested_state=requested, **assessment,
        damaged_actual_snapshot_controls_rejected=rejected,
        scope='Actual original restore assignments and read-only pre-replay state agree with the requested warm save and preserve the first live projected word boundary. Execution between snapshots is not observed. The extra channel-1 word is already encoded in this warm input, not a new cold sample-length or padding rule. Native onset, handoffs and whole PCM acceptance remain open.',
        native_runtime_changed=False, native_waveform_accepted=False)
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print('Four actual restore assignments match; channel-1 extra word already exists in requested warm input; six changed-snapshot controls rejected')


if __name__ == '__main__':
    main()
