"""Bounded headless host for the unmodified Engine9000 ami9000 DLL.

ABI authority: pinned engine9000-src/{ami9000/libretro,e9k-debugger/libretro_host.c}.
This is an automation host, not an emulator implementation. One core per process.
"""
import argparse
import ctypes as C
import hashlib
import json
import os
import time
import wave
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
# Captures may be sealed with the standalone replay runner. Keep the tools
# core as the default, but allow a caller to select the exact recorded engine
# root for instruction-faithful traces.
ENGINE = Path(os.environ.get('FA18_ENGINE_ROOT',
                            str(ROOT / 'tools/engine9000/e9k-debugger')))
U = C.c_uint
P = C.c_void_p
S = C.c_size_t
ENV = C.CFUNCTYPE(C.c_bool, U, P)
VIDEO = C.CFUNCTYPE(None, P, U, U, S)
AUDIO = C.CFUNCTYPE(None, C.c_int16, C.c_int16)
BATCH = C.CFUNCTYPE(S, P, S)
POLL = C.CFUNCTYPE(None)
INPUT = C.CFUNCTYPE(C.c_int16, U, U, U, U)
KEY = C.CFUNCTYPE(None, C.c_bool, U, C.c_uint32, C.c_uint16)
LOG = C.CFUNCTYPE(None, C.c_int, C.c_char_p)
VBLANK = C.CFUNCTYPE(None, P)
CUSTOM_FRAME = C.CFUNCTYPE(None, P, S, U, C.c_uint64, P)
LED = C.CFUNCTYPE(None, C.c_int, C.c_int)


class LedInterface(C.Structure):
    _fields_ = [('set_led_state', LED)]


class CustomWrite(C.Structure):
    _fields_ = [('vpos', C.c_uint16), ('hpos', C.c_uint16), ('reg', C.c_uint16),
                ('value', C.c_uint16), ('source', C.c_uint32), ('copper', C.c_uint8),
                ('reserved', C.c_uint8 * 3)]


class Variable(C.Structure):
    _fields_ = [('key', C.c_char_p), ('value', C.c_char_p)]


class Option(C.Structure):
    _fields_ = [('key', C.c_char_p), ('desc', C.c_char_p), ('desc_cat', C.c_char_p),
                ('info', C.c_char_p), ('info_cat', C.c_char_p), ('category', C.c_char_p),
                ('values', Variable * 128), ('default', C.c_char_p)]


class Options(C.Structure):
    _fields_ = [('categories', P), ('definitions', C.POINTER(Option))]


class Game(C.Structure):
    _fields_ = [('path', C.c_char_p), ('data', P), ('size', S), ('meta', C.c_char_p)]


class Descriptor(C.Structure):
    _fields_ = [('flags', C.c_uint64), ('ptr', P), ('offset', S), ('start', S),
                ('select', S), ('disconnect', S), ('len', S), ('addrspace', C.c_char_p)]


class MemoryMap(C.Structure):
    _fields_ = [('descriptors', C.POINTER(Descriptor)), ('count', U)]


class Geometry(C.Structure):
    _fields_ = [('width', U), ('height', U), ('max_width', U), ('max_height', U),
                ('aspect', C.c_float)]


class Timing(C.Structure):
    _fields_ = [('fps', C.c_double), ('sample_rate', C.c_double)]


class AvInfo(C.Structure):
    _fields_ = [('geometry', Geometry), ('timing', Timing)]


def sha(data):
    return hashlib.sha256(data).hexdigest()


