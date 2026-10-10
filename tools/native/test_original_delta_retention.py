"""Interrupted original recordings remain rejected and survive temp cleanup."""
import gzip
import hashlib
from pathlib import Path
import tempfile
import unittest

from check_original_frame_delta import retain_failed_capture


class OriginalDeltaRetention(unittest.TestCase):
    def test_interrupted_bytes_are_retained_without_accepting_a_footer(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory);output = root/'output'
            with tempfile.TemporaryDirectory(dir=root) as temporary:
                work = Path(temporary)
                data = b'FA18_FRAME_DELTA_V1\n'+bytes(range(120))
                (work/'frames.delta').write_bytes(data)
                (work/'trace.jsonl').write_bytes(b'{"iteration":1}\n{"iteration":')
                (work/'registers.jsonl').write_bytes(b'')
                report = retain_failed_capture(work,output,'actual process limit')
                self.assertFalse(report['accepted_evidence'])
                self.assertEqual(len(report['retained']),3)
                for row in report['retained']:
                    recovered = gzip.decompress((output/'failure'/row['file']).read_bytes())
                    self.assertEqual(len(recovered),row['decoded_bytes'])
                    self.assertEqual(hashlib.sha256(recovered).hexdigest(),row['decoded_sha256'])
            self.assertEqual(gzip.decompress((output/'failure/frames.delta.gz').read_bytes()),data)
            self.assertFalse((output/'report.json').exists())

    def test_failure_before_any_output_does_not_create_capture_claims(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            report = retain_failed_capture(root,root/'output','probe did not initialize')
            self.assertFalse(report['accepted_evidence'])
            self.assertEqual(report['retained'],[])


if __name__ == '__main__':
    unittest.main()
