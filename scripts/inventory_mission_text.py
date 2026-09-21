"""Inventory printable mission/menu text from a captured Slow-RAM snapshot.

The output is evidence for text placement and ordering only.  It deliberately
does not infer message selection, mission state, or game-flow ownership.
"""
import argparse
import json
from pathlib import Path

SLOW_BASE = 0xC00000
DEFAULT_START = 0xC3F000
DEFAULT_END = 0xC41128


def printable_runs(data, start, end, minimum):
    """Return ASCII runs wholly contained in the requested runtime range."""
    offset = start - SLOW_BASE
    limit = end - SLOW_BASE
    runs = []
    while offset < limit:
        if 0x20 <= data[offset] < 0x7F:
            run_start = offset
            while offset < limit and 0x20 <= data[offset] < 0x7F:
                offset += 1
            if offset - run_start >= minimum:
                runs.append({
                    'address': f'${SLOW_BASE + run_start:06X}',
                    'length': offset - run_start,
                    'text': data[run_start:offset].decode('ascii'),
                })
        else:
            offset += 1
    return runs


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--slow', type=Path,
                        default=Path('captures/baseline_menu/slow.bin'))
    parser.add_argument('--start', type=lambda value: int(value, 0),
                        default=DEFAULT_START)
    parser.add_argument('--end', type=lambda value: int(value, 0),
                        default=DEFAULT_END)
    parser.add_argument('--minimum', type=int, default=4)
    parser.add_argument('--output', type=Path,
                        default=Path('analysis/data/mission_text_inventory.json'))
    args = parser.parse_args()
    if not SLOW_BASE <= args.start <= args.end <= SLOW_BASE + 0x80000:
        raise ValueError('requested range is outside the 512 KiB Slow-RAM snapshot')
    if args.minimum < 1:
        raise ValueError('--minimum must be positive')

    data = args.slow.read_bytes()
    if len(data) != 0x80000:
        raise ValueError(f'expected 524288-byte Slow-RAM snapshot, got {len(data)}')
    report = {
        'authority': str(args.slow),
        'runtime_range': f'${args.start:06X}-${args.end - 1:06X}',
        'minimum_printable_run_length': args.minimum,
        'runs': printable_runs(data, args.start, args.end, args.minimum),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(f'wrote {args.output}: {len(report["runs"])} printable runs')


if __name__ == '__main__':
    main()
