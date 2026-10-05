"""Compare complete native primary/secondary placement with sealed source."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc2374c, 0xc2377e)


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
        if kind in ('interp', 'jsr', 'bsr'):
            raise RuntimeError(f'unexpected child/exception {pc:06X}')
        if pc == 0xc238a6:
            pending.append(pc+length)
        elif pc == 0xc239e0:
            pending.append(target)
        elif kind in ('bcc', 'dbcc'):
            if target is None:
                raise RuntimeError(f'unresolved target {pc:06X}')
            pending.extend((pc+length, target))
        elif kind in ('bra', 'jmp'):
            if target is None:
                raise RuntimeError(f'unresolved transfer {pc:06X}')
            pending.append(target)
        else:
            pending.append(pc+length)
    # Every complete path to the first kind test writes literal 0/1. Seal the
    # entire intervening instruction sequence, not just the branch opcodes.
    # Neither the reset/transform/publication tail nor a child can change kind.
    allowed_writes = {0xc2381c: '137c00010062', 0xc2383c: '137c00000062'}
    for pc, raw in allowed_writes.items():
        if rows[pc]['bytes'] != raw:
            raise RuntimeError('source kind assignment changed')
    for pc, row in rows.items():
        if 0xc2381c <= pc <= 0xc239e0 and '($62,A1)' in row['assembly']:
            if pc not in (*allowed_writes, 0xc2389a, 0xc23980, 0xc239d4):
                raise RuntimeError('intervening kind access needs new proof')
    tests = {0xc2389a:'12290062', 0xc2389e:'020100f0', 0xc238a2:'0c010030',
             0xc238a6:'6762', 0xc239d4:'10290062', 0xc239d8:'020000f0',
             0xc239dc:'0c000030', 0xc239e0:'6642'}
    if any(rows[pc]['bytes'] != raw for pc, raw in tests.items()):
        raise RuntimeError('source kind guard changed')
    directory = ROOT/'build/recomp'
    directory.mkdir(parents=True, exist_ok=True)
    header = '/* Sealed complete placement graph; validation only. */\n'
    header += 'static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } scene_source_bytes[]={\n'
    for pc, row in sorted(rows.items()):
        values = ','.join(f'0x{b:02x}' for b in bytes.fromhex(row['bytes']))
        header += f'{{0x{pc:06X},{row["length"]},{{{values}}}}},\n'
    (directory/'native_record_action_placement_source.h').write_text(header+'};\n')
    exe = build_oracle('native_record_action_placement_oracle',
                       'tools/recomp/native_record_action_placement_oracle.c', default_bash())
    result = subprocess.run([str(exe), str(args.cases)], cwd=ROOT, capture_output=True,
                            text=True, timeout=180)
    (directory/'native_record_action_placement.log').write_text(result.stdout+result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    visited, reports = set(), []
    for line in result.stdout.splitlines():
        if line.startswith('visited:'):
            visited.update(line.partition(':')[2].split())
        else:
            print(line, flush=True)
            reports.append(line)
    print(f'native action placement: {len(visited)}/{len(rows)} original boundaries')
    if args.cases >= 8192:
        expected = {f'{pc:06X}' for pc in rows}
        if visited != expected:
            raise RuntimeError(f'uncovered boundaries: {sorted(expected-visited)}')
        paths = ['port/native_record_action_placement.c', 'port/native_record_action_placement.h',
                 'port/native_record_action_placement_contract_test.c',
                 'port/native_record_action_placement_test_support.h',
                 'port/native_control_record_update.c', 'port/native_control_record_update.h',
                 'port/native_control_record_update_contract_test.c', 'port/CMakeLists.txt',
                 'port/native_record_update_stage_contract_test.c', 'port/scene_bootstrap_native_contract_test.c',
                 'port/scene_bootstrap_native.c',
                 'port/native_scene_placement.c', 'port/native_scene_placement.h',
                 'tools/recomp/native_record_action_placement_oracle.c',
                 'tools/recomp/check_native_record_action_placement.py']
        checkpoint = {
            'status':'complete_native_primary_secondary_placement_no_child_contracts',
            'complete_entries':['C2374C','C2377E'], 'child_contracts':[],
            'unreachable_arm_proof':{'kind_writes':allowed_writes, 'guards':tests,
                'excluded':['C2390A-C2391A','C239E2-C23A1E'],
                'reason':'all complete-entry paths assign kind 0/1; no intervening kind write or child; source bytes sealed',
                'scope':'only C2374C/C2377E; manoeuvre/release can reach the class-30 arms'},
            'cases':args.cases*len(ENTRIES),'cases_per_entry':args.cases,
            'source_boundaries':len(rows),'covered_boundaries':len(visited),
            'comparison':'all Chip/Slow RAM except CPU ABI stack C7FD00..C7FF00; shared native record owners independently verified; carried axis and typed descriptors checked',
            'native_cpu_dependency':False,
            'limitations':'caller binds original assets, live mode counters and explicit adjacent numeric fields beyond the first relocated table operand; record-stream renderer and full native startup/frame graph pending',
            'original_state_sha256':seal,
            'original_pc_bytes_sha256':hashlib.sha256(b''.join(
                pc.to_bytes(4,'big')+bytes.fromhex(row['bytes']) for pc,row in sorted(rows.items()))).hexdigest(),
            'reports':reports,
            'source_sha256':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths}}
        (ROOT/'analysis/figures/native_record_action_placement_checkpoint.json').write_text(json.dumps(checkpoint,indent=2)+'\n')


if __name__ == '__main__':
    main()
