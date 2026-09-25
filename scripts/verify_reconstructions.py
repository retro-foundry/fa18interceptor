"""Assemble every observed source slice, compare it with captured runtime RAM, report coverage.

The byte comparison is the gate: a mismatch fails. Coverage is reported alongside
it so the headline numbers in STATUS.md and analysis/semantics.json cannot drift
from what is actually on disk.
"""
import argparse
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import coverage as cov

ROOT = Path(__file__).resolve().parents[1]


def bank_for(address, chip, slow):
    if 0 <= address < len(chip):
        return chip, address, 'chip'
    slow_base = 0xC00000
    if slow_base <= address < slow_base + len(slow):
        return slow, address - slow_base, 'slow'
    raise ValueError(f'ORG ${address:06X} is outside exported RAM')


def report_coverage(covered, slices):
    """Print the three coverage denominators and return the machine-readable summary."""
    executed, exports = cov.observed_bytes()
    segments = cov.resolved_code_segments()
    data_refs, flow_refs = cov.symbol_references()
    classified = cov.classify_segments(segments, executed, data_refs, flow_refs)

    data_bytes = sum(row['size'] for row in classified if row['verdict'] == 'data')
    unclassified_bytes = sum(row['size'] for row in classified if row['verdict'] == 'unclassified')
    candidate_data_bytes = sum(row['size'] for row in classified
                               if 'candidate_data_evidence' in row)
    all_code = cov.total_code_bytes()
    instruction_denominator = all_code - data_bytes
    resolved_bytes = sum(row['size'] for row in classified)
    estimated_data_bytes = data_bytes + candidate_data_bytes
    estimated_instruction_denominator = all_code - estimated_data_bytes
    estimated_unclassified_bytes = unclassified_bytes - candidate_data_bytes

    # Only slow/chip RAM execution is ours; Kickstart ROM is excluded by the exports.
    runtime_backed = len(covered & executed)
    static_only = len(covered - executed)

    print()
    print(f'trace-observed instructions: {len(executed):,} bytes across {exports} P-code exports')
    print(f'reconstructed:      {len(covered):,} distinct bytes '
          f'({runtime_backed:,} runtime-backed, {static_only:,} static-only)')
    print()
    print('coverage')
    print(f'  of observed-executed code   {runtime_backed / len(executed) * 100:5.1f}%  '
          f'({runtime_backed:,} / {len(executed):,})')
    print(f'  of plausible instructions   {len(covered) / instruction_denominator * 100:5.1f}%  '
          f'({len(covered):,} / {instruction_denominator:,}; CODE minus {data_bytes:,} confirmed data)')
    print(f'  of all CODE hunk bytes      {len(covered) / all_code * 100:5.1f}%  '
          f'({len(covered):,} / {all_code:,})')
    print(f'  captured trace / resolved CODE {len(executed) / resolved_bytes * 100:5.1f}%  '
          f'({len(executed):,} / {resolved_bytes:,}; not a game-logic coverage claim)')
    print(f'  {unclassified_bytes:,} bytes remain unclassified (never executed, never referenced)')
    print('  working scene-data estimate '
          f'{len(covered) / estimated_instruction_denominator * 100:5.1f}%  '
          f'({len(covered):,} / {estimated_instruction_denominator:,}; includes '
          f'{candidate_data_bytes:,} candidate scene bytes, not strict coverage)')

    return {
        'pcode_exports': exports,
        'observed_executed_bytes': len(executed),
        'reconstructed_bytes': len(covered),
        'reconstructed_slices': len(slices),
        'reconstructed_runtime_backed_bytes': runtime_backed,
        'reconstructed_static_only_bytes': static_only,
        'confirmed_data_bytes': data_bytes,
        'unclassified_bytes': unclassified_bytes,
        'resolved_code_bytes': resolved_bytes,
        'all_code_bytes': all_code,
        'coverage_denominator': instruction_denominator,
        'candidate_data_bytes': candidate_data_bytes,
        'estimated_data_bytes': estimated_data_bytes,
        'estimated_instruction_denominator': estimated_instruction_denominator,
        'estimated_unclassified_bytes': estimated_unclassified_bytes,
        'coverage_of_observed_percent': round(runtime_backed / len(executed) * 100, 2),
        'coverage_of_instructions_percent': round(len(covered) / instruction_denominator * 100, 2),
        'game_exercised_percent': round(len(executed) / resolved_bytes * 100, 2),
        'estimated_coverage_of_instructions_percent': round(
            len(covered) / estimated_instruction_denominator * 100, 2),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--snapshot', type=Path,
                        default=ROOT / 'captures' / 'baseline_menu')
    parser.add_argument('--source-dir', type=Path,
                        default=ROOT / 'source_amiga' / 'observed')
    parser.add_argument('--no-coverage', action='store_true',
                        help='byte gate only; skip the coverage scan')
    parser.add_argument('--write-summary', type=Path,
                        default=ROOT / 'analysis' / 'coverage.json',
                        help='machine-readable coverage summary destination')
    args = parser.parse_args()

    chip = (args.snapshot / 'chip.bin').read_bytes()
    slow = (args.snapshot / 'slow.bin').read_bytes()
    slices, covered = cov.assemble_slices(args.source_dir)

    checked = []
    for row in slices:
        actual = row['assembled']
        address = row['address']
        memory, offset, bank = bank_for(address, chip, slow)
        expected = memory[offset:offset + len(actual)]
        if actual != expected:
            for index, (left, right) in enumerate(zip(actual, expected)):
                if left != right:
                    raise AssertionError(
                        f'{row["name"]}: mismatch at ${address + index:06X}: '
                        f'assembled ${left:02X}, runtime ${right:02X}')
            raise AssertionError(f'{row["name"]}: output exceeds mapped runtime bytes')
        checked.append((row['name'], bank, address, len(actual)))

    assembled_total = sum(row[3] for row in checked)
    print(f'verified {len(checked)} source slices, {assembled_total} bytes')
    for name, bank, address, size in checked:
        print(f'{name}: {bank} ${address:06X} +{size}')

    overlaps = cov.overlapping_slices(slices)
    for earlier, later, amount in overlaps:
        print(f'WARNING: {earlier["name"]} (${earlier["address"]:06X}) overlaps '
              f'{later["name"]} (${later["address"]:06X}) by {amount} bytes; '
              f'those bytes are counted once in coverage, twice in the assembled total')

    if args.no_coverage:
        return
    summary = report_coverage(covered, checked)
    summary['assembled_total_bytes'] = assembled_total
    summary['overlapping_slice_pairs'] = [
        {'earlier': earlier['name'], 'later': later['name'], 'bytes': amount}
        for earlier, later, amount in overlaps]
    args.write_summary.write_text(json.dumps(summary, indent=2) + '\n')
    print(f'\nwrote {args.write_summary.relative_to(ROOT)}')


if __name__ == '__main__':
    main()
