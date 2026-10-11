"""Validate a captured trace, then import observed RAM and export Ghidra P-code."""
import argparse
import json
import os
from pathlib import Path
import re
import subprocess
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from amiga_decomp.project import FRAMEWORK_ROOT, selected_project


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('capture', type=Path)
    parser.add_argument('--name', required=True, help='new Ghidra project name')
    parser.add_argument('--headless', type=Path, help='analyzeHeadless executable; otherwise use GHIDRA_HOME')
    args = parser.parse_args()
    if not re.fullmatch(r'[A-Za-z0-9_-]+', args.name):
        parser.error('--name must contain only letters, digits, underscores and hyphens')
    project = selected_project()
    headless = args.headless
    if headless is None:
        home = os.environ.get('GHIDRA_HOME')
        if not home:
            parser.error('Provide --headless or set GHIDRA_HOME to the installed Ghidra directory')
        headless = Path(home) / 'support' / ('analyzeHeadless.bat' if os.name == 'nt' else 'analyzeHeadless')
    if not headless.is_file():
        parser.error(f'Ghidra launcher missing: {headless}')
    capture = args.capture.resolve()
    subprocess.run([sys.executable, str(FRAMEWORK_ROOT / 'scripts/prepare_ghidra.py'), str(capture)], check=True)
    snapshot = json.loads((capture / 'snapshot.json').read_text())
    first = snapshot['memory'][0]
    workspace = project.root / 'runtime_project'
    workspace.mkdir(parents=True, exist_ok=True)
    if (workspace / (args.name + '.gpr')).exists() or (workspace / (args.name + '.rep')).exists():
        raise FileExistsError(f'Ghidra project already exists: {args.name}')
    subprocess.run([str(headless.resolve()), str(workspace), args.name,
                    '-import', str(capture / first['file']), '-loader', 'BinaryLoader',
                    '-loader-baseAddr', f"{first['start']:x}", '-processor', '68000:BE:32:default',
                    '-noanalysis', '-scriptPath', str(FRAMEWORK_ROOT / 'scripts/ghidra'),
                    '-postScript', 'ImportAmiga.java', str(capture),
                    '-postScript', 'ExportPcode.java', str(capture / 'pcode')], check=True)
    if not (capture / 'pcode/summary.json').is_file():
        raise RuntimeError('Ghidra did not export a summary; inspect the headless output')


if __name__ == '__main__':
    main()
