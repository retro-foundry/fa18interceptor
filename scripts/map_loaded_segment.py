"""Validate one loaded Hunk segment and infer bases of its relocation targets."""
import argparse
import json
import struct
from collections import defaultdict
from pathlib import Path


def u32(data, offset):
    return struct.unpack_from('>I', data, offset)[0]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--executable', type=Path, required=True)
    parser.add_argument('--inventory', type=Path, required=True)
    parser.add_argument('--ram', type=Path, required=True)
    parser.add_argument('--ram-base', type=lambda x: int(x, 0), required=True)
    parser.add_argument('--segment', type=int, required=True)
    parser.add_argument('--runtime-base', type=lambda x: int(x, 0), required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    inventory = json.loads(args.inventory.read_text())
    segment = inventory['segments'][args.segment]
    if segment['kind'] == 'BSS':
        raise ValueError('BSS has no disk payload')
    disk = args.executable.read_bytes()
    ram = args.ram.read_bytes()
    raw = disk[segment['payload_file_offset']:segment['payload_file_offset'] + segment['size_bytes']]
    start = args.runtime_base - args.ram_base
    loaded = ram[start:start + len(raw)]
    if len(loaded) != len(raw):
        raise ValueError('Loaded segment falls outside supplied RAM bank')
    reloc_offsets = {offset for group in segment['reloc32'] for offset in group['offsets']}
    unchanged = [offset for offset in range(len(raw)) if not any(r <= offset < r + 4 for r in reloc_offsets)]
    mismatch = sum(raw[offset] != loaded[offset] for offset in unchanged)
    bases = defaultdict(list)
    for group in segment['reloc32']:
        for offset in group['offsets']:
            bases[group['target_segment']].append((u32(loaded, offset) - u32(raw, offset)) & 0xffffffff)
    target_bases = {str(target): {'candidates': sorted(set(values)), 'consistent': len(set(values)) == 1,
                                  'references': len(values)} for target, values in bases.items()}
    report = {'segment': args.segment, 'kind': segment['kind'], 'runtime_base': f'{args.runtime_base:06x}',
              'size_bytes': len(raw), 'non_relocated_byte_mismatches': mismatch,
              'reloc32_count': len(reloc_offsets), 'target_bases': target_bases}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
