"""Exercise isolation, explicit selection and byte-preserving relocations."""
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

from amiga_decomp.project import FRAMEWORK_ROOT, load_project, selected_project
from amiga_decomp.snapshots import checksum, relocate_snapshot


class Projects(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)

    def project(self, identifier, start=0):
        root = self.root / identifier
        root.mkdir()
        settings = {'schema': 1, 'id': identifier, 'title': identifier,
                    'config': 'local/game.uae', 'engine_root': 'local/engine',
                    'replay_engine': 'local/engine/debugger.exe', 'system_dir': 'local/system',
                    'memory': [{'name': 'ram', 'start': start, 'size': 4096}]}
        manifest = root / 'project.json'
        manifest.write_text(json.dumps(settings))
        return load_project(manifest)

    def launch(self, project, *arguments):
        environment = os.environ.copy()
        environment.pop('AMIGA_DECOMP_PROJECT', None)
        return subprocess.run([sys.executable, '-m', 'amiga_decomp', '--project', str(project.manifest),
                               *arguments], cwd=FRAMEWORK_ROOT, env=environment,
                              capture_output=True, text=True)

    def test_selection_requires_context_and_reports_missing_project(self):
        with patch.dict(os.environ, {}, clear=True), patch('pathlib.Path.cwd', return_value=self.root):
            with self.assertRaisesRegex(ValueError, 'Select a project'):
                selected_project()
        with self.assertRaises(FileNotFoundError):
            load_project('absent-project')

    def test_launcher_passes_separate_roots_memory_and_child_context(self):
        for project in (self.project('first-game'), self.project('second-game', 0x200000)):
            scripts = project.root / 'scripts'
            scripts.mkdir()
            (scripts / 'context.py').write_text(
                "from amiga_decomp.project import selected_project\n"
                "import json, os, subprocess, sys\n"
                "p=selected_project()\n"
                "subprocess.run([sys.executable, '-m', 'amiga_decomp', 'info'], check=True)\n"
                "print(json.dumps({'cwd': os.getcwd(), 'root': str(p.root), 'memory': p.memory_regions}))\n")
            result = self.launch(project, 'run', 'context')
            self.assertEqual(result.returncode, 0, result.stderr)
            row = json.loads(result.stdout.splitlines()[-1])
            self.assertEqual(Path(row['cwd']), project.root)
            self.assertEqual(Path(row['root']), project.root)
            self.assertEqual(row['memory'][0][1], project.memory_regions[0][1])

    def test_shared_pruner_cannot_delete_other_projects_or_sealed_captures(self):
        first, second = self.project('first-game'), self.project('second-game')
        build = second.root / 'build'
        build.mkdir()
        frame = build / 'frame.bin'
        frame.write_bytes(bytes(4096))
        result = self.launch(first, 'run', 'prune_build_artifacts', '--build-dir', str(build),
                             '--max-gib', '0.000001', '--min-file-mib', '0')
        self.assertNotEqual(result.returncode, 0)
        self.assertTrue(frame.exists())
        capture = second.root / 'captures'
        capture.mkdir()
        result = self.launch(second, 'run', 'prune_build_artifacts', '--build-dir', str(capture))
        self.assertNotEqual(result.returncode, 0)
        result = self.launch(second, 'run', 'prune_build_artifacts', '--max-gib', '0.000001', '--min-file-mib', '0')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertFalse(frame.exists())

    def test_runtime_config_preserves_original_bytes_name_and_machine_options(self):
        project = self.project('relocated-game')
        project.settings['former_roots'] = ['C:/original/project']
        original = project.path('config')
        original.parent.mkdir()
        source = b'floppy0=C:/original/project/local/disk.adf\r\ncpu_cycle_exact=true\r\n'
        original.write_bytes(source)
        runtime = project.runtime_config(original)
        self.assertEqual(original.read_bytes(), source)
        self.assertEqual(runtime.name, original.name)
        self.assertIn((project.root.as_posix() + '/local/disk.adf').encode(), runtime.read_bytes())
        self.assertTrue(runtime.read_bytes().endswith(b'cpu_cycle_exact=true\r\n'))
        self.assertEqual(project.runtime_config(original), runtime)

    def test_memory_regions_reject_overlap_and_path_like_names(self):
        project = self.project('invalid-memory')
        for memory in ([{'name': '../ram', 'start': 0, 'size': 1}],
                       [{'name': 'ram', 'start': 0, 'size': 8}, {'name': 'other', 'start': 4, 'size': 8}],
                       [{'name': 'ram', 'start': 0, 'size': 0}]):
            project.settings['memory'] = memory
            project.manifest.write_text(json.dumps(project.settings))
            with self.assertRaises(ValueError):
                load_project(project.manifest)

    def test_manifest_cannot_designate_sealed_or_external_data_as_build_cache(self):
        project = self.project('invalid-cache')
        for directory in ('captures', 'captures/build', '../other-game/build', str(self.root / 'build')):
            project.settings['artifact_policy'] = {'build_roots': [directory]}
            project.manifest.write_text(json.dumps(project.settings))
            with self.assertRaisesRegex(ValueError, 'project-local build'):
                load_project(project.manifest)

    def test_snapshot_relocation_changes_only_checked_config_identity(self):
        before, after = b'floppy0=C:/old/disk.adf', b'floppy0=C:/new/disk.adf'
        raw = struct.pack('<8sIQQIQ', b'E9KSNAP\0', 8, 13, checksum(before), 6, 1) + bytes(range(256))
        derived = relocate_snapshot(raw, before, after)
        self.assertEqual(derived[:20], raw[:20])
        self.assertEqual(derived[28:], raw[28:])
        self.assertEqual(struct.unpack('<Q', derived[20:28])[0], checksum(after))
        with self.assertRaisesRegex(ValueError, 'checksum'):
            relocate_snapshot(raw, b'wrong config', after)
        with self.assertRaisesRegex(ValueError, 'Truncated'):
            relocate_snapshot(raw[:12], before, after)

    def test_snapshot_validation_accepts_non_a500_memory_and_rejects_changed_code(self):
        project = self.project('different-memory', 0x200000)
        capture = project.root / 'build/trace'
        capture.mkdir(parents=True)
        (capture / 'snapshot.json').write_text(json.dumps({'memory': [
            {'name': 'fast', 'start': 0x200000, 'size': 4, 'file': 'fast.bin'}]}))
        (capture / 'fast.bin').write_bytes(bytes.fromhex('4e714e75'))
        (capture / 'trace.jsonl').write_text(json.dumps(
            {'pc': 0x200000, 'bytes': '4e71', 'asm': 'nop', 'next_pc': 0x200002}) + '\n')
        result = self.launch(project, 'run', 'prepare_ghidra', 'build/trace')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('00200000\t4e71', (capture / 'instructions.tsv').read_text())
        (capture / 'fast.bin').write_bytes(bytes(4))
        result = self.launch(project, 'run', 'prepare_ghidra', 'build/trace')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('Code changed since snapshot', result.stderr)


if __name__ == '__main__':
    unittest.main()
