"""Read the isolated reference observer's actual newsample bytes.

The normal original DLL has no observer API. Explicit selection and a hashed
build manifest are required; acceptance additionally requires paired complete
PCM/execution preservation. This module never supplies native game state.
"""
import ctypes as C
import hashlib
import json
import struct
from pathlib import Path

MAGIC = b'FA18_ORIGINAL_AUDIO_SAMPLES_V1\n'
FRAME = struct.Struct('<IIQQQ')
SAMPLE = struct.Struct('<QIHbBHH')
FOOTER = struct.Struct('<IQ')
WORD_MAGIC = b'FA18_ORIGINAL_AUDIO_WORD_STATES_V1\n'
WORD = struct.Struct('<QIIIHHHHHHBBBBH')
LIVE = struct.Struct('<QIIIIHHHHBBBBBBB')
MIXER_MAGIC = b'FA18_ORIGINAL_AUDIO_MIXER_V1\n'
MIXER = struct.Struct('<QQI4i4IB')
CYCLE_UNIT = 512  # sysdeps.h; e9k_debug_read_cycle_count returns get_cycles()/512.


def mixer_records(payload, before, after):
    if len(payload) % MIXER.size:
        raise ValueError('Truncated original mixer record')
    previous = before * CYCLE_UNIT
    for values in MIXER.iter_unpack(payload):
        service, logical, duration, *rest = values
        kind = rest[-1]
        if not previous <= service < (after + 1) * CYCLE_UNIT or logical > service or kind > 6:
            raise ValueError('Invalid original mixer cycle/kind')
        if kind != 1 and duration:
            raise ValueError('Non-accumulator mixer duration')
        if kind >= 3:
            channel = kind - 3
            if not -128 <= rest[channel] <= 127 or any(rest[i] for i in range(4) if i != channel):
                raise ValueError('Invalid original mixer sample transition')
        previous = service
        yield dict(service_cycle=service, logical_cycle=logical, duration=duration,
                   values=rest[:4], times=rest[4:8], kind=kind)


