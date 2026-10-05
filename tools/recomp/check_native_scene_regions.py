"""Prove the complete region scheduler/spawner and their actual native leaves."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc28996, 0xc28b16, 0xc28b34, 0xc28f16, 0xc2d954)


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
        if kind in ('jsr', 'bsr', 'bcc', 'dbcc'):
            if target is None:
                raise RuntimeError(f'unresolved target {pc:06X}')
            pending.extend((pc+length, target))
        elif kind in ('bra', 'jmp'):
            if target is None:
                raise RuntimeError(f'unresolved transfer {pc:06X}')
            pending.append(target)
        else:
            pending.append(pc+length)
    directory = ROOT/'build/recomp'
    directory.mkdir(parents=True, exist_ok=True)
    header = '/* Sealed complete region graph; validation only. */\n'
    header += 'static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } scene_source_bytes[]={\n'
    for pc, row in sorted(rows.items()):
        values = ','.join(f'0x{b:02x}' for b in bytes.fromhex(row['bytes']))
        header += f'{{0x{pc:06X},{row["length"]},{{{values}}}}},\n'
    (directory/'native_scene_regions_source.h').write_text(header+'};\n')
    exe = build_oracle('native_scene_regions_oracle',
                       'tools/recomp/native_scene_regions_oracle.c', default_bash())
    result = subprocess.run([str(exe), str(args.cases)], cwd=ROOT, capture_output=True,
                            text=True, timeout=240)
    (directory/'native_scene_regions.log').write_text(result.stdout+result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    visited, reports = set(), []
    for line in result.stdout.splitlines():
        if line.startswith('visited:'):
            visited.update(line.partition(':')[2].split())
        else:
            print(line, flush=True)
            reports.append(line)
    print(f'native regions: {len(visited)}/{len(rows)} original boundaries', flush=True)
    if args.cases >= 8192:
        expected = {f'{pc:06X}' for pc in rows}
        if visited != expected:
            raise RuntimeError(f'uncovered boundaries: {sorted(expected-visited)}')
        paths = ['port/native_scene_regions.c', 'port/native_scene_regions.h',
                 'port/native_record_orientation.c', 'port/native_record_orientation.h',
                 'port/native_scene_regions_contract_test.c', 'port/CMakeLists.txt',
                 'port/native_scene_placement.c', 'port/native_scene_placement.h',
                 'port/native_record_action_placement_contract_test.c',
                 'port/native_control_record_update.c', 'port/native_control_record_update.h',
                 'port/native_control_record_update_contract_test.c',
                 'port/native_scene_regions_test_support.h',
                 'port/native_record_update_stage_contract_test.c',
                 'port/scene_bootstrap_native_contract_test.c',
                 'tools/recomp/native_scene_regions_oracle.c',
                 'tools/recomp/check_native_scene_regions.py']
        checkpoint = {
            'status':'complete_native_scene_regions_no_child_contracts',
            'complete_entries':[f'{pc:06X}' for pc in ENTRIES], 'child_contracts':[],
            'cases':args.cases*len(ENTRIES),'cases_per_entry':args.cases,
            'source_boundaries':len(rows),'covered_boundaries':len(visited),
            'comparison':'all Chip/Slow RAM except CPU ABI stack C7FD00..C7FF00; independent typed record and descriptor owners, carried axis and mutable region index',
            'native_cpu_dependency':False,
            'limitations':'scheduler calls actual regions; original region/parameter loader is verified; numeric third-descriptor operand needs explicit source bindings; dispatch, original startup constructor and full native main remain pending',
            'original_state_sha256':seal,
            'original_pc_bytes_sha256':hashlib.sha256(b''.join(
                pc.to_bytes(4,'big')+bytes.fromhex(row['bytes']) for pc,row in sorted(rows.items()))).hexdigest(),
            'reports':reports,
            'source_sha256':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths}}
        (ROOT/'analysis/figures/native_scene_regions_checkpoint.json').write_text(json.dumps(checkpoint,indent=2)+'\n')


if __name__ == '__main__':
    main()
