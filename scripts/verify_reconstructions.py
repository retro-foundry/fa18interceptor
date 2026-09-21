"""Assemble every observed source slice and compare it with captured runtime RAM."""
import argparse
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ORG = re.compile(r'^\s*org\s+\$(?P<address>[0-9a-fA-F]+)', re.MULTILINE)


def bank_for(address, chip, slow):
    if 0 <= address < len(chip):
        return chip, address, 'chip'
    slow_base = 0xC00000
    if slow_base <= address < slow_base + len(slow):
        return slow, address - slow_base, 'slow'
    raise ValueError(f'ORG ${address:06X} is outside exported RAM')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--snapshot', type=Path,
                        default=ROOT / 'captures' / 'baseline_menu')
    parser.add_argument('--source-dir', type=Path,
                        default=ROOT / 'source_amiga' / 'observed')
    args = parser.parse_args()
    assembler = shutil.which('vasmm68k_mot')
    if not assembler:
        raise RuntimeError('vasmm68k_mot is required on PATH')
    chip = (args.snapshot / 'chip.bin').read_bytes()
    slow = (args.snapshot / 'slow.bin').read_bytes()
    sources = sorted(args.source_dir.glob('*.asm'))
    if not sources:
        raise RuntimeError(f'No source files in {args.source_dir}')
    checked = []
    with tempfile.TemporaryDirectory(prefix='fa18-verify-') as tmp:
        for source in sources:
            match = ORG.search(source.read_text())
            if not match:
                raise ValueError(f'Missing ORG in {source}')
            address = int(match.group('address'), 16)
            output = Path(tmp) / (source.stem + '.bin')
            subprocess.run([assembler, '-Fbin', '-o', str(output), str(source)],
                           check=True, capture_output=True, text=True)
            actual = output.read_bytes()
            memory, offset, bank = bank_for(address, chip, slow)
            expected = memory[offset:offset + len(actual)]
            if actual != expected:
                for index, (left, right) in enumerate(zip(actual, expected)):
                    if left != right:
                        raise AssertionError(
                            f'{source.name}: mismatch at ${address + index:06X}: '
                            f'assembled ${left:02X}, runtime ${right:02X}')
                raise AssertionError(f'{source.name}: output exceeds mapped runtime bytes')
            checked.append((source.name, bank, address, len(actual)))
    checked.sort(key=lambda row: (row[1], row[2]))
    for previous, current in zip(checked, checked[1:]):
        if previous[1] == current[1] and current[2] < previous[2] + previous[3]:
            raise AssertionError(f'overlapping source slices: {previous[0]} and {current[0]}')
    total = sum(row[3] for row in checked)
    print(f'verified {len(checked)} source slices, {total} bytes')
    for name, bank, address, size in checked:
        print(f'{name}: {bank} ${address:06X} +{size}')


if __name__ == '__main__':
    main()