def live_records(payload):
    if len(payload) != 8 * LIVE.size:
        raise ValueError('Incomplete live audio snapshots')
    names = ('cycle', 'pt', 'lc', 'period_cycles', 'next_cycles', 'length', 'remaining', 'dat', 'dat2',
             'channel', 'state', 'volume', 'interrupt_pending', 'flags', 'drhpos', 'phase')
    result = []
    for index, values in enumerate(LIVE.iter_unpack(payload)):
        row = dict(zip(names, values))
        if (row['channel'], row['phase']) != (index % 4, index // 4) or row['flags'] > 7 or row['interrupt_pending'] > 1:
            raise ValueError('Invalid live audio snapshot order/flags')
        result.append(row)
    return result


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
    def __init__(self, engine, file, engine_root, mixer_call_range=None):
        self.engine, self.file = engine, file
        self.mixer_call_range = mixer_call_range
        if mixer_call_range is not None and not (1 <= mixer_call_range[0] <= mixer_call_range[1]):
            raise ValueError('Mixer call range must be positive and ordered')
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
        self.initial_live_state = None
        if 'live_state_include_sha256' in self.manifest:
            if self.manifest['live_state_record_bytes'] != LIVE.size or self.manifest['live_state_records'] != 8:
                raise RuntimeError('Reference live-state observer ABI changed')
            if engine.bind('e9k_debug_audio_live_record_size', C.c_uint)() != LIVE.size:
                raise RuntimeError('Reference live-state record size differs')
            read_live = engine.bind('e9k_debug_audio_live_read', C.c_uint, C.POINTER(C.c_void_p))
            live_pointer = C.c_void_p()
            if read_live(C.byref(live_pointer)) != 8 or not live_pointer.value:
                raise RuntimeError('Original restore snapshots missing')
            self.initial_live_state = live_records(C.string_at(live_pointer, 8 * LIVE.size))
            if engine.core.e9k_debug_read_cycle_count() != self.previous_cycle:
                raise RuntimeError('Live-state read advanced original execution')
        self.write(MAGIC)
        self.enable(1)
        self.word_file = None
        if 'word_state_include_sha256' in self.manifest:
            if self.manifest['word_state_record_bytes'] != WORD.size or self.manifest['word_state_ring_records'] != 65536:
                raise RuntimeError('Reference word-state observer ABI changed')
            self.word_enable = engine.bind('e9k_debug_audio_word_enable', None, C.c_int)
            self.word_take = engine.bind('e9k_debug_audio_word_take', C.c_uint, C.POINTER(C.c_void_p))
            if engine.bind('e9k_debug_audio_word_record_size', C.c_uint)() != WORD.size:
                raise RuntimeError('Reference word-state record size differs')
            self.word_file = Path(file.name).with_name('audio_word_states.bin').open('wb')
            self.word_digest = hashlib.sha256()
            self.word_total = self.word_max = 0
            self.write_word(WORD_MAGIC)
            self.word_enable(1)
        self.mixer_file = None
        if 'mixer_include_sha256' in self.manifest:
            if self.manifest['mixer_record_bytes'] != MIXER.size or self.manifest['mixer_ring_records'] != 65536:
                raise RuntimeError('Reference mixer observer ABI changed')
            self.mixer_enable = engine.bind('e9k_debug_audio_mixer_enable', None, C.c_int)
            self.mixer_take = engine.bind('e9k_debug_audio_mixer_take', C.c_uint, C.POINTER(C.c_void_p))
            if engine.bind('e9k_debug_audio_mixer_record_size', C.c_uint)() != MIXER.size:
                raise RuntimeError('Reference mixer observer record size differs')
            self.mixer_file = Path(file.name).with_name('audio_mixer.bin').open('wb')
            self.mixer_digest = hashlib.sha256()
            self.mixer_total = self.mixer_max = 0
            self.write_mixer(MIXER_MAGIC)
            self.mixer_enable(mixer_call_range is None)
        elif mixer_call_range is not None:
            raise RuntimeError('Mixer call range requires a mixer observer DLL')

    def before_call(self, call):
        """Read current accumulators at the chosen ordinary replay boundary."""
        if self.mixer_call_range is not None:
            first, last = self.mixer_call_range
            if call == first:
                self.mixer_enable(1)
            elif call == last + 1:
                self.mixer_enable(0)

    def write_mixer(self, data):
        if self.engine.audio_capture_bytes + len(data) > self.engine.audio_capture_budget:
            raise RuntimeError('Original mixer capture exceeds shared budget')
        self.mixer_file.write(data)
        self.mixer_digest.update(data)
        self.engine.audio_capture_bytes += len(data)

    def write_word(self, data):
        if self.engine.audio_capture_bytes + len(data) > self.engine.audio_capture_budget:
            raise RuntimeError('Original word-state capture exceeds shared budget')
        self.word_file.write(data)
        self.word_digest.update(data)
        self.engine.audio_capture_bytes += len(data)

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
        if self.word_file:
            word_pointer = C.c_void_p()
            words = self.word_take(C.byref(word_pointer))
            if words > 65536 or (words and not word_pointer.value):
                raise RuntimeError('Reference word-state overflow or missing buffer')
            word_data = C.string_at(word_pointer, words * WORD.size) if words else b''
            previous = self.previous_cycle * CYCLE_UNIT
            for row in WORD.iter_unpack(word_data):
                if not previous <= row[0] < (after+1)*CYCLE_UNIT or row[10] > 3 or row[12] not in (1,2,3,4):
                    raise RuntimeError('Invalid word-state cycle/channel/kind')
                previous = row[0]
            self.write_word(b'\1' + FRAME.pack(call, words, self.previous_cycle, after, self.engine.audio_capture_frames))
            self.write_word(word_data)
            self.word_total += words
            self.word_max = max(self.word_max, words)
        if self.mixer_file:
            mixer_pointer = C.c_void_p()
            records = self.mixer_take(C.byref(mixer_pointer))
            if records > 65536 or (records and not mixer_pointer.value):
                raise RuntimeError('Reference mixer overflow or missing buffer')
            data = C.string_at(mixer_pointer, records * MIXER.size) if records else b''
            list(mixer_records(data, self.previous_cycle, after))
            self.write_mixer(b'\1' + FRAME.pack(call, records, self.previous_cycle, after, self.engine.audio_capture_frames))
            self.write_mixer(data)
            self.mixer_total += records
            self.mixer_max = max(self.mixer_max, records)
        self.previous_cycle = after
        self.calls += 1
        self.samples += count

    def finish(self):
        try:
            self.write(b'\0' + FOOTER.pack(self.calls, self.samples))
            if self.word_file:
                self.write_word(b'\0' + FOOTER.pack(self.calls, self.word_total))
            if self.mixer_file:
                self.write_mixer(b'\0' + FOOTER.pack(self.calls, self.mixer_total))
        finally:
            self.enable(0)
            self.file.close()
            if self.word_file:
                self.word_enable(0)
                self.word_file.close()
            if self.mixer_file:
                self.mixer_enable(0)
                self.mixer_file.close()
        report = dict(file='audio_samples.bin', sha256=self.digest.hexdigest(), calls=self.calls,
            consumed_bytes=self.samples, bytes_by_channel=self.channels,
            unknown_initial_provenance_bytes=self.unknown, record_bytes=SAMPLE.size,
            boundary_cycle_unit=CYCLE_UNIT,
            ring_records=65536, observer_manifest_sha256=self.manifest_sha256,
            observer_manifest_file=self.manifest_file,
            observer=self.manifest,
            scope='Every newsample byte during requested ordinary replay calls; service cycle/beam and actual '
                'word provenance. Initial restored pipeline provenance is explicit unknown. Paired original '
                'PCM/execution preservation is mandatory; native sound acceptance remains separate.')
        if self.word_file:
            report['word_states'] = dict(file='audio_word_states.bin', sha256=self.word_digest.hexdigest(),
                calls=self.calls, records=self.word_total, max_records_per_call=self.word_max,
                record_bytes=WORD.size, ring_records=65536)
        if self.initial_live_state is not None:
            report['initial_live_state'] = self.initial_live_state
        if self.mixer_file:
            report['mixer_timeline'] = dict(file='audio_mixer.bin', sha256=self.mixer_digest.hexdigest(),
                calls=self.calls, records=self.mixer_total, max_records_per_call=self.mixer_max,
                record_bytes=MIXER.size, ring_records=65536)
            if self.mixer_call_range is not None:
                report['mixer_timeline']['observed_call_range'] = list(self.mixer_call_range)
        return report
