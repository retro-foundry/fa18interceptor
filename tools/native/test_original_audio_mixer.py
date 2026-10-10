"""Logical event times and complete averaging budgets cannot be fitted."""
import copy
import io
import unittest

from check_original_audio_mixer import verify_timeline, mixer_frames, rejection_controls
from original_audio_samples import MIXER_MAGIC, MIXER, FRAME, FOOTER, mixer_records


class MixerTimelineContract(unittest.TestCase):
    def rows(self):
        def row(kind, logical, duration, values, times):
            return dict(service_cycle=12000, logical_cycle=logical, duration=duration,
                        values=values, times=times, kind=kind)
        return [row(0, 8192, 0, [-5, 0, 0, 0], [2] * 4),
                row(1, 9216, 2, [-3, 7, 0, 0], [2] * 4),
                row(2, 9216, 0, [-2, 3, 0, 0], [4] * 4),
                row(3, 9216, 0, [-3, 0, 0, 0], [0] * 4)]

    def verify(self, rows):
        return verify_timeline([(1, rows, [(12000, 0x100, 0xfd, -3, 0, 3, 0, 0)], 1)])

    def test_actual_interval_units_and_signed_truncation(self):
        result = self.verify(self.rows())
        self.assertEqual(result['exact_output_averages'], 1)
        self.assertEqual(result['exact_consumed_transitions'], 1)
        self.assertEqual(result['final_accumulators'], [0] * 4)

    def test_every_defined_record_control_rejects(self):
        observations = lambda: [(1, self.rows(), [(12000, 0x100, 0xfd, -3, 0, 3, 0, 0)], 1)]
        self.assertEqual(len(rejection_controls(observations)), 7)

    def test_changed_duration_average_budget_and_clock_are_rejected(self):
        for index, key, value in ((1, 'duration', 3), (1, 'logical_cycle', 9215),
                                  (2, 'values', [-3, 3, 0, 0]), (2, 'times', [3] * 4),
                                  (3, 'logical_cycle', 9215), (3, 'values', [-2, 0, 0, 0])):
            rows = self.rows(); rows[index][key] = value
            with self.subTest(index=index, key=key), self.assertRaises(AssertionError):
                self.verify(rows)

    def test_initial_or_output_or_sample_cannot_be_omitted(self):
        for index in (0, 2, 3):
            rows = self.rows(); del rows[index]
            with self.subTest(index=index), self.assertRaises(AssertionError):
                self.verify(rows)
        rows = self.rows(); rows.insert(1, copy.deepcopy(rows[0]))
        with self.assertRaises(AssertionError): self.verify(rows)

    def payload(self):
        return b''.join(MIXER.pack(r['service_cycle'], r['logical_cycle'], r['duration'],
                                 *r['values'], *r['times'], r['kind']) for r in self.rows())

    def test_record_framing_cycles_and_sample_channel(self):
        payload = self.payload()
        self.assertEqual(list(mixer_records(payload, 10, 30)), self.rows())
        for changed in (payload[:-1], payload + b'x',
                        MIXER.pack(12000, 12001, 0, *([0] * 8), 0),
                        MIXER.pack(12000, 9216, 0, 0, 1, 0, 0, *([0] * 4), 3),
                        MIXER.pack(12000, 9216, 1, *([0] * 8), 2)):
            with self.assertRaises(ValueError): list(mixer_records(changed, 10, 30))

    def test_lost_call_and_terminal_count_are_rejected(self):
        stream = MIXER_MAGIC + b'\1' + FRAME.pack(1, 4, 10, 30, 1) + self.payload() + b'\0' + FOOTER.pack(1, 4)
        self.assertEqual(len(list(mixer_frames(io.BytesIO(stream), 1))), 1)
        for changed in (stream[:-1], stream + b'x', stream[:-FOOTER.size] + FOOTER.pack(1, 3)):
            with self.assertRaises((AssertionError, ValueError)): list(mixer_frames(io.BytesIO(changed), 1))


if __name__ == '__main__':
    unittest.main()
