"""Report contiguous byte ranges changed between two equal-sized ADF images."""
import argparse
import hashlib
import json
from pathlib import Path


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('before', type=Path)
    parser.add_argument('after', type=Path)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    before, after = args.before.read_bytes(), args.after.read_bytes()
    if len(before) != len(after):
        raise ValueError(f'image sizes differ: {len(before)} vs {len(after)}')
    ranges = []
    start = None
    for index, (left, right) in enumerate(zip(before, after)):
        if left != right and start is None:
            start = index
        elif left == right and start is not None:
            ranges.append({'start': start, 'end_exclusive': index, 'length': index - start,
                           'before_hex': before[start:index].hex(), 'after_hex': after[start:index].hex()})
            start = None
    if start is not None:
        ranges.append({'start': start, 'end_exclusive': len(before), 'length': len(before) - start,
                       'before_hex': before[start:].hex(), 'after_hex': after[start:].hex()})
    report = {'before': str(args.before), 'after': str(args.after), 'size': len(before),
              'before_sha256': sha(before), 'after_sha256': sha(after),
              'changed_ranges': ranges, 'changed_bytes': sum(item['length'] for item in ranges)}
    payload = json.dumps(report, indent=2) + '\n'
    if args.output:
        args.output.write_text(payload)
    else:
        print(payload, end='')


if __name__ == '__main__':
    main()
