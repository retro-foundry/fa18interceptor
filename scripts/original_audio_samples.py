"""Read the isolated reference observer's actual newsample bytes.

The normal original DLL has no observer API. Explicit selection and a hashed
build manifest are required; acceptance additionally requires paired complete
PCM/execution preservation. This module never supplies native game state.
"""
import ctypes as C
import hashlib
import json
import struct

MAGIC = b'FA18_ORIGINAL_AUDIO_SAMPLES_V1\n'
FRAME = struct.Struct('<IIQQQ')
SAMPLE = struct.Struct('<QIHbBHH')
FOOTER = struct.Struct('<IQ')
CYCLE_UNIT = 512  # sysdeps.h; e9k_debug_read_cycle_count returns get_cycles()/512.


def sample_records(payload, before, after):
    if len(payload) % SAMPLE.size:
        raise ValueError('Truncated original sample record')
    previous = before * CYCLE_UNIT
    for cycle, address, word, value, encoded, hpos, vpos in SAMPLE.iter_unpack(payload):
        channel, state = encoded & 3, encoded >> 2
        if not previous <= cycle < (after+1)*CYCLE_UNIT or state not in (2, 3) or hpos >= 288 or vpos >= 1000:
            raise ValueError('Invalid original sample cycle/state/beam')
        if address != 0xffffffff and (address & 1 or address >= 0x80000):
            raise ValueError('Invalid original sample provenance address')
        expected = (word >> 8) if state == 2 else (word & 255)
        expected = expected if expected < 128 else expected - 256
        if value != expected:
            raise ValueError('Original sample byte differs from consumed word/state')
        previous = cycle
        yield cycle, address, word, value, channel, state, hpos, vpos


class AudioSamplesWriter:
    def __init__(self, engine, file, engine_root):
        self.engine, self.file = engine, file
        manifest = engine_root / 'build.json'
        self.manifest = json.loads(manifest.read_text())
        core = engine_root / 'system/ami9000.dll'
        with core.open('rb') as dll:
            if hashlib.file_digest(dll, 'sha256').hexdigest() != self.manifest['core_sha256']:
                raise RuntimeError('Reference sample observer DLL differs from its manifest')
        if self.manifest['record_bytes'] != SAMPLE.size or self.manifest['ring_records'] != 65536:
            raise RuntimeError('Reference sample observer ABI changed')
        self.manifest_sha256 = hashlib.sha256(manifest.read_bytes()).hexdigest()
        self.manifest_file = str(manifest)
        self.enable = engine.bind('e9k_debug_audio_probe_enable', None, C.c_int)
        self.take = engine.bind('e9k_debug_audio_probe_take', C.c_uint, C.POINTER(C.c_void_p))
        if engine.bind('e9k_debug_audio_probe_record_size', C.c_uint)() != SAMPLE.size:
            raise RuntimeError('Reference sample observer record size differs')
        self.digest = hashlib.sha256()
        self.calls = self.samples = self.unknown = 0
        self.channels = [0] * 4
        self.previous_cycle = engine.core.e9k_debug_read_cycle_count()
        self.write(MAGIC)
        self.enable(1)

    def write(self, data):
        if self.engine.audio_capture_bytes + len(data) > self.engine.audio_capture_budget:
            raise RuntimeError('Original PCM/event/DMA/sample capture exceeds budget')
        self.file.write(data)
        self.digest.update(data)
        self.engine.audio_capture_bytes += len(data)

    def boundary(self, call):
        pointer = C.c_void_p()
        count = self.take(C.byref(pointer))
        if count > 65536 or (count and not pointer.value):
            raise RuntimeError('Reference sample observer overflow or missing buffer')
        after = self.engine.core.e9k_debug_read_cycle_count()
        payload = C.string_at(pointer, count*SAMPLE.size) if count else b''
        for cycle, address, word, value, channel, state, hpos, vpos in sample_records(payload, self.previous_cycle, after):
            self.channels[channel] += 1
            self.unknown += address == 0xffffffff
        self.write(b'\1' + FRAME.pack(call, count, self.previous_cycle, after, self.engine.audio_capture_frames))
        self.write(payload)
        self.previous_cycle = after
        self.calls += 1
        self.samples += count

    def finish(self):
        try:
            self.write(b'\0' + FOOTER.pack(self.calls, self.samples))
        finally:
            self.enable(0)
            self.file.close()
        return dict(file='audio_samples.bin', sha256=self.digest.hexdigest(), calls=self.calls,
            consumed_bytes=self.samples, bytes_by_channel=self.channels,
            unknown_initial_provenance_bytes=self.unknown, record_bytes=SAMPLE.size,
            boundary_cycle_unit=CYCLE_UNIT,
            ring_records=65536, observer_manifest_sha256=self.manifest_sha256,
            observer_manifest_file=self.manifest_file,
            observer=self.manifest,
            scope='Every newsample byte during requested ordinary replay calls; service cycle/beam and actual '
                'word provenance. Initial restored pipeline provenance is explicit unknown. Paired original '
                'PCM/execution preservation is mandatory; native sound acceptance remains separate.')
