"""Inventory remaining renderer entry adapters, including all 32 writer callbacks."""
import argparse
import json
import re
from audit_command_dispatch import ROOT, audit, source_decoder
from audit_render_leaf_helpers_scope import inventory as leaf_inventory
from recomp import classify, static_target

MANIFEST = ROOT/'analysis/data/render_entry_helpers_scope_inventory.json'

def inventory():
    authority = leaf_inventory()['pixel_writer_dispatch_authority']
    writers = [slot['target'] for table in authority['tables'].values() for slot in table]
    assert len(writers) == len(set(writers)) == 32
    entries = ['C2F688', 'C2F63A', 'C2F64E', 'C301F6', 'C330FE'] + writers
    result = audit(entries, dynamic_targets={0xc2f764: [int(entry,16) for entry in writers]},
                   additional_cold_entries=entries)
    _, decoder = source_decoder()
    incoming = {entry: [] for entry in entries}
    candidates = set()
    for path in (ROOT/'port/recomp/generated').glob('recomp_*.c'):
        candidates.update(int(pc,16) for pc in re.findall(r'/\* ([0-9A-F]{6}): (?:jsr|bsr)\s',path.read_text()))
    for pc in sorted(candidates):
        decoded = decoder.decode(pc)
        if not decoded:
            continue
        length, opcode, handler, assembly = decoded
        kind = classify(handler)
        if kind not in ('jsr', 'bsr'):
            continue
        target = static_target(decoder, pc, opcode, handler, kind)
        if target is None or f'{target:06X}' not in incoming:
            continue
        incoming[f'{target:06X}'].append({'pc': f'{pc:06X}', 'return_pc': f'{pc+length:06X}',
            'instruction': assembly, 'bytes': ''.join(f'{decoder.word(pc+i):04x}' for i in range(0,length,2))})
    for table, slots in authority['tables'].items():
        for slot in slots:
            incoming[slot['target']].append({'kind': 'original writer callback', 'jump_pc': 'C2F764',
                'table': table, **slot, 'native_guard_added': False})
    assert all(incoming.values()), [entry for entry, rows in incoming.items() if not rows]
    registry = (ROOT/'port/game/glue/ports.c').read_text()
    registered = set(re.findall(r'\{0x([0-9A-F]{6}),',registry))
    seeded = set(re.findall(r'\{fa18_fn_([0-9A-F]+),',(ROOT/'port/recomp/generated/recomp_table.c').read_text()))
    result['actual_original_callability'] = incoming
    result['pixel_writer_dispatch_authority'] = authority
    result['writer_semantics'] = [{'entry': slot['target'], 'colour': slot['selector'],
        'rows': 1 if table == 'C2F786' else 2} for table, slots in authority['tables'].items() for slot in slots]
    result['classification'] = {'purpose': 'Complete remaining renderer entries and cold writer callbacks',
        'implementation': False, 'upgraded_registered_entries': [entry for entry in entries if entry in registered],
        'new_seeded_entries': [entry for entry in entries if entry not in registered and entry in seeded],
        'new_source_only_entries': [entry for entry in entries if entry not in registered and entry not in seeded],
        'os_service_replacement': False, 'selector_guard_added': False}
    return result

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write', action='store_true')
    args = parser.parse_args()
    result = inventory()
    if args.write:
        MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text()) != result:
        raise ValueError('Renderer entry source or callability changed')
    print(f"Renderer entry helpers: {len(result['owners'])} owners, {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared source boundaries")
    print(json.dumps(result['classification']))

if __name__ == '__main__':
    main()
