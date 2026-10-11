"""Inventory semantic blitter families from an Engine9000 DMA capture."""
from __future__ import annotations

import json
import sys
from collections import defaultdict
from pathlib import Path


def main() -> None:
    if len(sys.argv) != 3:
        raise SystemExit("usage: inventory_dma_families.py CAPTURE OUTPUT")
    records = json.loads(Path(sys.argv[1]).read_text())['selected_records']
    starts = [i for i, record in enumerate(records)
              if record['addr'] == 'DFF058']
    starts.append(len(records))
    families = defaultdict(lambda: {'jobs': [], 'c_words': [], 'd_words': [],
                                    'destinations': []})
    for job in range(len(starts) - 1):
        packet = records[starts[job]:starts[job + 1]]
        registers = {record['addr']: record['dat'] for record in packet
                     if record['addr'].startswith('DFF')}
        key = tuple(registers.get(name) for name in
                    ('DFF040', 'DFF042', 'DFF048', 'DFF054', 'DFF058'))
        c = [record for record in packet
             if record['type'] == 5 and record.get('extra') == 34]
        d = [record for record in packet
             if record['type'] == 5 and record.get('extra') == 35]
        family = families[key]
        family['jobs'].append(job)
        family['c_words'].append(len(c))
        family['d_words'].append(len(d))
        family['destinations'].append(sorted({record['addr'] for record in d}))
    output = []
    for key, family in families.items():
        output.append({
            'blitter': dict(zip(('bltcon0', 'bltcon1', 'amod', 'dmod', 'size'), key)),
            **family,
        })
    Path(sys.argv[2]).write_text(json.dumps(output, indent=2) + '\n')


if __name__ == '__main__':
    main()
