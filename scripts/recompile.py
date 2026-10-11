"""Run the shared static recompiler with explicit game context and safe output."""
import argparse
from pathlib import Path
import subprocess
import sys

from amiga_decomp.project import FRAMEWORK_ROOT, selected_project


def main():
    project = selected_project()
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=['analyze', 'recompile', 'disasm',
                                          'bootblock', 'diskcheck', 'version'])
    parser.add_argument('input', nargs='?', type=Path,
                        help='TOML manifest or disk; defaults to project recomp_manifest for code commands')
    parser.add_argument('--tool', type=Path,
                        default=FRAMEWORK_ROOT / 'build/recomp/bin' /
                        ('arecomp.exe' if sys.platform == 'win32' else 'arecomp'))
    parser.add_argument('-o', '--output', type=Path)
    parser.add_argument('--from', dest='start', help='disasm address or diskcheck sector')
    parser.add_argument('--count', help='disasm instruction count')
    parser.add_argument('--disasm', action='store_true', help='bootblock/diskcheck details')
    args = parser.parse_args()
    if args.output is not None and args.command != 'recompile':
        parser.error('--output applies to recompile')
    tool = args.tool.resolve()
    if not tool.is_file():
        parser.error(f'Recompiler missing: {tool}. Build it from the framework root with '
                     'cmake -S . -B build/recomp -G Ninja and cmake --build build/recomp.')
    command = [str(tool), args.command]
    if args.command != 'version':
        source = args.input
        if source is None and args.command in ('analyze', 'recompile', 'disasm'):
            if 'recomp_manifest' not in project.settings:
                parser.error('Supply a TOML input or configure project recomp_manifest.')
            source = project.path('recomp_manifest')
        if source is None or not source.is_file():
            parser.error(f'Missing input: {source}')
        command.append(str(source.resolve()))
    elif args.input is not None:
        parser.error('version does not take input')
    if args.command == 'recompile':
        output = (args.output or project.root / 'build/recomp').resolve()
        allowed = [project.root / name for name in
                   project.settings.get('artifact_policy', {}).get('build_roots', ['build'])]
        if not output.is_relative_to(project.root) or not any(
                path.resolve().is_relative_to(project.root) and output.is_relative_to(path.resolve())
                for path in allowed):
            parser.error('Generated code must stay in a project-local build root; '
                         'use --output build/<name>.')
        command += ['--output', str(output), '--framework-root', str(FRAMEWORK_ROOT)]
    if args.start is not None:
        if args.command not in ('disasm', 'diskcheck'):
            parser.error('--from applies to disasm/diskcheck')
        command += ['--from', args.start]
    if args.count is not None:
        if args.command != 'disasm':
            parser.error('--count applies to disasm')
        command += ['--count', args.count]
    if args.disasm:
        if args.command not in ('bootblock', 'diskcheck'):
            parser.error('--disasm applies to bootblock/diskcheck')
        command.append('--disasm')
    return subprocess.run(command, cwd=project.root, check=False).returncode


if __name__ == '__main__':
    raise SystemExit(main())
