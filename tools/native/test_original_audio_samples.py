"""Reject missing/corrupt consumed bytes, time units and observer overflow."""
import ctypes as C
import gzip
import hashlib
import io
import json
from pathlib import Path
import tempfile
from types import SimpleNamespace
import unittest

from check_original_audio_samples import frames, sample_records, MAGIC, FRAME, SAMPLE, FOOTER
from original_audio_samples import AudioSamplesWriter, LIVE, live_records
from check_original_audio_capture import sha, reference_file


class SampleObserverContract(unittest.TestCase):
    def test_live_snapshots_require_complete_ordered_restore_and_current_channels(self):
        snapshots = [LIVE.pack(5376, 0x100, 0x80, 358*512, 512, 16, 8, 0x21fd, 0x21fd,
                               index%4, 2, 10, 0, 0, 19, index//4) for index in range(8)]
        data = b''.join(snapshots)
        self.assertEqual(len(live_records(data)), 8)
        for changed in (data[:-1], data+b'x', b''.join(snapshots[4:]+snapshots[:4]),
                        b''.join(snapshots[:4]+[snapshots[0]]+snapshots[5:])):
            with self.assertRaises(ValueError):
                live_records(changed)
        for field, value in ((12, 2), (13, 8)):
            changed = list(LIVE.unpack(snapshots[0]))
            changed[field] = value
            with self.assertRaises(ValueError):
                live_records(LIVE.pack(*changed) + b''.join(snapshots[1:]))

    def records(self):
        return SAMPLE.pack(10*512+256, 0x100, 0x21fd, 33, 8, 19, 277)

    def stream(self):
        return MAGIC+b'\1'+FRAME.pack(1,1,10,11,882)+self.records()+b'\0'+FOOTER.pack(1,1)

    def test_debugger_cycle_units_and_fractional_endpoint(self):
        rows = list(sample_records(self.records(), 10, 10))
        self.assertEqual(rows[0][:6], (5376,0x100,0x21fd,33,0,2))

    def test_changed_byte_or_word_rejected(self):
        for payload in (SAMPLE.pack(5376,0x100,0x21fd,32,8,19,277),
                        SAMPLE.pack(5376,0x100,0x20fd,33,8,19,277)):
            with self.assertRaises(ValueError):
                list(sample_records(payload, 10, 11))

    def test_invalid_state_address_beam_or_cycle_rejected(self):
        row = [5376,0x100,0x21fd,33,8,19,277]
        for field, value in ((0,5119),(0,6144),(1,3),(1,0x80000),(4,4),(5,288),(6,1000)):
            with self.subTest(field=field, value=value):
                altered = row.copy(); altered[field] = value
                with self.assertRaises(ValueError):
                    list(sample_records(SAMPLE.pack(*altered), 10, 11))

    def test_initial_unknown_provenance_is_explicit(self):
        row = next(sample_records(SAMPLE.pack(5376,0xffffffff,0x21fd,33,8,19,277), 10, 11))
        self.assertEqual(row[1], 0xffffffff)

    def test_lost_call_footer_and_truncation_rejected(self):
        stream = self.stream()
        self.assertEqual(len(list(frames(io.BytesIO(stream), 1))), 1)
        for altered in (stream[:-1],stream+b'x',
                        MAGIC+b'\1'+FRAME.pack(2,1,10,11,882)+self.records()+b'\0'+FOOTER.pack(1,1),
                        stream[:-FOOTER.size]+FOOTER.pack(1,0)):
            with self.assertRaises((AssertionError, ValueError)):
                list(frames(io.BytesIO(altered), 1))

    def test_broken_cycle_boundary_rejected(self):
        stream = self.stream()[:-1-FOOTER.size]
        stream += b'\1'+FRAME.pack(2,0,12,13,1764)+b'\0'+FOOTER.pack(2,1)
        with self.assertRaises(AssertionError):
            list(frames(io.BytesIO(stream), 2))

    def writer(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        folder = Path(temporary.name)
        (folder / 'system').mkdir()
        (folder / 'system/ami9000.dll').write_bytes(b'test-only-ABI-provider')
        (folder / 'build.json').write_text(json.dumps(dict(core_sha256=sha(b'test-only-ABI-provider'),
            record_bytes=20,ring_records=65536)))
        storage = C.create_string_buffer(self.records())
        self.storage = storage
        self.count = 1
        self.enabled = []
        def take(pointer):
            C.cast(pointer,C.POINTER(C.c_void_p))[0] = C.addressof(storage)
            return self.count
        functions = {'e9k_debug_audio_probe_enable':self.enabled.append,
                     'e9k_debug_audio_probe_take':take,
                     'e9k_debug_audio_probe_record_size':lambda:20}
        engine = SimpleNamespace(audio_capture_bytes=0,audio_capture_budget=100000,audio_capture_frames=882,
            core=SimpleNamespace(e9k_debug_read_cycle_count=lambda:11),
            bind=lambda name,*args:functions[name])
        output = io.BytesIO()
        writer = AudioSamplesWriter(engine,output,folder)
        writer.previous_cycle = 10
        return writer, engine, output

    def test_observer_overflow_rejected(self):
        writer, engine, output = self.writer()
        self.count = 0xffffffff
        with self.assertRaisesRegex(RuntimeError, 'overflow'):
            writer.boundary(1)
        writer.finish()
        self.assertEqual(self.enabled, [1,0])

    def test_shared_budget_and_cleanup_on_failure(self):
        writer, engine, output = self.writer()
        engine.audio_capture_budget = engine.audio_capture_bytes
        with self.assertRaisesRegex(RuntimeError, 'budget'):
            writer.boundary(1)
        with self.assertRaisesRegex(RuntimeError, 'budget'):
            writer.finish()
        self.assertEqual(self.enabled, [1,0])
        self.assertTrue(output.closed)

    def test_word_observer_cleanup_when_sample_footer_exceeds_budget(self):
        writer, engine, output = self.writer()
        writer.word_file = io.BytesIO()
        disabled = []
        writer.word_enable = disabled.append
        engine.audio_capture_budget = engine.audio_capture_bytes
        with self.assertRaisesRegex(RuntimeError, 'budget'):
            writer.finish()
        self.assertEqual(disabled, [0])
        self.assertTrue(writer.word_file.closed)

    def test_word_observer_cleanup_when_word_footer_exceeds_budget(self):
        writer, engine, output = self.writer()
        writer.word_file = io.BytesIO()
        writer.word_total = 0
        writer.word_digest = hashlib.sha256()
        disabled = []
        writer.word_enable = disabled.append
        engine.audio_capture_budget = engine.audio_capture_bytes+1+FOOTER.size
        with self.assertRaisesRegex(RuntimeError, 'budget'):
            writer.finish()
        self.assertEqual(self.enabled, [1,0])
        self.assertEqual(disabled, [0])
        self.assertTrue(output.closed and writer.word_file.closed)

    def test_omitted_record_bytes_rejected(self):
        with self.assertRaisesRegex(ValueError, 'Truncated'):
            list(sample_records(self.records()[:-1],10,11))

    def test_compressed_reference_is_exact_and_accepts_explicit_gzip(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / 'original.wav'
            data = b'exact recorded bytes, without conversion'
            compressed = Path(str(path)+'.gz')
            compressed.write_bytes(gzip.compress(data,mtime=0))
            for selected in (path, compressed):
                with reference_file(selected) as file:
                    self.assertEqual(file.read(), data)


if __name__ == '__main__':
    unittest.main()
