"""Compare complete native record-pose calls with sealed original bytes."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRY = 0xc25b66
ENTRIES = (ENTRY, 0xc2651e, 0xc2d970)
CHILDREN = (0xc28e28,0xc2c392,0xc1b27e,0xc25704,0xc13d84,0xc2d408,
            0xc149be,0xc26ebe,0xc17f8c,0xc26322,0xc2b05a,0xc26352)


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
    guard = ((0xc2603c,4,'14290062'),(0xc26040,4,'020200f0'),(0xc26044,4,'0c020000'),
             (0xc26048,2,'6734'),(0xc2604a,4,'14290062'),(0xc2604e,4,'020200f0'),
             (0xc26052,4,'0c020000'),(0xc26056,2,'6612'))
    for pc,length,expected in guard:
        n,_,_,_=decoder.decode(pc)
        raw=b''.join(decoder.word(pc+i).to_bytes(2,'big') for i in range(0,length,2))
        if n!=length or raw.hex()!=expected:
            raise RuntimeError('sealed duplicate class read differs')
    rows, pending = {}, list(ENTRIES)
    while pending:
        pc = pending.pop()
        if pc in CHILDREN or pc in rows:
            continue
        length, opcode, handler, assembly = decoder.decode(pc)
        raw = b''.join(decoder.word(pc+i).to_bytes(2, 'big') for i in range(0, length, 2))
        rows[pc] = {'length': length, 'bytes': raw.hex(), 'assembly': assembly}
        kind = classify(handler)
        target = static_target(decoder, pc, opcode, handler, kind)
        if kind == 'rts':
            continue
        if kind == 'interp':
            raise RuntimeError(f'unexpected exception instruction {pc:06X}')
        if pc == 0xc26056:
            # The repeated class read cannot change after the prior zero-class exit.
            pending.append(target)
            continue
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
    if rows[0xc06c02]['bytes'] != '4e75':
        raise RuntimeError('release fault hook is no longer the sealed RTS')
    directory = ROOT/'build/recomp'
    directory.mkdir(parents=True, exist_ok=True)
    header = '/* Sealed record-pose graph with actual motion-history child; validation only. */\n'
    header += 'static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } scene_source_bytes[]={\n'
    for pc, row in sorted(rows.items()):
        values = ','.join(f'0x{b:02x}' for b in bytes.fromhex(row['bytes']))
        header += f'{{0x{pc:06X},{row["length"]},{{{values}}}}},\n'
    (directory/'native_record_pose_source.h').write_text(header+'};\n')
    exe = build_oracle('native_record_pose_oracle', 'tools/recomp/native_record_pose_oracle.c', default_bash())
    result = subprocess.run([str(exe), str(args.cases)], cwd=ROOT, capture_output=True,
                            text=True, timeout=180)
    (directory/'native_record_pose.log').write_text(result.stdout+result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    visited, reports = set(), []
    for line in result.stdout.splitlines():
        if line.startswith('visited:'):
            visited.update(line.partition(':')[2].split())
        else:
            print(line, flush=True)
            reports.append(line)
    print(f'native record pose: {len(visited)}/{len(rows)} original boundaries')
    if args.cases >= 8192:
        expected = {f'{pc:06X}' for pc in rows}
        if visited != expected:
            raise RuntimeError(f'uncovered boundaries: {sorted(expected-visited)}')
        paths = ['port/native_record_pose.c', 'port/native_record_pose.h',
                 'port/native_record_orientation.c', 'port/native_record_orientation.h',
                 'port/flight.c', 'port/flight.h', 'port/two_angle_matrix.c', 'port/two_angle_matrix.h',
                 'port/native_scene_regions_test_support.h',
                 'port/native_record_pose_contract_test.c', 'port/native_record_pose_test_support.h',
                 'port/native_control_record_update.c', 'port/native_control_record_update.h',
                 'port/native_control_record_update_contract_test.c', 'port/CMakeLists.txt',
                 'port/native_record_update_stage_contract_test.c', 'port/scene_bootstrap_native_contract_test.c',
                 'tools/recomp/native_record_pose_oracle.c', 'tools/recomp/check_native_record_pose.py']
        checkpoint = {
            'status': 'complete_native_record_pose_with_actual_history_inverse_and_release_fault',
            'complete_entries': [f'{pc:06X}' for pc in ENTRIES], 'actual_children': ['C2651E','C2D970','C2E514','C2E6DA','C06C02'],
            'release_fault_hook': {'pc':'C06C02','bytes':'4e75','effect':'return only'},
            'child_contracts': [f'{pc:06X}' for pc in CHILDREN],
            'unreachable_arm_proof': {'guard':guard,'excluded':'C26058-C26068',
                'reason':'unchanged class byte is reloaded after its zero-class exit; second BNE must branch'},
            'cases': args.cases*len(ENTRIES), 'cases_per_entry': args.cases,
            'source_boundaries': len(rows), 'covered_boundaries': len(visited),
            'comparison': 'all Chip/Slow RAM except CPU ABI stack C7FD00..C7FF00; shared native record owners verified independently',
            'native_cpu_dependency': False,
            'limitations': 'motion/matrix/input/collision lower children remain explicit; runtime integration pending',
            'original_state_sha256': seal,
            'original_pc_bytes_sha256': hashlib.sha256(b''.join(
                pc.to_bytes(4, 'big')+bytes.fromhex(row['bytes']) for pc, row in sorted(rows.items()))).hexdigest(),
            'reports': reports,
            'source_sha256': {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
        }
        (ROOT/'analysis/figures/native_record_pose_checkpoint.json').write_text(json.dumps(checkpoint, indent=2)+'\n')


if __name__ == '__main__':
    main()
