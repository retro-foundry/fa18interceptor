"""Predict every head-up output bit on sealed independent owner inputs.

The original-checked owner supplies zero/one transfer functions, not pixels
to gameplay. Full actual and spatially mixed pages must satisfy the function.
Recorded message-entry scene pixels separately bind the actual caller path.
"""
import argparse
import gzip
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

from check_gameplay_checkpoint import ROOT
from check_qualification_message_cadence import pages


def digest(data):
    return hashlib.sha256(data).hexdigest()


def apply_transfer(before, operations):
    assert len(operations) == 128000 and len(before) == 8 and all(len(p) == 8000 for p in before)
    return [bytes(z ^ ((z ^ o) & b) for z, o, b in zip(
        operations[p * 8000:(p + 1) * 8000],
        operations[64000 + p * 8000:64000 + (p + 1) * 8000], before[p])) for p in range(8)]


def reject_controls(before, actual, operations):
    flat = b''.join(before)
    rejected = {}
    for name, value in (('lost_set', 255), ('lost_clear', 0)):
        changed = bytearray(operations)
        for i, (z, o, b) in enumerate(zip(operations[:64000], operations[64000:], flat)):
            candidate = z & o & ~b if value else ~z & ~o & b
            bit = candidate & -candidate
            if bit:
                # Replace a real constant write with preservation, including
                # writes hidden by an already equal captured background bit.
                changed[i] &= ~bit
                changed[64000 + i] |= bit
                assert apply_transfer(before, changed) != actual
                rejected[name] = dict(plane=i // 8000, byte=i % 8000, bit=bit)
                break
        assert name in rejected, 'Actual owner did not exercise ' + name
    wrong_role = operations[32000:64000] + operations[:32000] + operations[96000:] + operations[64000:96000]
    assert apply_transfer(before, wrong_role) != actual
    rejected['wrong_page_role'] = True
    assert before != actual
    rejected['omitted_owner'] = True
    return rejected


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('oracle', 'evidence', 'source', 'native', 'source-post', 'native-post', 'out'):
        parser.add_argument('--' + name, type=Path, required=True)
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    evidence_bytes = args.evidence.read_bytes()
    evidence = json.loads(evidence_bytes)
    sealed = {(ROOT / p).resolve(): h for p, h in evidence['evidence_sha256'].items()}
    inputs = {name: getattr(args, name.replace('-', '_')) for name in ('source', 'native', 'source-post', 'native-post')}
    for path in inputs.values():
        assert digest(path.read_bytes()) == sealed[path.resolve()], 'Unsealed actual owner fixture'
    oracle_hash = digest(args.oracle.read_bytes())
    source_paths = ('check_headup_transfer.py', 'native_headup_owner_oracle.c',
                    'native_hud_oracle.c', 'native_model_oracle.c')
    source_hashes = {name: digest((Path(__file__).parent / name).read_bytes()) for name in source_paths}
    outcomes, predicted = {}, {}
    with tempfile.TemporaryDirectory(prefix='ram-headup-transfer-', dir=ROOT / 'build') as temporary:
        work = Path(temporary)
        for role in ('source', 'native'):
            fixture, output, transfer = (work / name for name in ('before.dat', 'after.dat', 'operations.bin'))
            data = gzip.decompress(inputs[role].read_bytes())
            fixture.write_bytes(data)
            result = subprocess.run([str(args.oracle.resolve()), str(fixture), str(output), str(transfer)],
                cwd=ROOT, capture_output=True, text=True, timeout=60)
            (args.out / (role + '.log')).write_text(result.stdout + result.stderr)
            assert result.returncode == 0, result.stdout + result.stderr
            state = json.loads(result.stdout)
            assert state['complete_non_stack_ram_matching'] and state['complete_bit_transfer_matching']
            after, operations = output.read_bytes(), transfer.read_bytes()
            before, actual = pages(data), pages(after)
            predicted[role] = apply_transfer(before, operations)
            assert predicted[role] == actual, 'Complete actual head-up pixels differ'
            observed = pages(gzip.decompress(inputs[role + '-post'].read_bytes()))
            assert all(a[:5120] == b[:5120] for a, b in zip(actual, observed)), 'Actual caller scene writes differ'
            state.update(actual_all_plane_prediction_matching=True, predicted_bytes=64000,
                         complete_original_comparisons=4, recorded_caller_scene_bytes_matching=40960,
                         mutation_rejections=reject_controls(before, actual, operations))
            outcomes[role] = state
            for name, content in (('after.dat', after), ('transfer.bin', operations)):
                (args.out / f'{role}.{name}.gz').write_bytes(gzip.compress(content, mtime=0))
    assert digest(args.oracle.read_bytes()) == oracle_hash
    assert args.evidence.read_bytes() == evidence_bytes
    assert {n: digest((Path(__file__).parent / n).read_bytes()) for n in source_paths} == source_hashes
    report = dict(states=outcomes, scene_difference_bytes_by_plane=[sum(a != b for a, b in zip(x[:5120], y[:5120]))
        for x, y in zip(predicted['source'], predicted['native'])], oracle_sha256=oracle_hash,
        source_sha256=source_hashes, evidence_sha256=digest(evidence_bytes),
        input_encoded_sha256={str(p): digest(p.read_bytes()) for p in inputs.values()},
        scope='Every actual head-up output bit and actual caller scene byte is predicted; real set/clear/role/omission controls reject. No pixel mask, gameplay input substitution or complete independent flight acceptance.')
    (args.out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print('128,000 actual head-up bytes predicted; both actual callers and eight write mutations verified')


if __name__ == '__main__':
    main()
