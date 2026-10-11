"""Summarize observed call edges with verified Hunk segment identities."""
import argparse
import json
from collections import Counter
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--trace', type=Path, required=True)
    parser.add_argument('--inventory', type=Path, required=True)
    parser.add_argument('--resolved', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    inventory = json.loads(args.inventory.read_text())
    resolved = json.loads(args.resolved.read_text())['resolved']
    regions = [(entry['runtime_payload_base'], entry['runtime_payload_base'] + inventory['segments'][int(index)]['size_bytes'],
                int(index), entry['status'])
               for index, entry in resolved.items() if inventory['segments'][int(index)]['kind'] != 'BSS']

    def locate(address):
        for start, end, index, status in regions:
            if start <= address < end:
                return {'segment': index, 'offset': address - start, 'status': status}
        return None

    edges = Counter()
    rows = [json.loads(line) for line in args.trace.read_text().splitlines()]
    for row in rows:
        if row['asm'].startswith(('jsr ', 'bsr.')):
            source, target = locate(row['pc']), locate(row['next_pc'])
            edges[(row['pc'], row['next_pc'], json.dumps(source, sort_keys=True), json.dumps(target, sort_keys=True))] += 1
    report = {'edge_count': len(edges), 'edges': [
        {'source_pc': f'{source:06x}', 'target_pc': f'{target:06x}',
         'source': json.loads(source_info), 'target': json.loads(target_info), 'hits': hits}
        for (source, target, source_info, target_info), hits in edges.most_common()]}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'edges': len(edges), 'calls': sum(edges.values())}))


if __name__ == '__main__':
    main()
