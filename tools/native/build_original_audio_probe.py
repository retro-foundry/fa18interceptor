"""Build an isolated reference-only sample observer; never replace the core.

Original objects remain untouched. PCM/execution preservation is a separate,
mandatory check before this DLL's telemetry is accepted as reference evidence.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'tools/engine9000-src/ami9000'
BASELINE = ROOT / 'tools/engine9000/e9k-debugger/system'


def sha(path):
    with path.open('rb') as file:
        return hashlib.file_digest(file, 'sha256').hexdigest()


def observer_source(original, probe):
    declaration = 'static void newsample(int nr, sample8_t sample)'
    assert original.count(declaration) == 1
    modified = original.replace(declaration, probe + '\n' + declaration)
    begin = modified.index(declaration)
    start = modified.index('{', begin)
    depth, end = 1, start + 1
    while depth:
        if modified[end] == '{': depth += 1
        if modified[end] == '}': depth -= 1
        end += 1
    modified = modified[:end-1] + '\n\tfa18_audio_probe_sample(nr, sample);\n' + modified[end-1:]
    assert modified.count('cdp->dat2 = cdp->dat;') == 1
    modified = modified.replace('cdp->dat2 = cdp->dat;',
        'cdp->dat2 = cdp->dat;\n\t\tfa18_audio_dat2_addr[nr] = fa18_audio_dat_addr[nr];')
    declaration = 'void AUDxDAT_addr(int nr, uae_u16 v, uaecptr addr)\n{'
    assert modified.count(declaration) == 1
    return modified.replace(declaration, declaration + '\n\tfa18_audio_dat_addr[nr] = addr;')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT / 'build/native-audio/sample-probe-engine')
    parser.add_argument('--rebuild', action='store_true')
    args = parser.parse_args()
    work = args.out.resolve()
    # The only source copy, object and DLL this tool writes belong to its
    # explicit diagnostic output. No Makefile all/copy target is invoked.
    assert work != SOURCE and work != BASELINE and not SOURCE.is_relative_to(work)
    assert not BASELINE.is_relative_to(work)
    assert not work.is_relative_to(SOURCE), 'Observer output must be outside the original source/build'
    work.mkdir(parents=True, exist_ok=True)
    (work / 'system').mkdir(exist_ok=True)
    original = SOURCE / 'sources/src/audio.c'
    probe = ROOT / 'tools/native/original_audio_sample_probe.inc'
    generated = observer_source(original.read_text(), probe.read_text())
    copy, object_file, core = work / 'audio.c', work / 'audio.o', work / 'system/ami9000.dll'
    manifest_path = work / 'build.json'
    prior = json.loads(manifest_path.read_text()) if manifest_path.exists() else None
    reusable = bool(not args.rebuild and prior and copy.exists() and core.exists() and
        copy.read_text() == generated and sha(copy) == prior['probe_source_sha256'] and
        sha(core) == prior['core_sha256'] and sha(original) == prior['source_sha256'])
    if not reusable:
        copy.write_text(generated)
    plan = subprocess.run(['mingw32-make', '-n', 'platform=win', 'CC=gcc', '-W',
        'sources/src/audio.c', 'build/win/puae_libretro.dll'], cwd=SOURCE,
        check=True, capture_output=True, text=True).stdout
    commands = [shlex.split(line) for line in plan.splitlines() if line.startswith('gcc ')]
    assert len(commands) == 2, 'Original build plan changed'
    compile_command, link_command = commands
    compiler = shutil.which('gcc')
    assert compiler
    compile_command[0] = link_command[0] = compiler
    compile_command[compile_command.index('-o')+1] = str(object_file)
    compile_command[-1] = str(copy)
    compile_command.append('-Isources/src')
    link_command[link_command.index('-o')+1] = str(core)
    audio_objects = [arg for arg in link_command if Path(arg).as_posix().endswith('/sources/src/audio.o')]
    assert len(audio_objects) == 1
    link_command = [str(object_file) if arg == audio_objects[0] else arg for arg in link_command]
    dependencies = {arg: sha(SOURCE / arg) for arg in link_command if arg.endswith('.o') and arg != str(object_file)}
    if reusable and 'original_objects_sha256' in prior:
        assert dependencies == prior['original_objects_sha256'], 'Original link objects changed; use --rebuild'
    if not reusable:
        subprocess.run(compile_command, cwd=SOURCE, check=True)
        subprocess.run(link_command, cwd=SOURCE, check=True)
    for file in BASELINE.iterdir():
        if file.is_file() and file.name != 'ami9000.dll' and not (work / 'system' / file.name).exists():
            os.link(file, work / 'system' / file.name)
    manifest = dict(scope='Isolated original audio sample observer. Native gameplay and original reference core unchanged. '
        'Telemetry requires paired PCM/execution preservation before acceptance.',
        source_sha256=sha(original), probe_source_sha256=sha(copy),
        observer_include_sha256=sha(probe), core_sha256=sha(core),
        baseline_core_sha256=sha(BASELINE / 'ami9000.dll'),
        original_objects_sha256=dependencies, observer_object_sha256=sha(object_file),
        source_commit=subprocess.run(['git', 'rev-parse', 'HEAD'], cwd=SOURCE, check=True,
                                     capture_output=True, text=True).stdout.strip(),
        compiler=subprocess.run([compiler, '--version'], check=True, capture_output=True,
                                text=True).stdout.splitlines()[0],
        compile=compile_command, link=link_command, record_bytes=20, ring_records=65536,
        timing_scope='newsample service cycle/beam; ordered consumed bytes. Not independently reconstructed logical mixer time.')
    manifest_path.write_text(json.dumps(manifest, indent=2)+'\n')
    print(f'{"Reused verified" if reusable else "Built isolated"} sample observer: {core}')


if __name__ == '__main__':
    main()