class Engine:
    def __init__(self, config, save_dir, observe_led=False):
        self.options = {}
        self.overrides = {}
        for line in config.read_text().splitlines():
            if line.startswith('puae_'):
                key, val = line.split('=', 1)
                self.overrides[key.encode()] = val.encode()
        self.paths = {9: str(ROOT / 'local/system').encode(), 31: str(save_dir).encode()}
        self.keyboard = None
        self.keys = {}
        self.joy = {}
        self.mouse = {}
        self.pending = [[0, 0] for _ in range(4)]
        self.motion = [[0, 0] for _ in range(4)]
        self.pixel_format = 0
        self.video = None
        self.video_count = 0
        self.frame = 0
        self.hardware_frame = 0
        self.custom_log = None
        self.maps = []
        self.audio_hash = hashlib.sha256()
        self.audio_capture = None
        self.audio_capture_hash = hashlib.sha256()
        self.audio_capture_frames = 0
        self.audio_capture_bytes = 44
        self.audio_capture_budget = 0
        self.audio_capture_error = None
        self.audio_capture_log = None
        self.audio_capture_call = 0
        self.audio_events = None
        self.audio_voice_reader = None
        self.audio_event_rows = 0
        # The core's retro_led_state[] starts at zero and only publishes
        # changes. Capture the interface from initialization, before restore.
        # libretro-core.c:retro_led_interface publishes gui_data.powerled,
        # which cia.c:led_vsync also passes to audio.c:led_filter_audio.
        self.observe_led = observe_led
        self.led_states = [0] * 9
        self.led_callback = LED(self.on_led)
        self.led_interface_available = False
        self.env_commands = set()
        # A Python host does not initialise the MinGW DLL's stderr. The callback
        # accepts the fixed prefix of libretro's printf ABI; unused varargs are
        # deliberately not interpreted. Keep the raw formats for diagnostics.
        self.log_formats = []
        self.log_callback = LOG(lambda level, fmt: self.log_formats.append(fmt.decode(errors='replace')))
        self.dll_dir = os.add_dll_directory(str(ENGINE))
        self.core = C.CDLL(str(ENGINE / 'system/ami9000.dll'))
        self.callbacks = [ENV(self.environment), VIDEO(self.on_video), AUDIO(lambda l, r: None),
                          BATCH(self.on_audio), POLL(self.poll), INPUT(self.input)]
        for name, cb in zip(['environment', 'video_refresh', 'audio_sample', 'audio_sample_batch', 'input_poll', 'input_state'], self.callbacks):
            self.bind('retro_set_' + name, None, type(cb))(cb)
        for name in ['retro_init', 'retro_run', 'retro_reset', 'retro_unload_game', 'retro_deinit',
                     'e9k_debug_pause', 'e9k_debug_resume', 'e9k_debug_step_instr']:
            self.bind(name, None)
        self.bind('retro_load_game', C.c_bool, C.POINTER(Game))
        self.bind('retro_set_controller_port_device', None, U, U)
        self.bind('retro_serialize_size', S)
        self.bind('retro_serialize', C.c_bool, P, S)
        self.bind('retro_unserialize', C.c_bool, P, S)
        self.bind('e9k_debug_read_regs', S, P, S)
        self.bind('e9k_debug_read_memory', S, U, P, S)
        self.bind('e9k_debug_read_cycle_count', C.c_uint64)
        self.bind('e9k_debug_is_paused', C.c_int)
        self.bind('e9k_debug_add_breakpoint', None, U)
        self.bind('e9k_debug_remove_breakpoint', None, U)
        self.core.retro_init()
        game = Game(str(config).encode(), None, 0, None)
        if not self.core.retro_load_game(C.byref(game)):
            raise RuntimeError('Engine9000 failed to load UAE config')
        # Same automatic controller devices and reset as the stock frontend.
        for port in range(2):
            self.core.retro_set_controller_port_device(port, 1)
        self.core.retro_reset()
        self.vblank_callback = VBLANK(self.on_vblank)
        self.bind('e9k_debug_set_vblank_callback', None, VBLANK, P)(self.vblank_callback, None)
        self.custom_callback = CUSTOM_FRAME(self.on_custom_frame)
        self.bind('e9k_debug_set_amiga_custom_log_frame_callback', None, CUSTOM_FRAME, P)(self.custom_callback, None)

    def on_vblank(self, user):
        self.frame += 1

    def on_custom_frame(self, entries, count, dropped, core_frame, user):
        self.hardware_frame += 1
        if self.audio_events is not None and not self.audio_capture_error:
            try:
                if dropped:
                    raise RuntimeError(f'Original audio trace lost {dropped} custom writes')
                rows = C.cast(entries, C.POINTER(CustomWrite))
                for i in range(count):
                    r = rows[i]
                    if 0xa0 <= r.reg <= 0xda or r.reg in (0x96, 0x9a, 0x9c, 0x9e):
                        self.audio_voice_reader.observe_write(r.source)
                        self.write_audio_event({'kind': 'write',
                            'call': self.audio_capture_call,
                            'hardware_frame': self.hardware_frame,
                            'vpos': r.vpos, 'hpos': r.hpos,
                            'address': 0xdff000 + r.reg, 'value': r.value,
                            'source': r.source, 'copper': bool(r.copper)})
            except (OSError, RuntimeError) as error:
                self.audio_capture_error = str(error)
        if self.custom_log is not None:
            rows = C.cast(entries, C.POINTER(CustomWrite))
            for i in range(count):
                r = rows[i]
                self.custom_log.write(json.dumps({'hardware_frame': self.hardware_frame,
                    'vpos': r.vpos, 'hpos': r.hpos, 'address': 0xdff000 + r.reg,
                    'value': r.value, 'source': r.source, 'copper': bool(r.copper)}) + '\n')
            if dropped:
                self.custom_log.write(json.dumps({'error': 'dropped custom writes', 'count': dropped}) + '\n')

    def bind(self, name, result, *args):
        fn = getattr(self.core, name)
        fn.restype, fn.argtypes = result, list(args)
        return fn

    def on_led(self, led, state):
        try:
            if not 0 <= led < len(self.led_states) or state < 0 or (led == 0 and state not in (0, 1)):
                raise RuntimeError(f'Unsupported original LED notification: {led}/{state}')
            self.led_states[led] = state
            if self.audio_events is not None and not self.audio_capture_error:
                self.write_audio_event({'kind': 'led', 'call': self.audio_capture_call,
                    'sample_frames': self.audio_capture_frames, 'led': led, 'state': state})
        except (OSError, RuntimeError) as error:
            self.audio_capture_error = str(error)

    def write_audio_event(self, row):
        line = json.dumps(row) + '\n'
        size = len(line.encode('utf8'))
        if self.audio_capture_bytes + size > self.audio_capture_budget:
            raise RuntimeError('Original audio events exceed the bounded capture budget')
        self.audio_events.write(line)
        self.audio_event_rows += 1
        self.audio_capture_bytes += size

    def environment(self, cmd, data):
        self.env_commands.add(cmd)
        if cmd == (46 | 0x10000) and self.observe_led:
            C.cast(data, C.POINTER(LedInterface)).contents.set_led_state = self.led_callback
            self.led_interface_available = True
            return True
        if cmd == 27:
            C.cast(data, C.POINTER(P))[0] = C.cast(self.log_callback, P).value
            return True
        if cmd in self.paths:
            C.cast(data, C.POINTER(C.c_char_p))[0] = self.paths[cmd]
            return True
        if cmd == 52:  # GET_CORE_OPTIONS_VERSION
            C.cast(data, C.POINTER(U))[0] = 2
            return True
        if cmd in (67, 68):  # SET_CORE_OPTIONS_V2 / INTL
            ptr = data if cmd == 67 else C.cast(data, C.POINTER(P))[0]
            defs = C.cast(ptr, C.POINTER(Options)).contents.definitions
            i = 0
            while defs[i].key:
                self.options[defs[i].key] = defs[i].default or defs[i].values[0].key
                i += 1
            return True
        if cmd == 15:  # GET_VARIABLE
            var = C.cast(data, C.POINTER(Variable)).contents
            val = self.overrides.get(var.key, self.options.get(var.key))
            var.value = val
            return val is not None
        if cmd == 17:  # GET_VARIABLE_UPDATE
            C.cast(data, C.POINTER(C.c_bool))[0] = False
            return True
        if cmd == 10:
            self.pixel_format = C.cast(data, C.POINTER(U))[0]
            return self.pixel_format in (0, 1, 2)
        if cmd == 12:
            self.keyboard = KEY(C.cast(data, C.POINTER(P))[0])
            return True
        if cmd == (36 | 0x10000):
            mm = C.cast(data, C.POINTER(MemoryMap)).contents
            self.maps = [{k: getattr(mm.descriptors[i], k) for k in ['flags', 'start', 'len', 'offset', 'select', 'disconnect']}
                         for i in range(mm.count)]
            return True
        return cmd in (1, 3, 6, 11, 16, 18, 32, 37, 44, 55, 69)

    def on_video(self, ptr, width, height, pitch):
        if ptr:
            self.video = (C.string_at(ptr, pitch * height), width, height, pitch)
            self.video_count += 1

    def on_audio(self, ptr, frames):
        data = C.string_at(ptr, frames * 4)
        self.audio_hash.update(data)
        if self.audio_capture is not None and not self.audio_capture_error:
            # Exceptions raised inside ctypes callbacks are otherwise printed
            # and swallowed. Retain failure for the normal replay caller.
            try:
                row = json.dumps({'call': self.audio_capture_call,
                    'vblank': self.frame, 'hardware_frame': self.hardware_frame,
                    'first_sample': self.audio_capture_frames, 'frames': frames,
                    'pcm_sha256': sha(data)}) + '\n'
                used = len(data) + len(row.encode('utf8'))
                if self.audio_capture_bytes + used > self.audio_capture_budget:
                    raise RuntimeError('Original PCM exceeds the bounded capture budget')
                self.audio_capture.writeframesraw(data)
                self.audio_capture_hash.update(data)
                if self.audio_capture_log is not None:
                    self.audio_capture_log.write(row)
                self.audio_capture_frames += frames
                self.audio_capture_bytes += used
            except (OSError, RuntimeError) as error:
                self.audio_capture_error = str(error)
        return frames

    def poll(self):
        self.motion = [row[:] for row in self.pending]
        self.pending = [[0, 0] for _ in range(4)]

    def input(self, port, device, index, ident):
        device &= 255
        if device == 3:
            return self.keys.get(ident, 0)
        if device == 1:
            return self.joy.get((port, ident), 0)
        if device == 2 and port < 4:
            if ident < 2:
                return max(-32768, min(32767, self.motion[port][ident]))
            return self.mouse.get((port, ident), 0)
        return 0

    def event(self, kind, args):
        if kind == 'K':
            key, char, mods, down = args
            self.keys[key] = down
            if self.keyboard:
                self.keyboard(bool(down), key, char, mods)
        elif kind == 'J':
            port, ident, down = args
            self.joy[port, ident] = down
        elif kind == 'C':
            self.joy.clear()
        elif kind in ('m', 'b'):
            port, a, b = args
            for p in range(4) if port == 4 else [port]:
                if kind == 'm':
                    self.pending[p][0] += a
                    self.pending[p][1] += b
                else:
                    self.mouse[p, a] = b
        elif kind != 'U':
            raise ValueError(f'Unsupported recorded event {kind}')

    def regs(self):
        buf = (C.c_uint32 * 18)()
        if self.core.e9k_debug_read_regs(buf, 18) != 18:
            raise RuntimeError('Incomplete 68000 register read')
        return dict(zip([f'd{i}' for i in range(8)] + [f'a{i}' for i in range(8)] + ['sr', 'pc'], buf))

    def memory(self, addr, size):
        buf = C.create_string_buffer(size)
        if self.core.e9k_debug_read_memory(addr, buf, size) != size:
            raise RuntimeError(f'Incomplete memory read at {addr:06x}')
        return buf.raw

    def state(self):
        size = self.core.retro_serialize_size()
        # Core can discover a larger size during serialization (stock host does this too).
        while size <= 64 * 1024 * 1024:
            buf = C.create_string_buffer(size)
            if self.core.retro_serialize(buf, size):
                return buf.raw[:self.core.retro_serialize_size()]
            size = max(size * 2, self.core.retro_serialize_size())
        raise RuntimeError('Core serialization exceeds 64 MiB')

    def screenshot(self, path):
        from PIL import Image
        if self.video is None:
            raise RuntimeError('No video frame')
        data, w, h, pitch = self.video
        mode = {0: 'BGR;15', 1: 'BGRX', 2: 'BGR;16'}[self.pixel_format]
        Image.frombytes('RGB', (w, h), data, 'raw', mode, pitch).save(path)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--frames', type=int, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--playback', type=Path)
    parser.add_argument('--restore', type=Path)
    parser.add_argument('--config', type=Path, default=ROOT / 'local/fa18.uae')
    parser.add_argument('--start-frame', type=int, default=0)
    parser.add_argument('--trace-frames', type=int, default=0,
                        help='Single-step this many video frames after the normal replay window')
    parser.add_argument('--normal-custom-log', action='store_true',
                        help='Record Custom-register writes during ordinary full-frame replay')
    parser.add_argument('--wav', action='store_true', help='Capture complete reference PCM during the requested replay window')
    parser.add_argument('--audio-events', action='store_true',
                        help='With --wav/--restore, record audio/control writes, safe voice RAM and published LED/filter state at every replay boundary')
    parser.add_argument('--audio-dma', action='store_true',
                        help='With --audio-events, export actual fetched audio words from the existing DMA collector')
    parser.add_argument('--capture-budget-mib', type=int, default=512)
    args = parser.parse_args()
    if args.trace_frames and args.normal_custom_log:
        parser.error('--normal-custom-log cannot be combined with --trace-frames')
    if args.wav and args.trace_frames:
        parser.error('--wav requires ordinary full-frame replay, without instruction stepping')
    if args.audio_events and (not args.wav or not args.restore or args.normal_custom_log):
        parser.error('--audio-events requires --wav/--restore and its own filtered custom log')
    if args.audio_dma and not args.audio_events:
        parser.error('--audio-dma requires --audio-events')
    if args.frames < 0 or (args.wav and (args.frames < 1 or args.capture_budget_mib < 8)):
        parser.error('PCM capture requires positive frames and at least 8 MiB capture budget')
    args.output.mkdir(parents=True, exist_ok=False)
    saves = args.output / 'saves'
    saves.mkdir()
    events = {}
    if args.playback:
        lines = args.playback.read_text().splitlines()
        if not lines or lines[0] != 'E9K_INPUT_V1':
            raise ValueError('Expected E9K_INPUT_V1 recording')
        for line in lines[1:]:
            parts = line.split()
            if parts:
                if parts[0] != 'F':
                    raise ValueError(line)
                events.setdefault(int(parts[1]), []).append((parts[2], list(map(int, parts[3:]))))
    if args.trace_frames:
        trace_first = args.start_frame + args.frames + 1
        trace_last = trace_first + args.trace_frames - 1
        stepped_events = [frame for frame in events if trace_first <= frame <= trace_last]
        if stepped_events:
            raise ValueError(
                f'Recorded input occurs in instruction-stepped frames {stepped_events}. '
                'The stepping frontend does not yet have validated held-input/autorepeat parity; '
                'trace a no-input window or use a full-frame replay capture.')
    start = time.monotonic()
    engine = Engine(args.config.resolve(), saves.resolve(), observe_led=args.audio_events)
    if args.restore:
        engine.core.retro_run()  # Initialise UAE before requesting its synchronous restore.
        payload = args.restore.read_bytes()
        if not engine.core.retro_unserialize(payload, len(payload)):
            raise RuntimeError('Core rejected save state')
    engine.frame = args.start_frame
    engine.hardware_frame = args.start_frame
    av = AvInfo()
    engine.bind('retro_get_system_av_info', None, C.POINTER(AvInfo))(C.byref(av))
    if args.wav:
        rate = av.timing.sample_rate
        if not rate.is_integer() or rate < 1:
            raise RuntimeError(f'Unsupported original PCM sample rate: {rate}')
        engine.audio_capture_budget = (args.capture_budget_mib - 8) * 1024 * 1024
        if args.frames * rate * 4 / av.timing.fps > engine.audio_capture_budget:
            raise RuntimeError('Requested original PCM window exceeds capture budget')
        engine.audio_capture = wave.open(str(args.output / 'original.wav'), 'wb')
        engine.audio_capture.setparams((2, 2, int(rate), 0, 'NONE', 'not compressed'))
        engine.audio_capture_log = (args.output / 'audio_chunks.jsonl').open('w', encoding='utf8')
    if args.audio_events:
        from original_audio_events import audio_state, VoiceReader
        if not engine.led_interface_available:
            raise RuntimeError('Original core did not request the LED interface')
        engine.audio_events = (args.output / 'audio_events.jsonl').open('w', encoding='utf8')
        engine.audio_voice_reader = VoiceReader(engine.memory)
        engine.audio_voice_reader.discover()
        engine.write_audio_event({'kind': 'initial', 'call': args.start_frame,
                                 'sample_frames': 0, **audio_state(payload),
                                 **engine.audio_voice_reader.voices(), 'led_states': engine.led_states.copy()})
        engine.bind('e9k_debug_set_debug_option', None, U, U, P)(38, 1, None)
    samples = []
    normal_custom = None
    if args.normal_custom_log:
        # e9k-lib.h: E9K_DEBUG_OPTION_AMIGA_CUSTOM_LOGGER = 38.
        engine.bind('e9k_debug_set_debug_option', None, U, U, P)(38, 1, None)
        normal_custom = (args.output / 'normal_custom_writes.jsonl').open('w', encoding='utf8')
        engine.custom_log = normal_custom
    dma = None
    dma_report = None
    if args.audio_dma:
        from original_audio_dma import AudioDmaWriter
        dma = AudioDmaWriter(engine, (args.output / 'audio_dma.bin').open('wb'))
    try:
        for frame in range(args.start_frame + 1, args.start_frame + args.frames + 1):
            engine.audio_capture_call = frame
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            if engine.audio_capture_error:
                raise RuntimeError(engine.audio_capture_error)
            if engine.frame != frame:
                raise RuntimeError(f'Expected video frame {frame}, actual {engine.frame}')
            if args.audio_events:
                engine.write_audio_event({'kind': 'boundary', 'call': frame,
                    'sample_frames': engine.audio_capture_frames, **engine.audio_voice_reader.voices(),
                    'led_states': engine.led_states.copy()})
            if dma is not None:
                dma.boundary(frame)
            if frame % 100 == 0:
                samples.append({'frame': frame, 'pc': engine.regs()['pc']})
    finally:
        if dma is not None:
            dma_report = dma.finish()
        if normal_custom is not None:
            engine.custom_log = None
            normal_custom.close()
        if args.wav:
            engine.audio_capture.close()
            engine.audio_capture = None
            engine.audio_capture_log.close()
        if engine.audio_events is not None:
            engine.audio_events.close()
            engine.audio_events = None
    if args.wav and not engine.audio_capture_frames:
        raise RuntimeError('Reference emulator emitted no PCM samples')
    (args.output / 'state.bin').write_bytes(engine.state())
    maps = []
    for name, address, size in [('chip', 0, 0x80000), ('slow', 0xc00000, 0x80000)]:
        payload = engine.memory(address, size)
        (args.output / f'{name}.bin').write_bytes(payload)
        maps.append({'name': name, 'start': address, 'size': size, 'file': f'{name}.bin', 'sha256': sha(payload)})
    engine.screenshot(args.output / 'screen.png')
    report = {'frame': args.start_frame + args.frames, 'registers': engine.regs(),
              'hardware_frame': engine.hardware_frame,
              'authority': {'core_sha256': sha((ENGINE / 'system/ami9000.dll').read_bytes()),
                            'config_sha256': sha(args.config.read_bytes()),
                            'initial_state_sha256': sha(args.restore.read_bytes()) if args.restore else None,
                            'recording_sha256': sha(args.playback.read_bytes()) if args.playback else None,
                            'snapshot_sha256': sha((args.output / 'state.bin').read_bytes())},
              'cycles': engine.core.e9k_debug_read_cycle_count(), 'memory': maps,
              'core_memory_map': engine.maps, 'samples': samples,
              'video_sha256': sha(engine.video[0]), 'audio_sha256': engine.audio_hash.hexdigest(),
              'core_options': {k.decode(): engine.overrides.get(k, v).decode() for k, v in engine.options.items()},
              'wall_seconds': round(time.monotonic() - start, 3)}
    if args.wav:
        with (args.output / 'original.wav').open('rb') as captured:
            wav_hash = hashlib.file_digest(captured, 'sha256').hexdigest()
        report['recorded_audio'] = {'file': 'original.wav', 'channels': 2, 'sample_bits': 16,
            'first_replay_call': args.start_frame + 1, 'replay_calls': args.frames,
            'sample_rate': int(av.timing.sample_rate), 'reported_fps': av.timing.fps,
            'sample_frames': engine.audio_capture_frames,
            'pcm_sha256': engine.audio_capture_hash.hexdigest(),
            'wav_sha256': wav_hash,
            'capture_scope': 'All batch PCM from requested ordinary retro_run calls; startup/restore initialization excluded',
            'capture_budget_mib': args.capture_budget_mib}
    if args.audio_events:
        with (args.output / 'audio_events.jsonl').open('rb') as log:
            event_hash = hashlib.file_digest(log, 'sha256').hexdigest()
        report['audio_events'] = {'file': 'audio_events.jsonl',
            'rows': engine.audio_event_rows, 'sha256': event_hash,
            'final_hardware': audio_state((args.output / 'state.bin').read_bytes()),
            'led_interface': True,
            'final_led_states': engine.led_states.copy(),
            'voice_hunk_layout': True,
            'resolved_voice_layout': engine.audio_voice_reader.layout,
            'voice_source_executable_sha256': engine.audio_voice_reader.disk_sha256,
            'scope': 'All logged audio/control custom writes, safe voice RAM and published LED/filter state at every full-frame boundary; CIA pin/duty only known at retained endpoints'}
    if dma_report is not None:
        report['audio_dma'] = dma_report
    (args.output / 'snapshot.json').write_text(json.dumps(report, indent=2) + '\n')
    if args.trace_frames:
        import capstone
        decoder = capstone.Cs(capstone.CS_ARCH_M68K, capstone.CS_MODE_BIG_ENDIAN | capstone.CS_MODE_M68K_000)
        target_frame = engine.hardware_frame + args.trace_frames
        engine.core.e9k_debug_pause()
        engine.core.retro_run()
        # e9k-lib.h: E9K_DEBUG_OPTION_AMIGA_CUSTOM_LOGGER = 38.
        engine.bind('e9k_debug_set_debug_option', None, U, U, P)(38, 1, None)
        count = 0
        with (args.output / 'trace.jsonl').open('w', encoding='utf8') as out, \
                (args.output / 'custom_writes.jsonl').open('w', encoding='utf8') as custom:
            engine.custom_log = custom
            while engine.hardware_frame < target_frame:
                regs = engine.regs()
                pc = regs['pc']
                raw = engine.memory(pc, 10)
                ins = next(decoder.disasm(raw, pc, 1), None)
                if ins is None:
                    raise RuntimeError(f'68000 instruction decode failed at {pc:06x}')
                before = engine.core.e9k_debug_read_cycle_count()
                row = {'frame': engine.hardware_frame + 1, 'index': count, 'pc': pc,
                       'bytes': raw[:ins.size].hex(), 'asm': f'{ins.mnemonic} {ins.op_str}'.strip(),
                       'registers': regs, 'cycles': before}
                engine.core.e9k_debug_step_instr()
                engine.core.retro_run()
                row['next_pc'] = engine.regs()['pc']
                # Capstone 5's 68000 decoder can overrun certain two-byte
                # instructions (observed for SBCD predecrement forms).  A
                # shorter sequential PC advance is CPU evidence of the exact
                # executed length, whereas branch/call targets remain outside
                # this narrow fall-through case.
                executed_length = row['next_pc'] - pc
                if 2 <= executed_length < ins.size and executed_length % 2 == 0:
                    row['decoder_length'] = ins.size
                    row['bytes'] = raw[:executed_length].hex()
                out.write(json.dumps(row, separators=(',', ':')) + '\n')
                count += 1
                if count >= 1000000:
                    raise RuntimeError('Trace exceeded one million instructions')
            engine.custom_log = None
        print(f'Traced {count} instructions to chipset frame {engine.hardware_frame}', flush=True)
        (args.output / 'trace_summary.json').write_text(json.dumps({
            'instructions': count, 'first_frame': report['hardware_frame'] + 1,
            'last_frame': engine.hardware_frame, 'wall_seconds': round(time.monotonic()-start, 3),
            'trace_sha256': sha((args.output/'trace.jsonl').read_bytes()),
            'boundary': 'chipset customLogFrameCommit, not frontend vblank callback',
            'limitation': 'Single-stepping calls retro_run repeatedly; held-input/autorepeat equivalence needs separate verification.'
        }, indent=2)+'\n')
        trace_final = {}
        for name, address, size in [('chip', 0, 0x80000), ('slow', 0xc00000, 0x80000)]:
            payload = engine.memory(address, size)
            (args.output / f'trace_final_{name}.bin').write_bytes(payload)
            trace_final[f'{name}_sha256'] = sha(payload)
        (args.output / 'trace_final_state.bin').write_bytes(engine.state())
        trace_final.update({'hardware_frame': engine.hardware_frame, 'registers': engine.regs(),
                            'cycles': engine.core.e9k_debug_read_cycle_count(),
                            'state_sha256': sha((args.output / 'trace_final_state.bin').read_bytes()),
                            'video_sha256': sha(engine.video[0]), 'audio_sha256': engine.audio_hash.hexdigest()})
        (args.output / 'trace_final.json').write_text(json.dumps(trace_final, indent=2)+'\n')
    print(json.dumps({k: report[k] for k in ['frame', 'registers', 'wall_seconds', 'video_sha256']}), flush=True)
    engine.core.retro_unload_game()
    engine.core.retro_deinit()


if __name__ == '__main__':
    main()
