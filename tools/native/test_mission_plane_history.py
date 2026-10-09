"""An incomplete diagnostic must never become complete-flight acceptance."""
import copy
import unittest

from assess_mission_plane_history import coverage


class PlaneHistoryAcceptance(unittest.TestCase):
    def setUp(self):
        self.source = {i: dict(pages_valid=True, records=['complete core'], pages=['same'] * 8)
                       for i in range(3)}
        self.native = copy.deepcopy(self.source)
        self.mapping = {i: i for i in self.source}
        self.native[1]['pages'][3] = 'different draw plane'
        self.native[2]['pages'][7] = 'different display plane'
        self.histories = {1: dict(planes=[3, 7], difference_bytes=[1, 0], history='certified')}

    def test_default_gate_rejects_missing_display_history(self):
        with self.assertRaisesRegex(AssertionError, 'Missing bounded history'):
            coverage(self.source, self.native, self.mapping, 0, 2, self.histories, 3)

    def test_audit_preserves_open_observation_and_counts_only_proved_planes(self):
        result = coverage(self.source, self.native, self.mapping, 0, 2, self.histories, 3, audit=True)
        self.assertFalse(result['complete_plane_history_matching'])
        self.assertEqual(result['unresolved_observations'],
                         [dict(iteration=2, native_iteration=2, planes=[7])])
        # Six page-plane observations: four exact, one certified, one open.
        self.assertEqual(result['complete_plane_bytes_accounted'], 40000)
        self.assertEqual(result['complete_plane_observations'], 6)

    def test_complete_certificates_pass_without_changing_accounted_bytes(self):
        self.histories[2] = dict(planes=[3, 7], difference_bytes=[0, 1], history='certified display')
        result = coverage(self.source, self.native, self.mapping, 0, 2, self.histories, 3)
        self.assertEqual(result['complete_plane_bytes_accounted'], 48000)
        self.assertEqual(len(result['different_observations']), 2)

    def test_audit_rejects_certificate_that_contradicts_actual_page(self):
        self.histories[1]['difference_bytes'] = [0, 0]
        with self.assertRaises(AssertionError):
            coverage(self.source, self.native, self.mapping, 0, 2, self.histories, 3, audit=True)

    def test_audit_keeps_complete_record_cores_strict(self):
        self.native[0]['records'][0] = 'corrupt core'
        with self.assertRaisesRegex(AssertionError, 'Complete record cores differ'):
            coverage(self.source, self.native, self.mapping, 0, 2, self.histories, 3, audit=True)


if __name__ == '__main__':
    unittest.main()
