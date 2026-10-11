"""Expand a verified Hunk-to-runtime map through consistent 32-bit relocations."""
import argparse
import json
import struct
from collections import deque
from pathlib import Path


def u32(data, offset):
    return struct.unpack_from('>I', data, offset)[0]


def bank_slice(address, size, chip, slow):
    if 0 <= address and address + size <= len(chip):
        return chip[address:address + size], 'chip'
    if 0xc00000 <= address and address + size <= 0xc00000 + len(slow):
        offset = address - 0xc00000
        return slow[offset:offset + size], 'slow'
    return None, None


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--executable', type=Path, required=True)
    parser.add_argument('--inventory', type=Path, required=True)
    parser.add_argument('--chip', type=Path, required=True)
    parser.add_argument('--slow', type=Path, required=True)
    parser.add_argument('--seed', action='append',
                        help='segment=runtime_payload_base, e.g. 36=0xc2f490')
    parser.add_argument('--seed-file', type=Path,
                        help='JSON manifest containing evidence-backed seed entries')
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    inventory = json.loads(args.inventory.read_text())
    disk, chip, slow = args.executable.read_bytes(), args.chip.read_bytes(), args.slow.read_bytes()
    bases = {}
    queue = deque()
    seed_values = list(args.seed or [])
    if args.seed_file:
        seed_values.extend(f"{row['segment']}={row['runtime_payload_base']}"
                           for row in json.loads(args.seed_file.read_text())["seeds"])
    if not seed_values:
        parser.error('supply at least one --seed or --seed-file')
    for seed in seed_values:
        segment, base = seed.split('=', 1)
        bases[int(segment)] = int(base, 0)
        queue.append(int(segment))
    results = {}
    conflicts = []
    while queue:
        index = queue.popleft()
        segment = inventory['segments'][index]
        if segment['kind'] == 'BSS':
            results[index] = {'status': 'bss_unverified', 'runtime_payload_base': bases[index]}
            continue
        raw = disk[segment['payload_file_offset']:segment['payload_file_offset'] + segment['size_bytes']]
        loaded, bank = bank_slice(bases[index], len(raw), chip, slow)
        if loaded is None:
            results[index] = {'status': 'outside_exported_ram', 'runtime_payload_base': bases[index]}
            continue
        relocated = {byte for group in segment['reloc32'] for offset in group['offsets'] for byte in range(offset, offset + 4)}
        mismatches = sum(raw[o] != loaded[o] for o in range(len(raw)) if o not in relocated)
        status = 'verified' if mismatches == 0 else 'mutated_or_unmapped'
        results[index] = {'status': status, 'runtime_payload_base': bases[index], 'bank': bank,
                          'size_bytes': len(raw), 'non_relocated_byte_mismatches': mismatches}
        if status != 'verified':
            continue
        target_values = {}
        for group in segment['reloc32']:
            values = {(u32(loaded, offset) - u32(raw, offset)) & 0xffffffff for offset in group['offsets']}
            if len(values) == 1:
                target_values.setdefault(group['target_segment'], set()).update(values)
        for target, values in target_values.items():
            if len(values) != 1:
                conflicts.append({'source_segment': index, 'target_segment': target,
                                  'candidate_bases': sorted(values)})
                continue
            base = next(iter(values))
            if target in bases:
                if bases[target] != base:
                    conflicts.append({'source_segment': index, 'target_segment': target,
                                      'existing_base': bases[target], 'candidate_base': base})
            else:
                bases[target] = base
                queue.append(target)
    report = {'resolved': {str(index): results.get(index, {'status': 'not_visited', 'runtime_payload_base': base})
                           for index, base in sorted(bases.items())}, 'conflicts': conflicts,
              'verified_count': sum(row['status'] == 'verified' for row in results.values())}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'resolved': len(bases), 'verified': report['verified_count'], 'conflicts': len(conflicts)}, indent=2))


if __name__ == '__main__':
    main()
