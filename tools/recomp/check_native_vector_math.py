"""Compare complete native vector-math calls with sealed original bytes."""
import argparse
import hashlib
import json
import subprocess
from audit_command_dispatch import source_decoder
from check_record_region_probe import ROOT, build_oracle, default_bash
from recomp import classify, static_target

ENTRIES = (0xc2574a, 0xc25754, 0xc1d974)
CHILDREN = ()


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
    header = '/* Sealed vector normalization and magnitude graph; validation only. */\n'
    header += 'static const struct { uint32_t pc; unsigned length; uint8_t bytes[10]; } scene_source_bytes[]={\n'
    for pc, row in sorted(rows.items()):
        values = ','.join(f'0x{b:02x}' for b in bytes.fromhex(row['bytes']))
        header += f'{{0x{pc:06X},{row["length"]},{{{values}}}}},\n'
    (directory/'native_vector_math_source.h').write_text(header+'};\n')
    exe = build_oracle('native_vector_math_oracle', 'tools/recomp/native_vector_math_oracle.c', default_bash())
    result = subprocess.run([str(exe), str(args.cases)], cwd=ROOT, capture_output=True,
                            text=True, timeout=300)
    (directory/'native_vector_math.log').write_text(result.stdout+result.stderr)
    if result.returncode:
        raise RuntimeError(result.stderr or result.stdout)
    visited, reports = set(), []
    for line in result.stdout.splitlines():
        if line.startswith('visited:'):
            visited.update(line.partition(':')[2].split())
        else:
            print(line, flush=True)
            reports.append(line)
    print(f'native vector math: {len(visited)}/{len(rows)} original boundaries')
    if args.cases >= 8192:
        expected = {f'{pc:06X}' for pc in rows}
        if visited != expected:
            raise RuntimeError(f'uncovered boundaries: {sorted(expected-visited)}')
        paths = ['port/native_vector_math.c', 'port/native_vector_math.h',
                 'port/scene_component_magnitude.c', 'port/scene_component_magnitude.h', 'tools/recomp/native_vector_math_fixture.h',
                 'tools/recomp/native_vector_math_oracle.c', 'tools/recomp/check_native_vector_math.py']
        checkpoint = {
            'status': 'complete_native_vector_math_no_child_contracts',
            'complete_entries': [f'{pc:06X}' for pc in ENTRIES], 'actual_children': ['C1D974'],
            'child_contracts': [],
            'cases': args.cases*len(ENTRIES), 'source_boundaries': len(rows), 'covered_boundaries': len(visited),
            'comparison': 'all Chip/Slow RAM except CPU ABI stack C7FD00..C7FF00; carried axis, full magnitude, shared magnitude/normalized owners and completing/non-completing outcomes',
            'native_cpu_dependency': False,
            'limitations': 'original magnitude table/adjacent data must be bound; proven non-completing zero-factor source loops fail explicitly; full native startup/frame graph pending',
            'original_state_sha256': seal,
            'original_pc_bytes_sha256': hashlib.sha256(b''.join(
                pc.to_bytes(4, 'big')+bytes.fromhex(row['bytes']) for pc, row in sorted(rows.items()))).hexdigest(),
            'reports': reports,
            'source_sha256': {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in paths},
        }
        (ROOT/'analysis/figures/native_vector_math_checkpoint.json').write_text(json.dumps(checkpoint, indent=2)+'\n')


if __name__ == '__main__':
    main()
