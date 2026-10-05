"""Compare complete native post-flight scheduling with actual publication with sealed source."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc09e06,0xc09e98,0xc09ec4,0xc0a002,0xc0a15c,0xc0a1e0,0xc0a2f0,0xc0a334,0xc0a364,0xc0a12e,0xc0a3ea)



def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases', type=int, default=8192)
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error('cases must be positive')
    state, decoder = source_decoder()
    seal = json.loads((ROOT/'analysis/data/command_dispatch_source_scope.json').read_text())['state_sha256']
    if hashlib.sha256(state).hexdigest() != seal:
        raise RuntimeError('original state seal differs')
    rows, pending = {}, list(ENTRIES)
    while pending:
        pc = pending.pop()
        if pc in rows:
            continue
        length, opcode, handler, assembly = decoder.decode(pc)
        raw = b''.join(decoder.word(pc+i).to_bytes(2, 'big') for i in range(0, length, 2))
        rows[pc] = {'length': length, 'bytes': raw.hex(), 'assembly': assembly}
        kind = classify(handler)
        target = static_target(decoder, pc, opcode, handler, kind)
        if kind == 'rts':
            continue
        if kind == 'interp':
            raise RuntimeError(f'unexpected exception {pc:06X}')
        if pc in (0xc1ba26,0xc1ba96):
            pending.append(target)
        elif pc == 0xc1ba18:
            pending.append(pc+length)
        elif kind in ('bcc','dbcc','jsr','bsr'):
            if target is None:
                raise RuntimeError(f'unresolved target {pc:06X}')
            pending.extend((pc+length,target))
        elif kind in ('bra','jmp'):
            if target is None:
                raise RuntimeError(f'unresolved transfer {pc:06X}')
            pending.append(target)
        else:
            pending.append(pc+length)
    if rows[0xc1b906]['bytes'] != '4207' or rows[0xc1b9cc]['bytes'] != '13c700c457a7' or rows[0xc1ba26]['bytes'] != '6d3c':
        raise RuntimeError('zero-mode proof changed')
    directory = ROOT/'build/recomp'
    directory.mkdir(parents=True, exist_ok=True)
    header = '/* Sealed complete post-flight graph; validation only. */\n'
    header += 'static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } scene_source_bytes[]={\n'
    for pc, row in sorted(rows.items()):
        values = ','.join(f'0x{b:02x}' for b in bytes.fromhex(row['bytes']))
        header += f'{{0x{pc:06X},{row["length"]},{{{values}}}}},\n'
    (directory/'native_postflight_publication_source.h').write_text(header+'};\n')
    exe = build_oracle('native_postflight_publication_oracle',
                       'tools/recomp/native_postflight_publication_oracle.c', default_bash())
    result = subprocess.run([str(exe), str(args.cases)], cwd=ROOT, capture_output=True,
                            text=True, timeout=300)
    (directory/'native_postflight_publication.log').write_text(result.stdout+result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    visited, reports = set(), []
    for line in result.stdout.splitlines():
        if line.startswith('visited:'):
            visited.update(line.partition(':')[2].split())
        else:
            print(line, flush=True)
            reports.append(line)
    print(f'native post-flight: {len(visited)}/{len(rows)} original boundaries')
    if args.cases >= 8192:
        expected = {f'{pc:06X}' for pc in rows}
        if visited != expected:
            raise RuntimeError(f'uncovered boundaries: {sorted(expected-visited)}')
        paths = ['port/native_postflight.c','port/native_postflight.h',
            'port/native_context_publication.c','port/native_context_publication.h',
            'port/view_command_input.c','port/view_command_controls.c','port/command_queue.c',
            'port/native_record_selection.c','port/native_postflight_contract_test.c',
            'tools/recomp/native_context_publication_oracle.c',
            'tools/recomp/native_postflight_publication_oracle.c',
            'tools/recomp/check_native_postflight_publication.py']
        checkpoint = {
            'status':'complete_native_postflight_with_actual_publication_no_child_contracts',
            'complete_entries':[f'{p:06X}' for p in ENTRIES],
            'actual_children':['C230B0','C0A3EA','C0A12E','C1BEE8','C08324','C082B8','C1BA86','C1B906','C1C23C'],
            'child_contracts':[],
            'unreachable_arm_proof':'C1BEE8 sets mode zero for both view routes; unchanged kind test excludes class-30 zero-mode return; no intervening child changes kind/mode (actual zoom/redraw graphs sealed)',
            'cases':args.cases*len(ENTRIES),'cases_per_entry':args.cases,
            'source_boundaries':len(rows),'covered_boundaries':len(visited),
            'comparison':'all Chip/Slow RAM except CPU ABI stack C7FD00..C7FF00; typed records and carried axis independently checked',
            'native_cpu_dependency':False,
            'limitations':'original parameter/state bindings and full native startup/frame integration pending',
            'original_state_sha256':seal,
            'original_pc_bytes_sha256':hashlib.sha256(b''.join(
                pc.to_bytes(4,'big')+bytes.fromhex(row['bytes']) for pc,row in sorted(rows.items()))).hexdigest(),
            'reports':reports,
            'source_sha256':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths}}
        (ROOT/'analysis/figures/native_postflight_publication_checkpoint.json').write_text(json.dumps(checkpoint,indent=2)+'\n')


if __name__ == '__main__':
    main()
