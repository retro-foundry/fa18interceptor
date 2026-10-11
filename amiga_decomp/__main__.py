"""Launch shared or project-specific tools with an explicit project context."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import sys

from .project import FRAMEWORK_ROOT, load_project, selected_project


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--project', help='project ID or path to project.json')
    commands = parser.add_subparsers(dest='command', required=True)
    commands.add_parser('projects', help='list available projects')
    commands.add_parser('info', help='show selected project and resolved paths')
    run = commands.add_parser('run', help='run a shared or project tool; paths are project-relative')
    run.add_argument('tool', help='script name or relative .py tool path')
    run.add_argument('arguments', nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if args.command == 'projects':
        for manifest in sorted((FRAMEWORK_ROOT / 'projects').glob('*/project.json')):
            project = load_project(manifest)
            print(f"{project.settings['id']}\t{project.settings['title']}")
        return 0
    try:
        project = load_project(args.project) if args.project else selected_project()
        if args.command == 'info':
            print(json.dumps({'manifest': str(project.manifest), 'root': str(project.root),
                              **project.settings}, indent=2))
            return 0
        tool = Path(args.tool)
        if tool.is_absolute() or '..' in tool.parts:
            parser.error('Tool must be a name or a relative path inside the framework/project')
        if tool.suffix != '.py':
            tool = tool.with_suffix('.py')
        relative = Path('scripts') / tool if len(tool.parts) == 1 else tool
        candidates = [root / relative for root in (FRAMEWORK_ROOT, project.root)]
        scripts = [p.resolve() for p in candidates if p.is_file()]
        if len(scripts) != 1:
            parser.error(f'Tool {args.tool!r} must identify exactly one script: {candidates}')
        environment = os.environ.copy()
        environment['AMIGA_DECOMP_PROJECT'] = str(project.manifest)
        paths = [FRAMEWORK_ROOT, FRAMEWORK_ROOT / 'scripts',
                 FRAMEWORK_ROOT / 'tools/amiga/audio']
        paths += [project.root / p for p in project.settings.get('python_paths', ['scripts'])]
        environment['PYTHONPATH'] = os.pathsep.join(map(str, paths))
        if os.environ.get('PYTHONPATH'):
            environment['PYTHONPATH'] += os.pathsep + os.environ['PYTHONPATH']
        return subprocess.run([sys.executable, str(scripts[0]), *args.arguments],
                              cwd=project.root, env=environment, check=False).returncode
    except (ValueError, FileNotFoundError, KeyError) as error:
        parser.error(str(error))


if __name__ == '__main__':
    raise SystemExit(main())
