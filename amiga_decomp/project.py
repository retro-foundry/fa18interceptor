"""Explicit project selection and paths shared by capture/analysis tools."""
from __future__ import annotations

from dataclasses import dataclass
import hashlib
import json
import os
from pathlib import Path
import re

FRAMEWORK_ROOT = Path(__file__).resolve().parents[1]


@dataclass(frozen=True)
class Project:
    manifest: Path
    settings: dict

    @property
    def root(self) -> Path:
        return self.manifest.parent

    def path(self, key: str) -> Path:
        return (self.root / self.settings[key]).resolve()

    @property
    def memory_regions(self) -> list[tuple[str, int, int]]:
        return [(r['name'], r['start'], r['size']) for r in self.settings['memory']]

    def runtime_config(self, source: Path) -> Path:
        """Relocate path values in a derived config; never alter recorded bytes."""
        raw = source.read_bytes()
        text = raw.decode('utf-8')
        lines = []
        for line in text.splitlines(keepends=True):
            key, separator, value = line.partition('=')
            if separator:
                for former in self.settings.get('former_roots', []):
                    prefix = former.rstrip('/\\').replace('\\', '/') + '/'
                    if value.replace('\\', '/').startswith(prefix):
                        value = self.root.as_posix() + '/' + value.replace('\\', '/')[len(prefix):]
                        line = key + separator + value
                        break
            lines.append(line)
        relocated = ''.join(lines).encode('utf-8')
        if relocated == raw:
            return source
        directory = self.root / 'build/runtime-config' / hashlib.sha256(relocated).hexdigest()
        directory.mkdir(parents=True, exist_ok=True)
        target = directory / source.name
        if not target.exists():
            target.write_bytes(relocated)
        elif target.read_bytes() != relocated:
            raise ValueError(f'Runtime configuration hash mismatch: {target}')
        return target


def load_project(selection: str | Path) -> Project:
    candidate = Path(selection)
    if candidate.is_file():
        manifest = candidate.resolve()
    elif re.fullmatch(r'[a-z0-9][a-z0-9-]*', str(selection)):
        manifest = FRAMEWORK_ROOT / 'projects' / str(selection) / 'project.json'
    else:
        raise ValueError('Select a project ID or an existing project.json file')
    settings = json.loads(manifest.read_text(encoding='utf-8'))
    if settings.get('schema') != 1:
        raise ValueError(f'Unsupported project schema in {manifest}')
    if not re.fullmatch(r'[a-z0-9][a-z0-9-]*', settings.get('id', '')):
        raise ValueError(f'Invalid project ID in {manifest}')
    for key in ('title', 'config', 'engine_root', 'replay_engine', 'system_dir'):
        if not isinstance(settings.get(key), str) or not settings[key]:
            raise ValueError(f'Project requires {key}: {manifest}')
    regions = settings.get('memory')
    if not isinstance(regions, list) or not regions:
        raise ValueError('Project requires captured memory regions')
    names = set()
    intervals = []
    for region in regions:
        name, start, size = region['name'], region['start'], region['size']
        if not re.fullmatch(r'[a-z][a-z0-9_]*', name) or name in names:
            raise ValueError(f'Invalid or duplicate memory region: {name}')
        if type(start) is not int or type(size) is not int or not 0 <= start < start + size <= 0x1000000:
            raise ValueError(f'Invalid Amiga memory interval: {region}')
        if any(start < end and previous < start + size for previous, end in intervals):
            raise ValueError(f'Overlapping memory region: {name}')
        names.add(name)
        intervals.append((start, start + size))
    return Project(manifest, settings)


def selected_project() -> Project:
    selection = os.environ.get('AMIGA_DECOMP_PROJECT')
    if selection:
        return load_project(selection)
    for parent in (Path.cwd(), *Path.cwd().parents):
        if (parent / 'project.json').is_file():
            return load_project(parent / 'project.json')
    raise ValueError('Select a project with --project ID or AMIGA_DECOMP_PROJECT; see python -m amiga_decomp --help')
