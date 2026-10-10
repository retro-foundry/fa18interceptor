"""Reject counterfeit escort prefixes using the retained complete original flight."""
import argparse
import copy
import gzip
import json
from pathlib import Path
import shutil
import tempfile

from check_original_mission_recording import digest, verified_prefix
from check_gameplay_checkpoint import ROOT, integer


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--prefix', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    baseline = json.loads((args.prefix / 'report.json').read_text())
    hashes = {p: digest((ROOT / p).read_bytes()) for p in baseline['input_hashes']}
    evidence, _, _ = verified_prefix(args.prefix, hashes)
    results = {}
    with tempfile.TemporaryDirectory(prefix='escort-prefix-guards-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        for name in ('report.json', 'driver.jsonl.gz', 'driver.dat.gz', 'input.fa18in', 'consumed.fa18in'):
            shutil.copy2(args.prefix / name, work / name)
        for case in ('unverified_replay', 'different_replay', 'changed_controls', 'truncated_input',
                     'missing_qualification', 'missing_grade', 'false_menu', 'changed_trace'):
            report = copy.deepcopy(baseline)
            modified = None
            if case == 'unverified_replay':
                report['unmodified_replay_exact'] = False
            elif case == 'different_replay':
                report['unmodified_replay']['trace_sha256'] = '0' * 64
            elif case == 'changed_controls':
                modified = 'consumed.fa18in'
                (work / modified).write_bytes((work / modified).read_bytes() + b'1 0 K 0 1\n')
            elif case == 'truncated_input':
                modified = 'input.fa18in'
                lines = (work / modified).read_text().splitlines()
                lines[-1] = 'end 1 0'
                data = ('\n'.join(lines) + '\n').encode('ascii')
                (work / modified).write_bytes(data)
                report['generated_input_sha256'] = digest(data)
            elif case == 'changed_trace':
                modified = 'driver.jsonl.gz'
                data = gzip.decompress((work / modified).read_bytes())
                (work / modified).write_bytes(gzip.compress(data + b'{}\n', mtime=0))
            else:
                modified = 'driver.dat.gz'
                ram = bytearray(gzip.decompress((work / modified).read_bytes()))
                pilot = integer(ram, 0xC1AB74, 4)
                # Retag the altered RAM and its replay fingerprint too: the
                # live qualification/grade/menu checks must still reject it.
                address, size = ((pilot, 2) if case == 'missing_qualification' else
                                 (pilot + 21, 1) if case == 'missing_grade' else (0xC458A6, 1))
                offset = 0x80000 + address - 0xC00000
                ram[offset:offset + size] = bytes([4] if case == 'false_menu' else size)
                (work / modified).write_bytes(gzip.compress(ram, mtime=0))
                report['driver_final_ram_sha256'] = digest(ram)
                report['unmodified_replay']['final_ram_sha256'] = digest(ram)
            (work / 'report.json').write_text(json.dumps(report))
            try:
                verified_prefix(work, hashes)
            except AssertionError:
                results[case] = True
            else:
                raise AssertionError(f'counterfeit prefix accepted: {case}')
            if modified:
                shutil.copy2(args.prefix / modified, work / modified)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(dict(prefix=evidence, mutation_rejections=results), indent=2) + '\n')
    print(f'Actual complete prefix passes; all {len(results)} counterfeit prefixes rejected')


if __name__ == '__main__':
    main()
