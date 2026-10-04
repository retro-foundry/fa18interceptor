"""Validate cold paths using the same helpers called by complete production owners."""
import argparse
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SEGMENTS = {
    'C30260': {'C30260', 'C30262', 'C30264', 'C30266'},
    'C30274': {'C30274', 'C30276', 'C30278', 'C3027A'},
    'C30290': {'C30290', 'C30292', 'C30294', 'C30296'},
    'C302DE': {'C302DE', 'C302E0', 'C302E2', 'C302E4'},
    'C2FBBA': {'C2FBBA', 'C2FBC0', 'C2FBC2', 'C2FBC4', 'C2FBC6'},
}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cases', type=int, default=16384)
    parser.add_argument('--skip-build', action='store_true')
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error('case count must be positive')
    exe = ROOT / 'build/recomp/render_leaf_helpers_segment_oracle.exe'
    if not args.skip_build:
        subprocess.run([
            'python', 'scripts/build_recomp.py', '--output', str(exe.relative_to(ROOT)),
            '--main', 'tools/recomp/render_leaf_helpers_segment_oracle.c',
            '--replace-source', 'port/recomp/recomp_ports.c=tools/recomp/structural_write_log.c',
            '--replace-source', 'port/machine/machine.c=tools/recomp/render_leaf_helpers_hardware_log.c',
            '--replace-source', 'port/machine/blitter.c=tools/recomp/hud_stream_blitter_state.c',
            '--replace-source', 'port/game/glue/glue_child_call.c=tools/recomp/render_leaf_helpers_contract_children.c',
        ], cwd=ROOT, check=True)
    for entry, owned in SEGMENTS.items():
        result = subprocess.run([str(exe), str(args.cases), entry], cwd=ROOT,
                                capture_output=True, text=True, timeout=600)
        (ROOT / f'build/recomp/render_leaf_helpers_segment_{entry}.log').write_text(result.stdout + result.stderr)
        if result.returncode:
            raise RuntimeError(result.stderr or result.stdout)
        observed = set(result.stdout.split('visited:')[1].splitlines()[0].split())
        if observed != owned:
            raise RuntimeError(f'{entry} incomplete segment coverage: {sorted(owned-observed)}')
        print(result.stdout.splitlines()[0], flush=True)
    print('All five production segments cover their complete source scopes; whole-call coverage is recorded separately.')

if __name__ == '__main__':
    main()
