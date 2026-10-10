"""Whole-flight drawing evidence must keep bytes, roles and identities strict."""
import copy
import gzip
from pathlib import Path
import tempfile
import unittest

from check_complete_cockpit_history import align_history, history_difference, verified_native, shared_scene_geometry
from check_mission_message_pages import predicted_pages
from check_qualification_message_cadence import pages, instrument_panel_refresh
from frame_delta import bodies
from check_original_frame_delta import verify_owner_return
from test_frame_delta import fixture


def put(data, address, value, size):
    offset = address if address < 0x80000 else address - 0xC00000 + 0x80000
    data[offset:offset + size] = (value & ((1 << (size * 8)) - 1)).to_bytes(size, 'big')


def screen():
    data = bytearray(0x100000)
    put(data, 0xC456B6, 0xC4566E, 4)
    for plane in range(8):
        put(data, 0xC4566E + 4 * plane, 0x10000 + plane * 0x2000, 4)
    return data


class CompleteCockpitAcceptance(unittest.TestCase):
    def test_owned_scene_bits_are_not_repaired_from_input_pixels(self):
        data, expected, draw = screen(), {}, {}
        align_history(expected, draw, dict(source=data, native=data), True, copy_scene=False)
        changed = bytearray(data)
        changed[0x10000 + 4 * 0x2000 + 5119] = 1
        live = align_history(expected, draw, dict(source=data, native=changed), False, copy_scene=False)
        difference, = history_difference(live, expected)
        self.assertEqual((difference['plane'], difference['first_byte']), (4, 5119))

    def test_fresh_geometry_requires_every_active_scene_byte_to_match(self):
        data, expected, draw = screen(), {}, {}
        align_history(expected, draw, dict(source=data, native=data), True, copy_scene=False)
        changed = bytearray(data)
        changed[0x10000 + 3 * 0x2000 + 5119] = 1
        with self.assertRaisesRegex(AssertionError, 'Fresh active scene geometry differs'):
            shared_scene_geometry(expected, dict(source=data, native=changed))

    def test_fresh_geometry_preserves_inactive_headup_and_cockpit_history(self):
        data, expected, draw = screen(), {}, {}
        align_history(expected, draw, dict(source=data, native=data), True, copy_scene=False)
        expected['source'][4][5119] = 3
        expected['native'][4][5119] = 7
        expected['source'][0][7900] = 5
        changed = bytearray(data)
        changed[0x10000 + 5119] = 9
        shared_scene_geometry(expected, dict(source=changed, native=changed))
        self.assertEqual((expected['source'][0][5119], expected['native'][0][5119]), (9, 9))
        self.assertEqual((expected['source'][4][5119], expected['native'][4][5119]), (3, 7))
        self.assertEqual(expected['source'][0][7900], 5)

    def test_display_cockpit_byte_cannot_be_masked(self):
        data, expected, draw = screen(), {}, {}
        align_history(expected, draw, dict(source=data, native=data), True)
        changed = bytearray(data)
        changed[0x10000 + 7 * 0x2000 + 7999] = 1
        live = align_history(expected, draw, dict(source=data, native=changed), False)
        difference, = history_difference(live, expected)
        self.assertEqual((difference['plane'], difference['first_byte']), (7, 7999))

    def test_shared_scene_values_are_checked_before_copying(self):
        data, expected, draw = screen(), {}, {}
        align_history(expected, draw, dict(source=data, native=data), True)
        changed = bytearray(data)
        changed[0x10000 + 7 * 0x2000 + 5119] = 1
        with self.assertRaisesRegex(AssertionError, 'Scene rows differ'):
            align_history(expected, draw, dict(source=data, native=changed), False)

    def test_history_cannot_start_on_different_cockpit_pages(self):
        data, changed = screen(), screen()
        changed[0x10000 + 7900] = 1
        with self.assertRaisesRegex(AssertionError, 'common complete initial'):
            align_history({}, {}, dict(source=data, native=changed), True)

    def test_page_swap_carries_previous_writes_into_display_role(self):
        data, expected, draw = screen(), {}, {}
        align_history(expected, draw, dict(source=data, native=data), True)
        expected['source'][0][7900] = 1
        changed = bytearray(data)
        changed[0x10000 + 7900] = 1
        for value in (data, changed):
            put(value, 0xC4566C, 1, 2)
            put(value, 0xC456B6, 0xC4567E, 4)
        live = align_history(expected, draw, dict(source=changed, native=data), False)
        self.assertEqual(expected['source'][4][7900], 1)
        self.assertEqual(history_difference(live, expected), [])

    def test_self_hashed_native_snapshot_still_needs_sealed_identity(self):
        with tempfile.TemporaryDirectory() as directory:
            directory = Path(directory)
            path = directory / 'frames.delta.gz'
            path.write_bytes(gzip.compress(fixture()))
            body, = bodies(path)
            identity = dict(iteration=7, original_body_matching=True,
                before_tick=10, after_tick=12, saved_tick=23,
                **{key + '_sha256': body[key]['ram_sha256'] for key in body})
            report = dict(bodies=1, identities=[identity])
            self.assertEqual(len(list(verified_native(directory, report))), 1)
            for key in ('entry_sha256', 'before_sha256', 'after_sha256'):
                forged = copy.deepcopy(report)
                forged['identities'][0][key] = '0' * 64
                with self.subTest(key=key), self.assertRaisesRegex(AssertionError, 'sealed snapshot identity'):
                    list(verified_native(directory, forged))

    def test_glyph_translation_wraps_source_add_long(self):
        before = screen()
        put(before, 0xC45861, 128, 1)  # no alternate-plane clear
        put(before, 0xC3D790, 0x100, 2)
        offset = 0xC3D890 - 0xC00000 + 0x80000
        before[offset:offset + 5] = bytes([0xE0] * 5)
        message = 0xC4580A - 0xC00000 + 0x80000
        before[message:message + 26] = bytes([32] * 26)
        for translation in (-40, 0, 40):
            put(before, 0xC45918, translation, 4)
            predicted = predicted_pages(before, before, True)
            expected = [bytearray(page) for page in pages(before)]
            for row in range(5):
                expected[0][7692 + translation + row * 40] = 0xE0
            self.assertEqual(predicted, expected)

    def test_panel_translation_keeps_source_start_and_shortens_positive_lift(self):
        data = screen()
        put(data, 0xC45836, 1, 1)
        for plane in range(4):
            pointer = 0xC60000 + 4 * plane
            image = 0x50000 + 2200 * plane
            put(data, 0xC30752 + 4 * plane, pointer, 4)
            put(data, pointer, image, 4)
            data[image:image + 2200] = bytes([plane + 1]) * 2200
        for lift in (-1, 0, 2):
            put(data, 0xC458D8, lift, 2)
            put(data, 0xC45918, 40 * lift, 4)
            result = [bytearray(page) for page in pages(data)]
            instrument_panel_refresh(data, result)
            expected = [bytearray(page) for page in pages(data)]
            start, length = 5800 + lift * 40, (55 - max(lift, 0)) * 40
            for plane in range(4):
                expected[plane][start:start + length] = bytes([plane + 1]) * length
            self.assertEqual(result, expected)

    def panel_call(self):
        data = bytearray(0x100000)
        put(data, 0x1000, 0xC0F182, 4)
        entry = dict(iteration=9, boundary=0xC30764, data=data)
        registers = dict(registers=[0] * 15 + [0x1000])
        pending = {}
        verify_owner_return(entry, registers, pending)
        return entry, pending

    def test_panel_return_requires_saved_caller_and_restored_stack(self):
        _, pending = self.panel_call()
        returned = dict(iteration=9, boundary=0xC0F182)
        self.assertEqual(verify_owner_return(returned, dict(registers=[0] * 15 + [0x1004]), pending), 1)
        self.assertEqual(pending, {})
        _, pending = self.panel_call()
        with self.assertRaisesRegex(AssertionError, 'another caller'):
            verify_owner_return(returned, dict(registers=[0] * 15 + [0x1008]), pending)

    def test_panel_capture_cannot_cross_a_body_boundary(self):
        _, pending = self.panel_call()
        with self.assertRaisesRegex(AssertionError, 'interrupted an owner'):
            verify_owner_return(dict(iteration=9, boundary=0xC0F3C0), {}, pending)

    def test_panel_entry_rejects_the_next_caller_instruction_as_return(self):
        entry, _ = self.panel_call()
        put(entry['data'], 0x1000, 0xC0F188, 4)
        with self.assertRaises(AssertionError):
            verify_owner_return(entry, dict(registers=[0] * 15 + [0x1000]), {})


if __name__ == '__main__':
    unittest.main()
