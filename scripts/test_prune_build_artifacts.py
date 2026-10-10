"""Exercise artifact budgets, protected builds and failure retention."""
import contextlib
import io
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

import prune_build_artifacts as prune

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/native'))
from capture_workspace import CaptureWorkspace, retain_failure


class ArtifactTests(unittest.TestCase):
    def test_budget_removes_small_snapshots_and_old_oracles(self):
        with tempfile.TemporaryDirectory() as directory:
            build = Path(directory) / 'build'
            def create(name, age=0):
                path = build / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(bytes(1 << 20))
                os.utime(path, (age, age))
                return path
            old = create('native-flight/frame.1.before.dat')
            oracle = create('recomp/old_oracle.exe', 1)
            protected = [create(name, 2) for name in (
                'native-cmake/native/Release/fa18_native.exe', 'recomp/obj/game.o',
                'native-cmake/_deps/sdl/data.bin', 'native-flight/source.dat.gz',
                'native-flight/current.dat', 'native-flight/ram-live/frame.dat')]
            with patch.object(prune, 'BUILD_ROOTS', (build,)), patch.object(sys, 'argv', [
                'prune', '--build-dir', str(build), '--max-gib', str(6 / 1024),
                '--keep', str(protected[-2]), '--quiet']):
                self.assertEqual(prune.main(), 0)
            self.assertFalse(old.exists())
            self.assertFalse(oracle.exists())
            self.assertTrue(all(path.exists() for path in protected))

    def test_refuses_foreign_directory_and_dry_run_preserves_files(self):
        with tempfile.TemporaryDirectory() as directory:
            build = Path(directory) / 'build'
            build.mkdir()
            data = build / 'frame.dat'
            data.write_bytes(bytes(1 << 20))
            with patch.object(prune, 'BUILD_ROOTS', (build,)), contextlib.redirect_stderr(io.StringIO()):
                with patch.object(sys, 'argv', ['prune', '--build-dir', directory]):
                    with self.assertRaises(SystemExit) as error:
                        prune.main()
                    self.assertEqual(error.exception.code, 2)
                with patch.object(sys, 'argv', ['prune', '--build-dir', str(build),
                        '--max-gib', '0.0001', '--dry-run', '--quiet']):
                    self.assertEqual(prune.main(), 0)
            self.assertTrue(data.exists())

    def test_active_flight_capture_workspaces_are_protected(self):
        with tempfile.TemporaryDirectory() as directory:
            build = Path(directory) / 'build'
            for name in ('mission-frame-delta-live', 'original-frame-delta-live'):
                path = build / name / 'trace.jsonl'
                path.parent.mkdir(parents=True)
                path.write_bytes(bytes(1 << 20))
                self.assertFalse(prune.is_disposable(path,path.relative_to(build),0))
            ordinary = build / 'native-flight' / 'trace.jsonl'
            ordinary.parent.mkdir()
            ordinary.write_bytes(bytes(1 << 20))
            self.assertTrue(prune.is_disposable(ordinary,ordinary.relative_to(build),0))
            executable = build/'recomp/native_frame_body_oracle.exe'
            executable.parent.mkdir()
            executable.write_bytes(bytes(1 << 20))
            self.assertFalse(prune.is_disposable(executable,executable.relative_to(build),0))
            (build/'mission-frame-delta-live/trace.jsonl').unlink()
            (build/'mission-frame-delta-live').rmdir()
            self.assertTrue(prune.is_disposable(executable,executable.relative_to(build),0))

    def test_only_failed_case_survives_temporary_workspace(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory)
            with self.assertRaises(RuntimeError):
                with CaptureWorkspace(output) as captures:
                    for name in ('frame.1.before.dat', 'frame.1.after.dat', 'frame.10.before.dat'):
                        (captures / name).write_bytes(name.encode())
                    retain_failure(captures / 'frame.1', output)
                    raise RuntimeError('oracle mismatch')
            self.assertFalse(captures.exists())
            self.assertEqual(sorted(path.name for path in (output / 'failure').iterdir()),
                             ['frame.1.after.dat', 'frame.1.before.dat'])
            with CaptureWorkspace(output, keep=True) as captures:
                (captures / 'kept.dat').write_bytes(b'explicit debugging')
            self.assertTrue((output / 'kept.dat').exists())

    def test_linked_directory_is_not_traversed(self):
        with tempfile.TemporaryDirectory() as directory:
            build = Path(directory) / 'build'
            outside = Path(directory) / 'recordings'
            build.mkdir(); outside.mkdir()
            recorded = outside / 'sealed.dat'
            recorded.write_bytes(bytes(1 << 20))
            link = build / 'linked'
            try:
                link.symlink_to(outside, target_is_directory=True)
            except OSError:
                self.skipTest('Creating symlinks is unavailable on this host')
            self.assertEqual(list(prune.files_without_links(build)), [])
            self.assertTrue(recorded.exists())


if __name__ == '__main__':
    unittest.main()
