"""Clock snapshots must retain their actual ABI and finite source values."""
import struct
import unittest
import ctypes as C
from types import SimpleNamespace

from original_audio_samples import CLOCK, clock_record, AudioSamplesWriter
from check_original_audio_clock import verified_clock, controls, source_interval, value, f32


def bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


class ClockSnapshotContract(unittest.TestCase):
    def values(self):
        return [51200, 51000, bits(41245.0234375), bits(41245.0234375),
                bits(12000.25), bits(1), 44100, 312, 227, 1, bits(50), bits(50)]

    def test_exact_bits_and_all_source_clock_inputs_are_retained(self):
        payload = CLOCK.pack(*self.values())
        self.assertEqual(CLOCK.size, 56)
        row = clock_record(payload)
        self.assertEqual(row['scaled_current_bits'], bits(41245.0234375))
        self.assertEqual(row['next_output_bits'], bits(12000.25))
        self.assertEqual(row['long_field'], 1)
        self.assertEqual(row['short_line_clocks'], 227)

    def test_truncation_and_extended_record_are_rejected(self):
        payload = CLOCK.pack(*self.values())
        for changed in (payload[:-1], payload + b'x'):
            with self.assertRaises(ValueError): clock_record(changed)

    def test_invalid_cycle_geometry_rate_and_float_are_rejected(self):
        for index, changed in ((0, 50999), (6, 0), (7, 0), (8, 1000), (9, 2),
                               (2, bits(0)), (3, bits(-1)), (5, bits(0)),
                               (4, 0x7fc00000), (10, 0x7f800000)):
            values = self.values(); values[index] = changed
            with self.subTest(index=index), self.assertRaises(ValueError):
                clock_record(CLOCK.pack(*values))

    def clock_case(self, initial=1.25):
        start = clock_record(CLOCK.pack(*self.values()))
        start.update(service_cycle=0, last_cycle=0, next_output_bits=bits(initial), call=1, boundary='before')
        rounded = int(value(start['next_output_bits']) + 0.5)
        interval, _ = source_interval(start)
        next_time = f32(f32(f32(value(start['next_output_bits']) - rounded) + interval) - 100)
        end = dict(start, service_cycle=512, last_cycle=rounded + 100,
                   next_output_bits=bits(next_time), boundary='after')
        rows = [dict(kind=0, logical_cycle=0), dict(kind=1, logical_cycle=rounded),
                dict(kind=2, logical_cycle=rounded), dict(kind=1, logical_cycle=rounded + 100)]
        observations = lambda: iter([((1, len(rows), 0, 1, 1), rows)])
        return observations, [start, end]

    def test_source_float_rounding_and_zero_duration_output(self):
        for initial in (0.49, 0.5, 1.25, 1.5):
            observations, clocks = self.clock_case(initial)
            result = verified_clock(observations(), clocks, [1, 1])
            self.assertEqual(result['exact_predicted_output_boundaries'], 1)
            self.assertEqual(result['final_countdown_bits'], clocks[-1]['next_output_bits'])

    def test_source_phase_endpoint_interval_and_output_controls_reject(self):
        observations, clocks = self.clock_case()
        self.assertEqual(len(controls(observations, clocks, [1, 1])), 6)

    def test_incomplete_or_reordered_clock_context_is_rejected(self):
        observations, clocks = self.clock_case()
        for changed in (clocks[:-1], clocks[::-1], clocks + [clocks[1]]):
            with self.assertRaises(AssertionError): verified_clock(observations(), changed, [1, 1])

    def test_clock_metadata_uses_shared_budget_and_rejects_missing_pointer(self):
        storage = C.create_string_buffer(CLOCK.pack(*self.values()))
        writer = AudioSamplesWriter.__new__(AudioSamplesWriter)
        writer.clock_snapshots = []
        writer.engine = SimpleNamespace(audio_capture_bytes=0, audio_capture_budget=10000)
        def read(pointer):
            C.cast(pointer, C.POINTER(C.c_void_p))[0] = C.addressof(storage)
            return 1
        writer.clock_read = read
        writer.read_clock(1, 'before')
        self.assertGreater(writer.engine.audio_capture_bytes, CLOCK.size)
        writer.engine.audio_capture_budget = writer.engine.audio_capture_bytes
        with self.assertRaisesRegex(RuntimeError, 'budget'): writer.read_clock(1, 'after')
        self.assertEqual(len(writer.clock_snapshots), 1)
        writer.clock_read = lambda pointer: 0
        with self.assertRaisesRegex(RuntimeError, 'Missing'): writer.read_clock(1, 'after')


if __name__ == '__main__':
    unittest.main()
