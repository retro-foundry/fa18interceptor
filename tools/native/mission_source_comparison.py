"""Compare exported mission boundaries; original execution stays external."""
from pathlib import Path
import subprocess
import sys

from capture_workspace import retain_failure

ROOT = Path(__file__).resolve().parents[2]


def compare_mission_boundaries(prefix, entries, bodies, work):
    """Build original oracles serially, compare, and discard each passing RAM case."""
    writes = 0
    for name, cases in (('mode_entry', entries), ('frame_body', bodies)):
        oracle = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run([sys.executable, 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                        '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        with (work / f'{name}.log').open('w') as log:
            for item in cases:
                kind, index = ('entry', item['entry']) if name == 'mode_entry' else ('body', item['capture'])
                capture = f'{prefix}.{kind}.{index}'
                values = [str(item['tick']), *map(str, item['keys'])] if name == 'mode_entry' else [
                    str(item['before_tick']), str(item['after_tick']), str(item['saved_tick']), capture + '.source.dat']
                if name == 'frame_body' and item['owner_exit']:
                    values.append('owner-exit')
                check = subprocess.run([str(oracle), capture + '.before.dat', capture + '.after.dat', *values],
                                       cwd=ROOT, capture_output=True, text=True, timeout=15)
                log.write(check.stdout + check.stderr)
                log.flush()
                if check.returncode:
                    retain_failure(capture, work)
                    raise RuntimeError(f"{kind} {index}, update {item['iteration']}: {check.stderr or check.stdout}")
                for line in check.stdout.splitlines():
                    if line.startswith('Complete original file owners reached DOS Write '):
                        writes += int(line.split('Write ')[1].split()[0])
                for suffix in ('before', 'after', 'source'):
                    Path(capture + f'.{suffix}.dat').unlink(missing_ok=True)
    return writes
