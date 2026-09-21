"""Validate traced instruction bytes against a snapshot and prepare a bounded import."""
import argparse
import json
from collections import Counter
from pathlib import Path

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('capture', type=Path)
a = p.parse_args()
manifest = json.loads((a.capture / 'snapshot.json').read_text())
banks = [(b, (a.capture / b['file']).read_bytes()) for b in manifest['memory']]
rows = [json.loads(line) for line in (a.capture / 'trace.jsonl').read_text().splitlines()]
instructions = {}
targets = set()
counts = Counter()
for row in rows:
    pc = row['pc']
    bank = next(((b, data) for b, data in banks if b['start'] <= pc < b['start'] + b['size']), None)
    if bank is None:
        continue
    b, data = bank
    raw = bytes.fromhex(row['bytes'])
    offset = pc - b['start']
    if data[offset:offset + len(raw)] != raw:
        raise ValueError(f'Code changed since snapshot at {pc:08x}; needs a separate code version')
    if pc in instructions and instructions[pc]['bytes'] != row['bytes']:
        raise ValueError(f'Multiple instruction versions at {pc:08x}')
    instructions[pc] = row
    counts[pc] += 1
for row in rows:
    if row['asm'].startswith(('bsr.', 'jsr ')) and row['next_pc'] in instructions:
        targets.add(row['next_pc'])
with (a.capture / 'instructions.tsv').open('w') as f:
    f.write('address\tbytes\tcall_target\thits\n')
    for pc, row in sorted(instructions.items()):
        f.write(f'{pc:08x}\t{row["bytes"]}\t{int(pc in targets)}\t{counts[pc]}\n')
summary = {'trace_instructions': len(rows), 'observed_ram_instruction_starts': len(instructions),
           'observed_ram_instruction_bytes': sum(len(bytes.fromhex(r['bytes'])) for r in instructions.values()),
           'observed_ram_call_targets': len(targets),
           'behavioral_names': 0, 'scope': 'RAM includes operating-system code; ownership not yet separated'}
(a.capture / 'inventory.json').write_text(json.dumps(summary, indent=2) + '\n')
print(json.dumps(summary, indent=2))
