"""Inactive selected markers cannot conceal counter, contact or paint faults."""
import copy
import json
from pathlib import Path
import unittest

from assess_mission_radar_cadence import changed_paints, phase_controls, validate_controls, verify
from assess_mission_cockpit_history import validate_history_controls


class RadarControlObservability(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.fixture = json.loads((Path(__file__).parent / 'fixtures/radar_control_observability.json').read_text())

    def report(self, name):
        rows = copy.deepcopy(self.fixture[name]['rows'])
        active, inactive = phase_controls(rows)
        return dict(rows=rows, mutation_rejections=active, unobservable_phase_mutations=inactive,
                    paint_mutation_rejections=dict(lost_erase=True, wrong_marker_colour=True, wrong_head_slope=True))

    def test_actual_visible_selected_marker_keeps_all_four_controls(self):
        report = self.report('selected')
        self.assertEqual(len(report['mutation_rejections']), 4)
        self.assertEqual(report['unobservable_phase_mutations'], [])
        validate_controls(report)

    def prefix_row(self):
        fixture = Path(__file__).parent / 'fixtures/radar_prefix_selection.json'
        return json.loads(fixture.read_text())['row']

    def test_actual_selection_change_between_prefixes_passes_all_controls(self):
        row = self.prefix_row()
        self.assertEqual(row['source']['prefix_selected_records'], [4096, 5120])
        active, inactive = phase_controls([row])
        self.assertEqual(len(active), 4)
        self.assertEqual(inactive, [])

    def test_prefix_identity_selection_and_phase_cannot_be_relabelled(self):
        for key, value in (('prefix', 0), ('selected_record', 0), ('phase', 0)):
            row = self.prefix_row()
            row['source']['points'][0][key] = value
            with self.subTest(key=key), self.assertRaises(AssertionError):
                verify([row])

    def test_each_prefix_keeps_regular_contact_coordinates_strict(self):
        for prefix in (1, 2):
            row = self.prefix_row()
            point = next(p for p in row['source']['points']
                         if p['prefix'] == prefix and p['record_offset'] != p['selected_record'])
            point['x'] += 1
            with self.subTest(prefix=prefix), self.assertRaises(AssertionError):
                verify([row])

    def test_second_selection_cannot_be_replaced_by_initial_selection(self):
        row = self.prefix_row()
        row['source']['prefix_selected_records'][1] = row['source']['selected_record']
        with self.assertRaises(AssertionError):
            verify([row])

    def test_actual_no_sig_keeps_counter_and_ordinary_contact_controls(self):
        report = self.report('no_selected_marker')
        self.assertEqual(report['mutation_rejections'], dict(counter_increment=True, other_marker=True))
        self.assertEqual(set(report['unobservable_phase_mutations']), {'premature_marker', 'marker_coordinate'})
        validate_controls(report)

    def test_visible_marker_cannot_claim_unobservable_marker_controls(self):
        report = self.report('selected')
        for key in ('premature_marker', 'marker_coordinate'):
            del report['mutation_rejections'][key]
        report['unobservable_phase_mutations'] = ['premature_marker', 'marker_coordinate']
        with self.assertRaises(AssertionError):
            validate_controls(report)

    def test_no_sig_cannot_omit_counter_or_regular_contact_rejection(self):
        for key in ('counter_increment', 'other_marker'):
            report = self.report('no_selected_marker')
            del report['mutation_rejections'][key]
            with self.subTest(key=key), self.assertRaises(AssertionError):
                validate_controls(report)

    def test_no_sig_still_rejects_wrong_ordinary_contact_coordinates(self):
        report = self.report('no_selected_marker')
        report['rows'][0]['source']['points'][0]['x'] += 1
        with self.assertRaises(AssertionError):
            verify(report['rows'])

    def test_actual_colour_five_contact_supplies_the_colour_control(self):
        paints = self.fixture['no_selected_marker']['rows'][0]['source']['paints']
        changed = changed_paints(paints, 'wrong_marker_colour')
        self.assertIsNotNone(changed)
        self.assertNotEqual(changed, paints)
        self.assertEqual(paints[-1]['colour'], 5)
        self.assertEqual(changed[-1]['colour'], 8)

    def test_no_sig_cannot_relabel_missing_paint_controls(self):
        for key in ('lost_erase', 'wrong_marker_colour', 'wrong_head_slope'):
            report = self.report('no_selected_marker')
            del report['paint_mutation_rejections'][key]
            report['unobservable_paint_mutations'] = [key]
            with self.subTest(key=key), self.assertRaises(AssertionError):
                validate_controls(report)

    def test_actual_balanced_and_source_only_history_controls_pass(self):
        for name in ('selected', 'no_selected_marker'):
            validate_history_controls(self.fixture[name]['combined_history'])

    def test_source_only_control_cannot_substitute_a_different_owner(self):
        report = copy.deepcopy(self.fixture['no_selected_marker']['combined_history'])
        report['history_mutation_rejections']['lost_panel_refresh']['actual_mutation'] = 'lost_source_radar_writes'
        with self.assertRaises(AssertionError):
            validate_history_controls(report)

    def test_unobservable_balanced_control_still_needs_an_actual_rejected_probe(self):
        report = copy.deepcopy(self.fixture['no_selected_marker']['combined_history'])
        del report['history_mutation_rejections']['lost_panel_refresh']['actual_mutation']
        with self.assertRaises(AssertionError):
            validate_history_controls(report)

    def test_source_only_control_cannot_claim_a_failure_outside_its_window(self):
        report = copy.deepcopy(self.fixture['no_selected_marker']['combined_history'])
        report['history_mutation_rejections']['lost_panel_refresh']['first_difference'] = report['last'] + 1
        with self.assertRaises(AssertionError):
            validate_history_controls(report)


if __name__ == '__main__':
    unittest.main()
