"""Decode bounded, complete host-RAM snapshots; never supply gameplay state."""
import gzip
import hashlib
import struct
from pathlib import Path

MAGIC = b'FA18_FRAME_DELTA_V1\n'
RAM_SIZE = 0x100000
BLOCK_SIZE = 64


def snapshots(path):
    """Require ordered block offsets, full-MiB hashes and the terminal count."""
    path = Path(path)
    opener = gzip.open if path.suffix == '.gz' else open
    with opener(path, 'rb') as file:
        def read(size):
            data = file.read(size)
            assert len(data) == size, 'Truncated frame delta'
            return data

        def word():
            return struct.unpack('<I', read(4))[0]

        assert read(len(MAGIC)) == MAGIC, 'Unknown frame delta format'
        state, count = bytearray(RAM_SIZE), 0
        while True:
            tag = word()
            if tag == 0:
                assert word() == count, 'Frame delta terminal count differs'
                assert not file.read(1), 'Data follows frame delta terminal count'
                return
            assert tag == 1, 'Unknown frame delta record'
            iteration, frame, boundary, saved_tick = (word() for _ in range(4))
            assert iteration > 0 and boundary in (0, 1, 2, 3) and saved_tick <= 0xFFFF
            previous_offset = -BLOCK_SIZE
            while True:
                offset = word()
                if offset == 0xFFFFFFFF:
                    break
                assert offset % BLOCK_SIZE == 0 and previous_offset < offset <= RAM_SIZE - BLOCK_SIZE, \
                    'Invalid or repeated frame delta block'
                state[offset:offset + BLOCK_SIZE] = read(BLOCK_SIZE)
                previous_offset = offset
            digest = read(64).decode('ascii')
            assert hashlib.sha256(state).hexdigest() == digest, 'Complete frame delta RAM hash differs'
            count += 1
            yield dict(iteration=iteration, frame=frame, boundary=boundary, saved_tick=saved_tick,
                       ram_sha256=digest, data=bytes(state))


def bodies(path):
    """Require actual input -> body begin -> complete body end on each update."""
    entry = before = None
    last = 0
    for snapshot in snapshots(path):
        if snapshot['boundary'] == 2:
            assert entry is None and before is None and snapshot['iteration'] > last, 'Unfinished or repeated frame delta body'
            entry = snapshot
        elif snapshot['boundary'] == 0:
            assert entry is not None and before is None and snapshot['iteration'] == entry['iteration'], 'Body begin has no input boundary'
            assert snapshot['saved_tick'] == entry['saved_tick']
            before = snapshot
        else:
            assert snapshot['boundary'] in (1, 3) and entry is not None and before is not None, 'Body end has no body begin'
            assert snapshot['iteration'] == entry['iteration'] and snapshot['saved_tick'] == before['saved_tick']
            assert entry['frame'] <= before['frame'] <= snapshot['frame'], 'Frame clock runs backwards'
            yield dict(entry=entry, before=before, after=snapshot)
            last, entry, before = snapshot['iteration'], None, None
    assert entry is None and before is None, 'Frame delta ended inside a body'
