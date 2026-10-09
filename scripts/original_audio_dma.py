"""Export actual audio fetch words from the pinned core's existing DMA view.

Authority: e9k-lib.h DMA ABI; custom.c:dmal_func; debug.c:record_dma_read_value.
Mode 6 collects records without drawing debugger overlays. No register reads,
instruction stepping, serialization, or emulator modifications are involved.
"""
import ctypes as C
import hashlib
import struct

import numpy as np

from capture_dma_frame import Record, View

MAGIC = b'FA18_ORIGINAL_AUDIO_DMA_V1\n'
FRAME = struct.Struct('<IIiiiiiiIQ')
FETCH = struct.Struct('<IIHHhhH')
FOOTER = struct.Struct('<IQ')

# Derive raw field offsets from the existing pinned ctypes ABI. The output is
# packed little-endian and independent of host struct padding.
FIELDS = ('type', 'reg', 'dat', 'size', 'addr', 'hpos', 'vpos', 'extra', 'cf_reg')
TYPES = ('<i2', '<u2', '<u8', '<u2', '<u4', '<i4', '<i4', '<u2', '<u2')
RAW = np.dtype(dict(names=FIELDS, formats=TYPES,
    offsets=[getattr(Record, field).offset for field in FIELDS], itemsize=C.sizeof(Record)))
PACKED = np.dtype([('index', '<u4'), ('address', '<u4'), ('value', '<u2'),
    ('register', '<u2'), ('hpos', '<i2'), ('vpos', '<i2'), ('channel', '<u2')])


def fetch_words(records):
    """Return every audio DMA slot, failing on conflicting or invalid records."""
    raw = np.frombuffer(records, dtype=RAW)
    # A conflicting access can replace the word in cf_dat. Never silently
    # select the surviving first access and claim complete fetch coverage.
    audio_register = np.isin(raw['cf_reg'], (0xaa, 0xba, 0xca, 0xda))
    if np.any(audio_register) or np.any((raw['type'] == 4) & (raw['cf_reg'] != 0xffff)):
        raise ValueError('Audio DMA slot conflict: complete fetch coverage unavailable')
    indexes = np.flatnonzero(raw['type'] == 4)
    selected = raw[indexes]
    if np.any(selected['size'] != 2) or np.any(selected['dat'] > 65535):
        raise ValueError('Invalid audio DMA word')
    if np.any(selected['extra'] > 3) or np.any(selected['reg'] != 0xaa + 16 * selected['extra']):
        raise ValueError('Invalid audio DMA channel/register')
    if np.any(selected['addr'] >= 0x80000) or np.any(selected['addr'] & 1):
        raise ValueError('Audio DMA fetch outside aligned original Chip RAM')
    if np.any(selected['hpos'] < 0) or np.any(selected['hpos'] > 32767) or np.any(selected['vpos'] < 0) or np.any(selected['vpos'] > 32767):
        raise ValueError('Invalid audio DMA beam position')
    output = np.empty(len(selected), dtype=PACKED)
    output['index'] = indexes
    for target, source in (('address', 'addr'), ('value', 'dat'), ('register', 'reg'),
                           ('hpos', 'hpos'), ('vpos', 'vpos'), ('channel', 'extra')):
        output[target] = selected[source]
    return output


class AudioDmaWriter:
    def __init__(self, engine, file):
        if C.sizeof(Record) != 88 or PACKED.itemsize != FETCH.size:
            raise RuntimeError('Unexpected pinned audio DMA ABI')
        self.engine, self.file = engine, file
        self.get_view = engine.bind('e9k_debug_amiga_dma_debug_get_frame_view', C.POINTER(View), C.c_uint)
        self.mode = engine.bind('e9k_debug_amiga_get_dma_addr', C.POINTER(C.c_int))()
        if not self.mode:
            raise RuntimeError('Original DMA collector unavailable')
        self.previous_mode = self.mode.contents.value
        self.mode.contents.value = 6
        # Initialize the core's existing two buffers before the first replay.
        if not self.get_view(0):
            raise RuntimeError('Original DMA frame view unavailable')
        self.calls = self.words = 0
        self.first_frame = self.last_frame = None
        self.channels = [0] * 4
        self.digest = hashlib.sha256()
        self.write(MAGIC)

    def write(self, data):
        engine = self.engine
        if engine.audio_capture_bytes + len(data) > engine.audio_capture_budget:
            raise RuntimeError('Original audio DMA export exceeds capture budget')
        self.file.write(data)
        self.digest.update(data)
        engine.audio_capture_bytes += len(data)

    def boundary(self, call):
        view = self.get_view(0)
        if not view:
            raise RuntimeError('Original completed DMA frame unavailable')
        info = view.contents.info
        if info.version != 1 or info.frameSelect != 0 or not info.debugDmaEnabled or not view.contents.records:
            raise RuntimeError('Invalid original DMA frame descriptor')
        if (info.hposCount, info.vposCount, info.recordCount) != (288, 1000, 288000):
            raise RuntimeError('Pinned original DMA grid changed')
        if info.frameNumber < 0 or (self.last_frame is not None and info.frameNumber != self.last_frame + 1):
            raise RuntimeError('Missing or repeated original completed DMA frame')
        raw = (C.c_ubyte * (info.recordCount * C.sizeof(Record))).from_address(view.contents.records)
        words = fetch_words(raw)
        self.write(b'\1' + FRAME.pack(call, self.engine.hardware_frame,
            info.frameNumber, info.recordToggle, info.hposCount, info.vposCount,
            info.dmaHoffset, info.recordCount, len(words), self.engine.audio_capture_frames))
        self.write(words.tobytes())
        for channel in range(4):
            self.channels[channel] += int(np.count_nonzero(words['channel'] == channel))
        self.calls += 1
        self.words += len(words)
        if self.first_frame is None:
            self.first_frame = info.frameNumber
        self.last_frame = info.frameNumber

    def finish(self):
        try:
            self.write(b'\0' + FOOTER.pack(self.calls, self.words))
        finally:
            self.file.close()
            self.mode.contents.value = self.previous_mode
        return dict(file='audio_dma.bin', sha256=self.digest.hexdigest(),
            calls=self.calls, fetched_words=self.words, words_by_channel=self.channels,
            first_source_frame=self.first_frame, last_source_frame=self.last_frame,
            raw_record_bytes=C.sizeof(Record), packed_fetch_bytes=FETCH.size,
            collection_mode=6,
            scope='Every audio DMA record in consecutive completed core frame views; actual fetched word/address/beam, not retained-RAM substitution. Native sound acceptance remains open.')
