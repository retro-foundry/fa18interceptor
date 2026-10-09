"""Reject lost/conflicting audio fetches and damaged compact DMA coverage."""
import io
import gzip
from pathlib import Path
import sys
import tempfile
from types import SimpleNamespace
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'scripts'))
from capture_dma_frame import Record
from original_audio_dma import MAGIC, FRAME, FETCH, FOOTER, fetch_words, AudioDmaWriter
from check_original_audio_dma import frames, open_stream
from check_audio_dma_payloads import payload_spans, word_witness


class AudioDmaContract(unittest.TestCase):
    def records(self):
        rows = (Record * 6)()
        for row in rows:
            row.cf_reg = 0xffff
        for index, channel in ((1, 0), (4, 3)):
            row = rows[index]
            row.type, row.reg, row.extra = 4, 0xaa+16*channel, channel
            row.addr, row.dat, row.size = 0x120+index*2, 0xbe00+index, 2
            row.hpos, row.vpos = index, 0
        return rows

    def stream(self):
        payload = fetch_words(self.records()).tobytes()
        return MAGIC+b'\1'+FRAME.pack(1,1,100,0,288,1000,0,288000,2,882)+payload+b'\0'+FOOTER.pack(1,2)

    def decode(self, stream):
        return list(frames(io.BytesIO(stream), 1, 1))

    def test_exact_words_addresses_and_order(self):
        words = fetch_words(self.records())
        self.assertEqual(list(FETCH.iter_unpack(words.tobytes())),
            [(1,0x122,0xbe01,0xaa,1,0,0), (4,0x128,0xbe04,0xda,4,0,3)])
        self.assertEqual(len(self.decode(self.stream())), 1)

    def test_primary_audio_conflict_rejected(self):
        records = self.records()
        records[1].cf_reg = 0x180
        with self.assertRaisesRegex(ValueError, 'conflict'):
            fetch_words(records)

    def test_conflicting_audio_on_other_owner_rejected(self):
        records = self.records()
        records[2].type, records[2].cf_reg = 1, 0xaa
        with self.assertRaisesRegex(ValueError, 'conflict'):
            fetch_words(records)

    def test_bad_fetch_fields_rejected(self):
        for field, value in (('size',4),('dat',0x10000),('extra',4),('reg',0xba),
                             ('addr',0x80000),('addr',3),('hpos',-1),('vpos',32768)):
            with self.subTest(field=field,value=value):
                records = self.records()
                setattr(records[1],field,value)
                with self.assertRaises(ValueError):
                    fetch_words(records)

    def test_truncation_footer_and_trailing_bytes_rejected(self):
        stream = self.stream()
        for damaged in (stream[:-1],stream[:-FOOTER.size-1],stream+b'x',
                        stream[:-FOOTER.size]+FOOTER.pack(1,1)):
            with self.assertRaises(ValueError):
                self.decode(damaged)

    def test_missing_call_and_invalid_grid_rejected(self):
        stream = self.stream()
        start = len(MAGIC)+1
        for field,value in ((0,2),(1,2),(2,-1),(3,2),(4,287),(5,999),(6,288),(7,0),(8,288001)):
            row = list(FRAME.unpack(stream[start:start+FRAME.size]))
            row[field] = value
            damaged = stream[:start]+FRAME.pack(*row)+stream[start+FRAME.size:]
            with self.subTest(field=field), self.assertRaises(ValueError):
                self.decode(damaged)

    def test_duplicate_slot_invalid_channel_and_beam_rejected(self):
        stream = self.stream()
        start = len(MAGIC)+1+FRAME.size+FETCH.size
        for field,value in ((0,1),(1,1),(3,0xba),(4,5),(5,-1),(6,4)):
            row = list(FETCH.unpack(stream[start:start+FETCH.size]))
            row[field] = value
            damaged = stream[:start]+FETCH.pack(*row)+stream[start+FETCH.size:]
            with self.subTest(field=field), self.assertRaises(ValueError):
                self.decode(damaged)

    def test_missing_source_frame_rejected(self):
        stream = self.stream()
        body = stream[len(MAGIC):-FOOTER.size-1]
        row = list(FRAME.unpack(body[1:1+FRAME.size]))
        row[0]=row[1]=2
        row[2]+=2
        second = b'\1'+FRAME.pack(*row)+body[1+FRAME.size:]
        damaged = MAGIC+body+second+b'\0'+FOOTER.pack(2,4)
        with self.assertRaisesRegex(ValueError,'source frame'):
            list(frames(io.BytesIO(damaged),1,2))

    def test_no_fetches_still_retains_call(self):
        stream = MAGIC+b'\1'+FRAME.pack(1,1,100,0,288,1000,0,288000,0,882)+b'\0'+FOOTER.pack(1,0)
        self.assertEqual(self.decode(stream)[0][1], b'')

    def test_budget_rejects_before_writing(self):
        writer = AudioDmaWriter.__new__(AudioDmaWriter)
        writer.engine = SimpleNamespace(audio_capture_bytes=4,audio_capture_budget=5)
        writer.file = io.BytesIO()
        with self.assertRaisesRegex(RuntimeError,'budget'):
            writer.write(b'xx')
        self.assertEqual(writer.file.getvalue(),b'')
        self.assertEqual(writer.engine.audio_capture_bytes,4)

    def test_compressed_retention_keeps_exact_stream(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)/'audio_dma.bin'
            Path(str(path)+'.gz').write_bytes(gzip.compress(self.stream(),mtime=0))
            with open_stream(path) as file:
                self.assertEqual(file.read(),self.stream())
            with open_stream(path) as file:
                self.assertEqual(len(list(frames(file,1,1))),1)

    def test_fetched_word_is_compared_without_replacement(self):
        spans=[[(100,104,b'\x80\x12\0\0')],[],[],[]]
        self.assertTrue(word_witness(spans,0,100,0x8012))
        self.assertTrue(word_witness(spans,0,102,0))
        with self.assertRaises(AssertionError):
            word_witness(spans,0,100,0x8013)
        self.assertFalse(word_witness(spans,1,100,0x8012))
        self.assertFalse(word_witness(spans,0,104,0))

    def test_changed_native_payload_rejected(self):
        import hashlib
        item=dict(bytes=2,sha256=hashlib.sha256(b'\x80\x12').hexdigest(),
            native_requests=1,native_sample_addresses=[0],original_sample_addresses=[0])
        catalog={'payload_catalog_by_channel':[[item],[],[],[]]}
        payload_spans(catalog,b'\x80\x12',b'\x80\x12')
        with self.assertRaises(AssertionError):
            payload_spans(catalog,b'\x80\x12',b'\x80\x13')


if __name__ == '__main__':
    unittest.main()
