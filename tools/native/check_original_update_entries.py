"""Prove original update-call identities without changing reference execution.

Every legacy trace observation is retained. Only explicit C15DA2 calls establish
new updates; pre-LINK deadline/interrupt resumptions belong to the pending call.
The complete probe must reproduce the original trace, RAM and consumed controls.
"""
import argparse
import copy
import csv
import gzip
import hashlib
import json
import os
from pathlib import Path
import subprocess

from check_gameplay_checkpoint import ROOT
from compare_flight_traces import read_trace


def digest(data):
    return hashlib.sha256(data).hexdigest()


def content(path):
    compressed = path.with_suffix(path.suffix + '.gz')
    return path.read_bytes() if path.exists() else gzip.decompress(compressed.read_bytes())


def update_map(entries, instructions, source):
    assert len(entries) % 2 == 0, 'unfinished entry probe'
    calls, mapping, repeated = [], {}, []
    iteration, previous_after = 0, None
    for before, after in zip(entries[::2], entries[1::2]):
        assert before['kind'] == 'before' and after['kind'] == 'after'
        assert before['pc'] == 'C0EFD4' and int(before['iteration']) == iteration
        assert before['via_call'] == after['via_call']
        assert before['stack_long'] == '00C15DA8'
        if before['via_call'] == '1':
            assert before['ppc'] == 'C15DA2', 'update lacks its real JSR caller'
            calls.append(before)
        else:
            assert before['via_call'] == '0' and calls
            assert previous_after['pc'] == 'C0EFD4' and previous_after['result'] == '2', \
                'resumption did not stop before LINK'
            assert previous_after['stop_pc'] == 'C0EFD4'
            assert previous_after['stop_sp'] == previous_after['sp']
            assert before['sp'] == previous_after['sp'] == calls[-1]['sp']
        observed = int(after['iteration'])
        assert observed in (iteration, iteration + 1)
        if before['via_call'] == '1':
            assert observed == iteration + 1, 'real update call not observed'
        if observed != iteration:
            assert observed in source and source[observed]['frame'] == int(before['frame'])
            mapping[observed] = len(calls)
            if before['via_call'] == '0':
                repeated.append(dict(iteration=observed, update=len(calls),
                    frame=int(before['frame']), previous_entry=previous_after, resumed_entry=before))
        iteration = observed
        previous_after = after
    assert set(mapping) == set(source), 'missing legacy observations'
    links = [r for r in instructions if r['kind'] == 'instruction']
    assert len(links) == len(calls), 'update call count differs from executed LINK count'
    for index, (call, link) in enumerate(zip(calls, links)):
        assert link['source_pc'] == link['pc'] == 'C0EFD4'
        assert int(link['a7'], 16) == int(call['sp'], 16)
        assert int(link['frame']) >= int(call['frame'])
        if index + 1 < len(calls):
            assert int(link['frame']) <= int(calls[index + 1]['frame'])
    return mapping, repeated, len(calls)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--probe', type=Path, default=Path('build/recomp/fa18_update_entry_probe.exe'))
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--assess-existing', action='store_true')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    evidence, out = args.source_evidence, args.out
    original = json.loads((evidence / 'report.json').read_text())
    assert original['unmodified_replay_exact']
    if args.assess_existing and (out / 'report.json').exists():
        retained = json.loads((out / 'report.json').read_text())
        for name, key in (('entries.csv', 'entries_sha256'),
                          ('complete-boundary.csv', 'instruction_trace_sha256'),
                          ('mapping.json', 'mapping_sha256'),
                          ('update-consumed.fa18in', 'remapped_input_sha256')):
            assert digest(content(out / name)) == retained[key], f'retained probe changed: {name}'
        assert digest(args.probe.read_bytes()) == retained['probe_runner_sha256']
        assert digest((ROOT / 'tools/native/original_update_entry_probe.c').read_bytes()) == retained['probe_source_sha256']
    for name, expected in original['input_hashes'].items():
        assert digest((ROOT / name).read_bytes()) == expected
    if not args.assess_existing:
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(args.probe),
            '--replace-source', 'port/recomp/recomp_ports.c=tools/native/original_update_entry_probe.c'],
            cwd=ROOT, check=True)
        env = {k: v for k, v in os.environ.items() if not k.startswith(
            ('FA18_LOOP_', 'FA18_BOUNDARY_', 'FA18_UPDATE_ENTRY_', 'FA18_ORIGINAL_PILOT_'))}
        env.update(FA18_UPDATE_ENTRY_PROBE=str((out / 'entries.csv').resolve()),
            FA18_BOUNDARY_TRACE=str((out / 'complete-boundary.csv').resolve()),
            FA18_BOUNDARY_RANGE='c0efd4-c0efd8', FA18_BOUNDARY_TRACE_MAX_MIB='32',
            FA18_LOOP_TRACE=str((out / 'complete-source.jsonl').resolve()))
        with (out / 'complete-source.log').open('w') as log:
            result = subprocess.run([str(args.probe.resolve()), '--state',
                'captures/native/qual_carrier_success/state.bin', '--rom', 'local/system/kick13.rom',
                '--ports', 'off', '--input', str((evidence / 'input.fa18in').resolve()), '--to-end',
                '--game-input-out', str((out / 'complete-consumed.fa18in').resolve()),
                '--ram-out', str((out / 'complete-source.dat').resolve())],
                cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT, timeout=300)
        assert result.returncode == 0, 'original probe replay failed'
    for name, key in (('complete-source.jsonl', 'driver_trace_sha256'),
                      ('complete-source.dat', 'driver_final_ram_sha256'),
                      ('complete-consumed.fa18in', 'consumed_input_sha256')):
        assert digest(content(out / name)) == original[key], f'probe changed original execution: {name}'
    assert digest((evidence / 'input.fa18in').read_bytes()) == original['generated_input_sha256']
    for name in ('entries.csv', 'complete-boundary.csv', 'complete-source.jsonl', 'complete-source.dat'):
        path = out / name
        data = content(path)
        path.with_suffix(path.suffix + '.gz').write_bytes(gzip.compress(data, mtime=0))
        if path.exists():
            path.unlink()
    _, source = read_trace(out / 'complete-source.jsonl.gz')
    entries = list(csv.DictReader(content(out / 'entries.csv').decode('ascii').splitlines()))
    instructions = list(csv.DictReader(content(out / 'complete-boundary.csv').decode('ascii').splitlines()))
    mapping, repeated, updates = update_map(entries, instructions, source)
    assert repeated, 'recording contains no diagnosed duplicate observations'
    mutations = {}
    for name in ('caller', 'executed_link', 'resume_stack', 'resume_stop'):
        changed = copy.deepcopy(entries)
        links = copy.deepcopy(instructions)
        if name == 'caller':
            changed[0]['ppc'] = 'C15DA8'
        elif name == 'executed_link':
            links.pop(next(i for i, r in enumerate(links) if r['kind'] == 'instruction'))
        else:
            pos = next(i for i, r in enumerate(changed) if r['kind'] == 'before' and
                       int(r['iteration']) == repeated[0]['iteration'] - 1 and r['via_call'] == '0')
            if name == 'resume_stack':
                changed[pos]['sp'] = '00C55054'
            else:
                changed[pos - 1]['stop_pc'] = 'C0EFD8'
        try:
            update_map(changed, links, source)
        except AssertionError:
            mutations[name] = True
        else:
            raise AssertionError(f'{name} invalid evidence was accepted')
    remapped, edges = ['FA18_GAME_INPUT_V1'], 0
    for line in (out / 'complete-consumed.fa18in').read_text().splitlines()[1:]:
        parts = line.split()
        if not parts:
            continue
        if parts[0] == 'end':
            parts[1] = str(updates)
        else:
            parts[0] = str(mapping[int(parts[0])])
            edges += 1
        remapped.append(' '.join(parts))
    data = ('\n'.join(remapped) + '\n').encode('ascii')
    (out / 'update-consumed.fa18in').write_bytes(data)
    (out / 'mapping.json').write_text(json.dumps(mapping) + '\n')
    report = dict(source_trace_sha256=original['driver_trace_sha256'],
        source_final_ram_sha256=original['driver_final_ram_sha256'],
        source_consumed_input_sha256=original['consumed_input_sha256'],
        probe_runner_sha256=digest(args.probe.read_bytes()),
        probe_source_sha256=digest((ROOT / 'tools/native/original_update_entry_probe.c').read_bytes()),
        observations=len(source), real_update_calls=updates, executed_links=updates,
        duplicate_observations=len(repeated), repeats=repeated,
        input_edges_preserved=edges, remapped_input_sha256=digest(data),
        mapping_sha256=digest((out / 'mapping.json').read_bytes()),
        entries_sha256=digest(content(out / 'entries.csv')),
        instruction_trace_sha256=digest(content(out / 'complete-boundary.csv')),
        mutation_rejections=mutations,
        scope='Exact original execution and input retained; explicit JSR/LINK identities map every observation. '
              'No game tick, state or drawing equality is used to decide whether a boundary repeats. '
              'Native replay and complete gameplay/drawing parity remain separate acceptance checks.')
    (out / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({k: report[k] for k in ('observations', 'real_update_calls',
        'duplicate_observations', 'input_edges_preserved', 'mutation_rejections')}))
    subprocess.run(['python', 'scripts/prune_build_artifacts.py', '--quiet'], cwd=ROOT, check=True)


if __name__ == '__main__':
    main()
