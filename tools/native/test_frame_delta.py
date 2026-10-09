"""Corrupt or incomplete compact captures must fail before oracle acceptance."""
import hashlib
from pathlib import Path
import struct
import tempfile
import unittest

from frame_delta import MAGIC, RAM_SIZE, bodies, snapshots


def word(value):
    return struct.pack('<I', value)


def fixture():
    state = bytearray(RAM_SIZE)
    stream = bytearray(MAGIC)
    for boundary, frame, offset, value in ((2, 10, 0, 0xAB), (0, 10, 64, 0xCD), (1, 12, 0, 0xEF)):
        state[offset:offset + 64] = bytes([value]) * 64
        stream += word(1) + word(7) + word(frame) + word(boundary) + word(23)
        stream += word(offset) + state[offset:offset + 64] + word(0xFFFFFFFF)
        stream += hashlib.sha256(state).hexdigest().encode('ascii')
    return bytes(stream + word(0) + word(3))


class FrameDeltaAcceptance(unittest.TestCase):
    def decode(self, data, body=True):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'capture.delta'
            path.write_bytes(data)
            return list(bodies(path) if body else snapshots(path))

    def test_complete_body_preserves_all_unchanged_and_overwritten_bytes(self):
        body, = self.decode(fixture())
        self.assertEqual(body['entry']['data'][:64], bytes([0xAB]) * 64)
        self.assertEqual(body['before']['data'][64:128], bytes([0xCD]) * 64)
        self.assertEqual(body['after']['data'][:64], bytes([0xEF]) * 64)
        self.assertEqual(body['after']['data'][64:128], bytes([0xCD]) * 64)
        self.assertEqual(body['after']['data'][128:], bytes(RAM_SIZE - 128))

    def test_truncated_snapshot_or_footer_is_rejected(self):
        for size in (len(MAGIC), len(fixture()) - 1, len(fixture()) - 8):
            with self.subTest(size=size), self.assertRaisesRegex(AssertionError, 'Truncated'):
                self.decode(fixture()[:size])

    def test_changed_block_with_unmodified_hash_is_rejected(self):
        data = bytearray(fixture());data[len(MAGIC) + 24] ^= 1
        with self.assertRaisesRegex(AssertionError, 'RAM hash differs'):
            self.decode(data)

    def test_unaligned_or_out_of_range_block_is_rejected(self):
        for offset in (1, RAM_SIZE):
            data = bytearray(fixture());data[len(MAGIC) + 20:len(MAGIC) + 24] = word(offset)
            with self.subTest(offset=offset), self.assertRaisesRegex(AssertionError, 'Invalid or repeated'):
                self.decode(data)

    def test_wrong_terminal_count_or_trailing_bytes_is_rejected(self):
        for data in (fixture()[:-4] + word(2), fixture() + b'x'):
            with self.assertRaises(AssertionError):
                self.decode(data)

    def test_valid_snapshots_with_body_begin_missing_are_rejected(self):
        data = bytearray(fixture());data[len(MAGIC) + 12:len(MAGIC) + 16] = word(0)
        with self.assertRaisesRegex(AssertionError, 'no input boundary'):
            self.decode(data)


if __name__ == '__main__':
    unittest.main()
