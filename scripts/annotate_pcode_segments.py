"""Attach original Hunk segment identities to exported runtime P-code records."""
import argparse
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--pcode', type=Path, required=True)
    parser.add_argument('--inventory', type=Path, required=True)
    parser.add_argument('--resolved', type=Path, required=True)
    args = parser.parse_args()
    inventory = json.loads(args.inventory.read_text())
    resolved = json.loads(args.resolved.read_text())['resolved']
    regions = []
    for index_text, entry in resolved.items():
        base = entry['runtime_payload_base']
        segment = inventory['segments'][int(index_text)]
        if segment['kind'] != 'BSS':
            regions.append((base, base + segment['size_bytes'], int(index_text), entry['status']))
    rows = []
    mapped = 0
    for line in (args.pcode / 'instructions.pcode.jsonl').read_text().splitlines():
        row = json.loads(line)
        address = row['address']
        hit = next((region for region in regions if region[0] <= address < region[1]), None)
        if hit:
            start, _, index, status = hit
            row['hunk_segment'] = index
            row['hunk_offset'] = address - start
            row['hunk_mapping_status'] = status
            mapped += 1
        rows.append(row)
    (args.pcode / 'instructions.segmented.jsonl').write_text(
        ''.join(json.dumps(row, separators=(',', ':')) + '\n' for row in rows))
    summary = {'instructions': len(rows), 'mapped': mapped, 'unmapped': len(rows) - mapped,
               'mapping_source': str(args.resolved)}
    (args.pcode / 'segment_annotation.json').write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps(summary))


if __name__ == '__main__':
    main()
