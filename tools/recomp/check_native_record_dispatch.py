"""Compare complete native record-dispatch calls with sealed original bytes."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc23a7e, 0xc23f4a, 0xc24458, 0xc24568, 0xc1b7a6, 0xc23ca6, 0xc1bee8, 0xc23ff8, 0xc1ba86, 0xc1b906, 0xc243f2)
CHILDREN = (0xc2574a, 0xc06c02)


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
        if pc == 0xc1ba26:
            # C1B906 clears D7, C1B9CC publishes zero mode; no intervening queue.
            pending.append(target)
        elif kind in ('jsr', 'bsr', 'bcc', 'dbcc'):
            if target is None:
                raise RuntimeError(f'unresolved target {pc:06X}')
            pending.extend((pc+length, target))
        elif kind in ('bra', 'jmp'):
            if target is None:
                raise RuntimeError(f'unresolved transfer {pc:06X}')
            pending.append(target)
        else:
            pending.append(pc+length)
    if rows[0xc1b906]['bytes'] != '4207' or rows[0xc1b9cc]['bytes'] != '13c700c457a7' or rows[0xc1ba26]['bytes'] != '6d3c':
        raise RuntimeError('zero-mode branch proof changed')
    directory = ROOT/'build/recomp'
    directory.mkdir(parents=True, exist_ok=True)
    header = '/* Sealed record-dispatch graph, actual placement and in-sight children; validation only. */\n'
    header += 'static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } scene_source_bytes[]={\n'
    for pc, row in sorted(rows.items()):
        values = ','.join(f'0x{b:02x}' for b in bytes.fromhex(row['bytes']))
        header += f'{{0x{pc:06X},{row["length"]},{{{values}}}}},\n'
    (directory/'native_record_dispatch_source.h').write_text(header+'};\n')
    exe = build_oracle('native_record_dispatch_oracle', 'tools/recomp/native_record_dispatch_oracle.c', default_bash())
    result = subprocess.run([str(exe), str(args.cases)], cwd=ROOT, capture_output=True,
                            text=True, timeout=300)
    (directory/'native_record_dispatch.log').write_text(result.stdout+result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    visited, reports = set(), []
    for line in result.stdout.splitlines():
        if line.startswith('visited:'):
            visited.update(line.partition(':')[2].split())
        else:
            print(line, flush=True)
            reports.append(line)
    print(f'native record dispatch: {len(visited)}/{len(rows)} original boundaries')
    if args.cases >= 8192:
        expected = {f'{pc:06X}' for pc in rows}
        if visited != expected:
            raise RuntimeError(f'uncovered boundaries: {sorted(expected-visited)}')
        paths = ['port/native_record_dispatch.c', 'port/native_record_dispatch.h',
                 'port/native_record_view.c', 'port/native_record_view.h', 'port/native_record_range.c', 'port/native_record_range.h', 'port/scene_component_magnitude.c', 'port/scene_component_magnitude.h', 'port/native_context_publication.c', 'port/native_context_publication.h', 'port/view_command_input.c', 'port/view_command_input.h', 'port/native_control_record_update.c', 'port/native_control_record_update.h',
                 'port/native_scene_records.c', 'port/command_queue.c', 'port/command_queue.h', 'port/view_command_controls.c', 'tools/recomp/native_record_dispatch_oracle.c',
                 'tools/recomp/check_native_record_dispatch.py']
        checkpoint = {
            'status': 'complete_native_record_dispatch_with_explicit_normalize_fault_children',
            'complete_entries': [f'{pc:06X}' for pc in ENTRIES], 'actual_children': ['C24568', 'C23CA6', 'C091E0', 'C2436A', 'C1D974', 'C1BEE8', 'C1B7A6', 'C1C23C'],
            'child_contracts': ['C2574A (normalize scale/input/output)', 'C06C02 (fault, request mutation)'],
            'cases': args.cases*len(ENTRIES), 'source_boundaries': len(rows), 'covered_boundaries': len(visited),
            'comparison': 'all Chip/Slow RAM except CPU ABI stack C7FD00..C7FF00; carried axis, returned companion identity and decisions; typed record owners verified independently against original output',
            'native_cpu_dependency': False,
            'limitations': 'normalization/fault children remain explicit; caller binds source control/view/status/zone assets and live adjacent owners; only references in the supplied sixteen-record bank resolve; full native startup/frame graph pending',
            'original_state_sha256': seal,
            'original_pc_bytes_sha256': hashlib.sha256(b''.join(
                pc.to_bytes(4, 'big')+bytes.fromhex(row['bytes']) for pc, row in sorted(rows.items()))).hexdigest(),
            'reports': reports,
            'source_sha256': {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
        }
        (ROOT/'analysis/figures/native_record_dispatch_checkpoint.json').write_text(json.dumps(checkpoint, indent=2)+'\n')


if __name__ == '__main__':
    main()
