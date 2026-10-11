"""Verify imported fixtures and project-scoped recompilation boundaries."""
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import struct
import sys
import tempfile
import unittest

from amiga_decomp.project import FRAMEWORK_ROOT

ROOT = FRAMEWORK_ROOT
TOOL = ROOT / 'build/recomp/bin' / ('arecomp.exe' if sys.platform == 'win32' else 'arecomp')
spec = importlib.util.spec_from_file_location('converge', ROOT / 'tools/converge.py')
converge = importlib.util.module_from_spec(spec)
spec.loader.exec_module(converge)


class RecompTools(unittest.TestCase):
    def test_synthetic_assembly_matches_all_preserved_fixture_bytes(self):
        fixtures = ROOT / 'tests/recomp/testroms/synthetic'
        with tempfile.TemporaryDirectory() as folder:
            for source in fixtures.glob('*.s'):
                output = Path(folder) / source.with_suffix('.bin').name
                result = subprocess.run([sys.executable, str(ROOT / 'tools/asm68k.py'),
                                         str(source), '-o', str(output)], capture_output=True, text=True)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(output.read_bytes(), source.with_suffix('.bin').read_bytes(), source.name)

    def test_convergence_uses_guest_bank_base_and_only_code_entries(self):
        self.assertTrue(converge.looks_like_code(bytes([1]) * 32, 0xC00010, base=0xC00000))
        self.assertFalse(converge.looks_like_code(bytes([1]) * 32, 0xBFFFFF, base=0xC00000))
        self.assertFalse(converge.looks_like_code(bytes(32), 0xC00000, base=0xC00000))
        text = '[other]\nentry_points = [0xdead]\n[code]\n# entry_points = [0xbad]\nentry_points = [16, $20, 0x30] # existing\n[[code.range]]\nstart=0x10\nend=0x40\n'
        entries, match = converge.existing_entry_points(text)
        self.assertEqual(entries, {16, 32, 48})
        self.assertNotIn('dead', match.group())

    def test_convergence_writes_a_derived_manifest_without_duplicate_code_table(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            manifest, derived = root / 'game.toml', root / 'candidates.toml'
            original = '[input]\npath="image.bin"\n[code]\n# no entries yet\n[[code.range]]\nstart=0x1000\nend=0x1040\n'
            manifest.write_text(original)
            log, snapshot = root / 'run.log', root / 'ram.bin'
            log.write_text('no compiled block at 00001010\nno compiled block at 00002000\n')
            snapshot.write_bytes(bytes([1]) * 64)
            result = subprocess.run([sys.executable, str(ROOT / 'tools/converge.py'),
                                     str(manifest), str(log), '--snapshot', str(snapshot),
                                     '--snapshot-base', '0x1000', '--output', str(derived)],
                                    capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(manifest.read_text(), original)
            text = derived.read_text()
            self.assertEqual(text.count('[code]'), 1)
            self.assertEqual(converge.existing_entry_points(text)[0], {0x1010})

    @unittest.skipUnless(TOOL.is_file(), 'Build build/recomp before native integration checks')
    def test_two_projects_generate_against_shared_runtime_and_reject_foreign_output(self):
        with tempfile.TemporaryDirectory() as folder:
            roots = []
            for index, base in enumerate((0x1000, 0x2000)):
                root = Path(folder) / f'game-{index}'
                root.mkdir()
                roots.append(root)
                project = {'schema': 1, 'id': root.name, 'title': root.name,
                           'config': 'local/game.uae', 'engine_root': 'local/engine',
                           'replay_engine': 'local/engine/debugger', 'system_dir': 'local/system',
                           'memory': [{'name': 'chip', 'start': 0, 'size': 0x80000}],
                           'recomp_manifest': 'game.toml'}
                (root / 'project.json').write_text(json.dumps(project))
                (root / 'image.bin').write_bytes(bytes.fromhex('4e714e75'))
                (root / 'game.toml').write_text(
                    f'[game]\nid="{root.name}"\n[input]\npath="image.bin"\n'
                    f'load_address={base}\n[cpu]\nentry={base}\n'
                    f'[[code.range]]\nstart={base}\nend={base + 4}\n')
                result = self.launch(root, 'recompile')
                self.assertEqual(result.returncode, 0, result.stderr)
                cmake = (root / 'build/recomp/CMakeLists.txt').read_text()
                self.assertIn(ROOT.as_posix(), cmake)
                port = (root / 'build/recomp/port.hpp').read_text()
                self.assertIn((root / 'image.bin').as_posix(), port)
                direct = subprocess.run([str(TOOL), 'recompile', 'game.toml',
                                         '-o', 'build/direct'], cwd=root, capture_output=True, text=True)
                self.assertEqual(direct.returncode, 0, direct.stderr)
                direct_port = (root / 'build/direct/port.hpp').read_text()
                self.assertIn((root / 'image.bin').as_posix(), direct_port)
                self.assertIn(ROOT.as_posix(), (root / 'build/direct/CMakeLists.txt').read_text())
                generated = (root / 'build/recomp/generated/blocks_000.cpp').read_text()
                self.assertIn(f'block_{base:08x}', generated)
            result = self.launch(roots[0], 'recompile', '--output', str(roots[1] / 'build/foreign'))
            self.assertNotEqual(result.returncode, 0)
            self.assertIn('project-local build root', result.stderr)
            self.assertFalse((roots[1] / 'build/foreign').exists())

    def launch(self, root, *args):
        environment = os.environ.copy()
        environment.pop('AMIGA_DECOMP_PROJECT', None)
        return subprocess.run([sys.executable, '-m', 'amiga_decomp', '--project',
                               str(root / 'project.json'), 'run', 'recompile', *args],
                              cwd=ROOT, env=environment, capture_output=True, text=True)

    @unittest.skipUnless(TOOL.is_file(), 'Build build/recomp before native integration checks')
    def test_hunk_analysis_does_not_emit_a_port_with_an_incompatible_loader(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            words = [0x3F3, 0, 1, 0, 0, 1, 0x3E9, 1, 0x4E714E75, 0x3F2]
            (root / 'game.hunk').write_bytes(struct.pack('>' + 'I' * len(words), *words))
            manifest = root / 'game.toml'
            manifest.write_text('[input]\ntype="hunk"\npath="game.hunk"\n'
                                'load_address=0x10000\n[cpu]\nentry=0x10000\n')
            analyzed = subprocess.run([str(TOOL), 'analyze', str(manifest)], capture_output=True, text=True)
            self.assertEqual(analyzed.returncode, 0, analyzed.stderr)
            emitted = subprocess.run([str(TOOL), 'recompile', str(manifest), '-o', str(root / 'port')],
                                     capture_output=True, text=True)
            self.assertNotEqual(emitted.returncode, 0)
            self.assertIn('generated Hunk loading/relocation', emitted.stderr)
            self.assertFalse((root / 'port').exists())


if __name__ == '__main__':
    unittest.main()
