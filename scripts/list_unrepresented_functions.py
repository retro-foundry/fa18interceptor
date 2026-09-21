"""Rank observed Slow-RAM function entries with no exact source representation."""
import argparse
import collections
import json
from pathlib import Path

import coverage as cov

ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000
SLOW_END = 0xC80000


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--limit', type=int, default=80)
    parser.add_argument('--pcode-root', type=Path, default=ROOT / 'pcode' / 'raw')
    parser.add_argument('--source-dir', type=Path, default=ROOT / 'source_amiga' / 'observed')
    args = parser.parse_args()

    slices, _ = cov.assemble_slices(args.source_dir)
    ranges = [(row['address'], row['address'] + len(row['assembled'])) for row in slices]
    entries = collections.defaultdict(lambda: {
        'occurrences': 0, 'max_body_bytes': 0, 'names': set(), 'exports': set(),
    })
    for path in args.pcode_root.glob('*/functions.json'):
        for row in json.loads(path.read_text()):
            address = row.get('address')
            if not isinstance(address, int) or not SLOW_BASE <= address < SLOW_END:
                continue
            entry = entries[address]
            entry['occurrences'] += 1
            entry['max_body_bytes'] = max(entry['max_body_bytes'], row.get('body_bytes', 0))
            entry['names'].add(row.get('name', ''))
            entry['exports'].add(path.parent.name)

    missing = []
    for address, entry in entries.items():
        if any(start <= address < end for start, end in ranges):
            continue
        missing.append((address, entry))
    missing.sort(key=lambda item: (-item[1]['occurrences'], -item[1]['max_body_bytes'], item[0]))
    for address, entry in missing[:args.limit]:
        name = min(entry['names'])
        print(f'${address:06X} occurrences={entry["occurrences"]} '
              f'max_body_bytes={entry["max_body_bytes"]} exports={len(entry["exports"])} {name}')
    print(f'{len(missing)} unrepresented observed entries')


if __name__ == '__main__':
    main()
