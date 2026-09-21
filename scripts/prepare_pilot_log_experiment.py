"""Create a disposable write-enabled ADF and matching UAE config for pilot-log diffs."""
import argparse
import hashlib
import json
import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--name', required=True,
                        help='One directory name below captures/pilot_log_experiments')
    args = parser.parse_args()
    if Path(args.name).name != args.name:
        raise ValueError('--name must be one directory component')
    output = ROOT / 'captures' / 'pilot_log_experiments' / args.name
    if output.exists():
        raise FileExistsError(output)
    source_disk = ROOT / 'local' / 'media' / 'fa18.adf'
    source_config = ROOT / 'local' / 'fa18.uae'
    for path in (source_disk, source_config):
        if not path.is_file():
            raise FileNotFoundError(path)
    output.mkdir(parents=True)
    baseline = output / 'baseline.adf'
    writable = output / 'pilot_log_working.adf'
    shutil.copyfile(source_disk, baseline)
    shutil.copyfile(source_disk, writable)
    lines = source_config.read_text(encoding='ascii').splitlines()
    changed = {'floppy0': writable.as_posix(), 'floppy0writeprotected': 'false',
               'puae_floppy_write_protection': 'disabled'}
    rewritten = []
    seen = set()
    for line in lines:
        key = line.split('=', 1)[0]
        if key in changed:
            rewritten.append(f'{key}={changed[key]}')
            seen.add(key)
        else:
            rewritten.append(line)
    rewritten.extend(f'{key}={value}' for key, value in changed.items() if key not in seen)
    config = output / 'pilot_log.uae'
    config.write_text('\n'.join(rewritten) + '\n', encoding='ascii')
    manifest = {'authority_disk': str(source_disk.relative_to(ROOT)), 'authority_sha256': sha(source_disk),
                'baseline': baseline.name, 'baseline_sha256': sha(baseline),
                'working_disk': writable.name, 'working_sha256': sha(writable),
                'config': config.name, 'write_protection': False,
                'procedure': ['save untouched Rookie state', 'change exactly one pilot/log fact and save',
                              'run compare_pilot_log_adf.py against baseline and working disk']}
    (output / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(json.dumps(manifest, indent=2))


if __name__ == '__main__':
    main()
