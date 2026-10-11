"""Shared coverage accounting for observed execution, segment class and source slices.

Coverage is reported against three denominators because no single one is honest:

1. Trace-observed bytes - the union of instructions present in the available
   P-code exports. This is what the project's evidence rules allow a
   behavioural claim about. It is a capture-set metric, not a claim about how
   much of the game's real logic has been exercised.
2. CODE bytes minus confirmed data - plausible instruction bytes in the loaded
   executable.
3. All CODE hunk bytes - the raw AmigaDOS Hunk total, which includes large
   initialised-data regions the linker emitted as CODE. Ghidra's CODE
   classification is intentionally retained as a conservative upper bound;
   neither denominator proves that its remainder is executable game logic.

A segment is only called data on positive evidence: it has never executed in any
P-code export AND every reconstructed reference to it is a data operand, never a
branch or call target. Segments that have simply never run stay `unclassified`;
never having executed is equally consistent with a scenario that was never
captured, which is the distinction this module exists to keep.
"""
import json
import re
import shutil
import subprocess
import tempfile
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

ORG = re.compile(r'^\s*org\s+\$(?P<address>[0-9a-fA-F]+)', re.MULTILINE)
EQUATE = re.compile(r'^\s*(?P<name>[A-Za-z_][\w.]*)\s+equ\s+\$(?P<value>[0-9a-fA-F]{5,6})\b',
                    re.MULTILINE | re.IGNORECASE)
EQUATE_LINE = re.compile(r'^\s*\S+\s+equ\b', re.IGNORECASE)
FLOW_MNEMONIC = re.compile(r'^\s*(jsr|jmp|bsr|bra|b[a-z]{2})\b', re.IGNORECASE)

# A separate working estimate, not a strict data classification. Each hunk is
# byte-stable after relocation across five snapshots, has observed scene-control
# consumers, and has no observed instruction starts. A complete reference audit
# is still required before these bytes leave the strict denominator.
CANDIDATE_DATA_SEGMENTS = {
    41: "analysis/model_geometry_boundaries.md",
    42: "analysis/model_geometry_boundaries.md",
    43: "analysis/model_geometry_boundaries.md",
    44: "analysis/model_geometry_boundaries.md",
}


def observed_bytes(pcode_root=None):
    """Union of every byte of every instruction seen executing in any P-code export."""
    pcode_root = pcode_root or ROOT / 'pcode' / 'raw'
    seen = set()
    exports = sorted(pcode_root.glob('*/instructions.pcode.jsonl'))
    for export in exports:
        with export.open() as handle:
            for line in handle:
                try:
                    row = json.loads(line)
                except json.JSONDecodeError:
                    continue
                address = row['address']
                seen.update(range(address, address + len(row['bytes']) // 2))
    return seen, len(exports)


def resolved_code_segments(analysis_dir=None):
    """Original CODE hunks that have a verified runtime payload address."""
    analysis_dir = analysis_dir or ROOT / 'analysis'
    inventory = {segment['index']: segment
                 for segment in json.loads((analysis_dir / 'hunk_inventory.json').read_text())['segments']}
    resolved = json.loads((analysis_dir / 'hunk_runtime_resolved.json').read_text())['resolved']
    segments = []
    for index, entry in resolved.items():
        index = int(index)
        if inventory[index]['kind'] != 'CODE':
            continue
        segments.append({'segment': index,
                         'base': entry['runtime_payload_base'],
                         'size': entry['size_bytes']})
    return sorted(segments, key=lambda row: row['base'])


def total_code_bytes(analysis_dir=None):
    analysis_dir = analysis_dir or ROOT / 'analysis'
    segments = json.loads((analysis_dir / 'hunk_inventory.json').read_text())['segments']
    return sum(row['size_bytes'] for row in segments if row['kind'] == 'CODE')


def symbol_references(source_dir=None):
    """Count how reconstructed source uses each equated address: as data, or as a target.

    Equates are file-local, so a name is resolved only within the file defining it.
    """
    source_dir = source_dir or ROOT / 'source_amiga' / 'observed'
    data_refs = Counter()
    flow_refs = Counter()
    for source in sorted(source_dir.glob('*.asm')):
        text = source.read_text()
        symbols = {match.group('name').upper(): int(match.group('value'), 16)
                   for match in EQUATE.finditer(text)}
        if not symbols:
            continue
        pattern = re.compile(r'\b(' + '|'.join(re.escape(name) for name in symbols) + r')\b',
                             re.IGNORECASE)
        for line in text.splitlines():
            line = line.split(';')[0]
            if EQUATE_LINE.match(line):
                continue
            target = flow_refs if FLOW_MNEMONIC.match(line) else data_refs
            for match in pattern.finditer(line):
                target[symbols[match.group(1).upper()]] += 1
    return data_refs, flow_refs


def classify_segments(segments, executed, data_refs, flow_refs):
    """Annotate each segment with observed execution, reference counts and a verdict."""
    rows = []
    for segment in segments:
        span = range(segment['base'], segment['base'] + segment['size'])
        row = dict(segment)
        row['executed'] = len(executed.intersection(span))
        row['data_refs'] = sum(count for address, count in data_refs.items() if address in span)
        row['flow_refs'] = sum(count for address, count in flow_refs.items() if address in span)
        if row['executed']:
            row['verdict'] = 'code-executed'
        elif row['flow_refs']:
            row['verdict'] = 'code-branch-target'
        elif row['data_refs']:
            row['verdict'] = 'data'
        else:
            row['verdict'] = 'unclassified'
        if row['verdict'] == 'unclassified' and row['segment'] in CANDIDATE_DATA_SEGMENTS:
            row['candidate_data_evidence'] = CANDIDATE_DATA_SEGMENTS[row['segment']]
        rows.append(row)
    return rows


def assemble_slices(source_dir=None, assembler=None):
    """Assemble every source slice; return per-slice rows and the union of covered bytes."""
    source_dir = source_dir or ROOT / 'source_amiga' / 'observed'
    assembler = assembler or shutil.which('vasmm68k_mot')
    if not assembler:
        raise RuntimeError('vasmm68k_mot is required on PATH')
    sources = sorted(source_dir.glob('*.asm'))
    if not sources:
        raise RuntimeError(f'No source files in {source_dir}')
    rows = []
    with tempfile.TemporaryDirectory(prefix='fa18-coverage-') as tmp:
        for source in sources:
            match = ORG.search(source.read_text())
            if not match:
                raise ValueError(f'Missing ORG in {source}')
            address = int(match.group('address'), 16)
            output = Path(tmp) / (source.stem + '.bin')
            subprocess.run([assembler, '-Fbin', '-o', str(output), str(source)],
                           check=True, capture_output=True, text=True)
            rows.append({'name': source.name, 'address': address,
                         'assembled': output.read_bytes()})
    covered = set()
    for row in rows:
        covered.update(range(row['address'], row['address'] + len(row['assembled'])))
    return rows, covered


def overlapping_slices(rows):
    """Pairs of source slices that claim the same runtime bytes (double-counted coverage)."""
    ordered = sorted(rows, key=lambda row: row['address'])
    overlaps = []
    for earlier, later in zip(ordered, ordered[1:]):
        end = earlier['address'] + len(earlier['assembled'])
        if end > later['address']:
            overlaps.append((earlier, later, end - later['address']))
    return overlaps
