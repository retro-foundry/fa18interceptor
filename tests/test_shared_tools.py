"""Exercise extracted tools with independent executable and memory layouts."""
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

from amiga_decomp.project import FRAMEWORK_ROOT
from amiga_decomp.raster import compare_frames


class SharedTools(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)

    def project(self, identifier, base):
        root = self.root / identifier
        root.mkdir()
        settings = dict(schema=1, id=identifier, title=identifier,
                        config='private/machine.uae', engine_root='emulator',
                        replay_engine='emulator/debugger.exe', system_dir='roms',
                        executable='program.hunk', hunk_runtime_map='resolved.json',
                        disassembly_synthetic_base=0x600000,
                        memory=[dict(name='ram', start=base, size=256)], python_paths=[])
        (root / 'project.json').write_text(json.dumps(settings))
        # One code Hunk containing RTS, independent of any game's executable.
        (root / 'program.hunk').write_bytes(struct.pack('>10I',
            0x3f3, 0, 1, 0, 0, 1, 0x3e9, 1, 0x4e750000, 0x3f2))
        (root / 'resolved.json').write_text(json.dumps(dict(resolved={
            '0': dict(runtime_payload_base=base, size_bytes=4, status='verified')})))
        return root

    def run_tool(self, root, name, *arguments):
        env = os.environ.copy()
        env.pop('AMIGA_DECOMP_PROJECT', None)
        result = subprocess.run([sys.executable, '-m', 'amiga_decomp', '--project',
                                 str(root / 'project.json'), 'run', name, *arguments],
                                cwd=FRAMEWORK_ROOT, env=env, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        return result.stdout

    def test_hunk_mapping_disassembly_and_ram_diff_use_selected_layout(self):
        for identifier, base in [('first-game', 0x200000), ('second-game', 0x400000)]:
            root = self.project(identifier, base)
            self.run_tool(root, 'parse_hunk', 'program.hunk', '--output', 'inventory.json')
            before = bytearray(256)
            before[:4] = bytes.fromhex('4e750000')
            (root / 'ram.bin').write_bytes(before)
            after = before[:]
            after[31:33] = b'ab'
            (root / 'after.bin').write_bytes(after)
            report = json.loads(self.run_tool(root, 'diff_slow_ram', 'ram.bin', 'after.bin', '--region', 'ram'))
            self.assertEqual(report['changed_bytes'], 2)
            self.assertEqual(report['changed_ranges'][0]['start'], f'${base+31:06X}')
            self.run_tool(root, 'resolve_hunk_runtime', '--executable', 'program.hunk',
                          '--inventory', 'inventory.json', '--ram', 'ram=ram.bin',
                          '--seed', f'0={base}', '--output', 'mapped.json')
            mapped = json.loads((root / 'mapped.json').read_text())
            self.assertEqual(mapped['verified_count'], 1)
            self.assertEqual(mapped['resolved']['0']['runtime_payload_base'], base)
            listing = self.run_tool(root, 'disasm_game', f'{base:X}', '1')
            self.assertIn(f'{base:06X}', listing)
            self.assertIn('rts', listing)

    def test_dynamic_call_graph_excludes_other_memory_regions(self):
        for identifier, base in [('first-game', 0x200000), ('second-game', 0x400000)]:
            root = self.project(identifier, base)
            rows = [dict(pc=base, next_pc=base+16, asm='jsr target', bytes='4eb9', registers=dict(a7=128)),
                    dict(pc=base+16, next_pc=0xf80000, asm='jsr rom', bytes='4eb9', registers=dict(a7=124)),
                    dict(pc=0xf80000, asm='rts', bytes='4e75', registers=dict(a7=120))]
            (root / 'trace.jsonl').write_text(''.join(json.dumps(row)+'\n' for row in rows))
            self.run_tool(root, 'generate_trace_call_graph', '--trace', 'trace.jsonl', '--output', 'graph')
            graph = json.loads((root / 'graph.json').read_text())
            self.assertEqual(graph['edges'], [dict(caller=base, callee=base+16, calls=1)])
            self.assertEqual(graph['address_linked_native_callees'], 0)

    def test_audio_helpers_import_without_game_modules(self):
        root = self.project('independent-game', 0x200000)
        (root / 'scripts').mkdir()
        (root / 'scripts/check_audio_imports.py').write_text(
            'import check_original_audio_clock, check_original_audio_prefix\n'
            'import capture_workspace, original_audio_dma, original_audio_samples\n'
            'from amiga_decomp.audio_state import audio_state\n')
        self.run_tool(root, 'check_audio_imports')

    def test_raster_policy_never_hides_index_changes(self):
        files = [self.root / name for name in ('a.rgb', 'b.rgb', 'a.idx', 'b.idx')]
        files[0].write_bytes(struct.pack('<2H', 0, 0x123))
        files[1].write_bytes(struct.pack('<2H', 0x111, 0x123))
        files[2].write_bytes(b'\0\1')
        files[3].write_bytes(b'\0\1')
        result = compare_frames(*files, [{0, 0x111}], 2)
        self.assertTrue(result['passed'])
        self.assertEqual(result['ignored_fade_pixels'], 1)
        files[3].write_bytes(b'\1\1')
        self.assertFalse(compare_frames(*files, [{0, 0x111}], 2)['passed'])

    def test_audio_provenance_accepts_explicit_larger_chip_ram(self):
        from amiga_decomp.dma import Record
        from scripts.original_audio_dma import fetch_words
        from scripts.original_audio_samples import SAMPLE, sample_records
        records = (Record * 1)()
        row = records[0]
        row.type, row.size, row.reg, row.cf_reg = 4, 2, 0xaa, 0xffff
        row.addr, row.dat = 0x90000, 0x21fd
        with self.assertRaisesRegex(ValueError, 'Chip RAM'):
            fetch_words(records)
        self.assertEqual(int(fetch_words(records, 0x100000)[0]['address']), 0x90000)
        sample = SAMPLE.pack(5120, 0x90000, 0x21fd, 0x21, 8, 0, 0)
        with self.assertRaisesRegex(ValueError, 'provenance'):
            list(sample_records(sample, 10, 11))
        self.assertEqual(list(sample_records(sample, 10, 11, 0x100000))[0][1], 0x90000)


if __name__ == '__main__':
    unittest.main()
