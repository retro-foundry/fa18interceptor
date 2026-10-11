"""Locate an exact runtime byte window in the original Hunk payloads."""
import argparse
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--executable', type=Path, required=True)
    parser.add_argument('--inventory', type=Path, required=True)
    parser.add_argument('--ram', type=Path, required=True)
    parser.add_argument('--ram-base', type=lambda x: int(x, 0), required=True)
    parser.add_argument('--address', type=lambda x: int(x, 0), required=True)
    parser.add_argument('--length', type=int, default=16)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    executable = args.executable.read_bytes()
    ram = args.ram.read_bytes()
    offset = args.address - args.ram_base
    window = ram[offset:offset + args.length]
    if len(window) != args.length:
        raise ValueError('Window falls outside RAM file')
    inventory = json.loads(args.inventory.read_text())
    matches = []
    for segment in inventory['segments']:
        if segment['kind'] == 'BSS':
            continue
        start = segment['payload_file_offset']
        end = start + segment['size_bytes']
        pos = executable.find(window, start, end)
        if pos >= 0:
            matches.append({'segment_index': segment['index'], 'kind': segment['kind'],
                            'file_offset': pos, 'segment_offset': pos - start})
    report = {'runtime_address': f'{args.address:06x}', 'length': args.length,
              'bytes': window.hex(), 'matches': matches}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
