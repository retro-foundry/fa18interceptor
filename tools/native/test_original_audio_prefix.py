"""Reject damaged prefix PCM, event coverage, DMA and endpoint state."""
import json
from pathlib import Path
import tempfile
import unittest
import wave

from check_original_audio_dma import MAGIC, FRAME, FETCH, FOOTER
from check_original_audio_prefix import validate_prefix, retain, sha


class AudioPrefixContract(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.prefix, self.complete = self.root / 'prefix', self.root / 'complete'
        hardware = b'CIAA' + (16).to_bytes(4, 'big') + bytes(4) + bytes([0xc0, 0, 3, 0])
        for channel in range(4):
            raw = bytearray(25)
            raw[3] = 128
            raw[8:10] = (124).to_bytes(2, 'big')
            raw[20:24] = bytes([255])*4
            hardware += f'AUD{channel}'.encode() + (37).to_bytes(4, 'big') + bytes(4) + raw + bytes(3)
        hardware += b'END ' + bytes(8)
        for folder, calls in ((self.prefix, 1), (self.complete, 2)):
            folder.mkdir()
            (folder / 'state.bin').write_bytes(hardware)
            (folder / 'chip.bin').write_bytes(b'\x21\xfd\0\0')
            with wave.open(str(folder / 'original.wav'), 'wb') as file:
                file.setparams((2, 2, 44100, 0, 'NONE', 'not compressed'))
                file.writeframes(bytes(range(calls*4)))
            (folder / 'audio_chunks.jsonl').write_text(''.join(
                json.dumps(dict(call=call, first_sample=call-1, frames=1))+'\n' for call in range(1, calls+1)))
            events = [dict(kind='initial', call=0)] + [dict(kind='boundary', call=call) for call in range(1, calls+1)]
            (folder / 'audio_events.jsonl').write_text(''.join(json.dumps(row)+'\n' for row in events))
            stream = MAGIC
            for call in range(1, calls+1):
                stream += b'\1'+FRAME.pack(call,call,call,0,288,1000,0,288000,1,call)
                stream += FETCH.pack(21,0,0x21fd,0xca,21,0,2)
            stream += b'\0'+FOOTER.pack(calls,calls)
            (folder / 'audio_dma.bin').write_bytes(stream)
            snapshot = dict(frame=calls, hardware_frame=calls, core_options={},
                authority=dict(core_sha256='core',config_sha256='config',initial_state_sha256='initial',
                               recording_sha256='input',snapshot_sha256=sha(hardware)),
                memory=[dict(file='chip.bin',sha256=sha(b'\x21\xfd\0\0'))],
                recorded_audio=dict(first_replay_call=1,replay_calls=calls,sample_frames=calls,file='original.wav',
                    pcm_sha256=sha(bytes(range(calls*4))),wav_sha256=sha((folder / 'original.wav').read_bytes())),
                audio_events=dict(sha256=sha((folder / 'audio_events.jsonl').read_bytes())),
                audio_dma=dict(file='audio_dma.bin',calls=calls,fetched_words=calls,sha256=sha(stream)))
            (folder / 'snapshot.json').write_text(json.dumps(snapshot))

    def check(self):
        return validate_prefix(self.prefix, self.complete, self.complete / 'original.wav')

    def rewrite(self, change):
        path = self.prefix / 'snapshot.json'
        row = json.loads(path.read_text())
        change(row)
        path.write_text(json.dumps(row))

    def test_exact_prefix_and_compressed_retention(self):
        result = self.check()
        self.assertEqual(result['calls'], 1)
        self.assertEqual(result['actual_dma_words'], 1)
        retain(self.prefix, self.complete / 'original.wav')
        self.assertEqual(self.check()['endpoint_hardware'], result['endpoint_hardware'])
        self.assertFalse((self.prefix / 'original.wav').exists())

    def test_changed_pcm_rejected_with_rewritten_local_hashes(self):
        with wave.open(str(self.prefix / 'original.wav'), 'wb') as file:
            file.setparams((2, 2, 44100, 0, 'NONE', 'not compressed'))
            file.writeframes(b'abcd')
        self.rewrite(lambda row: row['recorded_audio'].update(pcm_sha256=sha(b'abcd'),
            wav_sha256=sha((self.prefix / 'original.wav').read_bytes())))
        with self.assertRaises(AssertionError):
            self.check()

    def test_lost_events_rejected_with_rewritten_local_hash(self):
        (self.prefix / 'audio_events.jsonl').write_text('')
        self.rewrite(lambda row: row['audio_events'].update(sha256=sha(b'')))
        with self.assertRaises(AssertionError):
            self.check()

    def test_changed_dma_word_rejected_with_rewritten_local_hash(self):
        path = self.prefix / 'audio_dma.bin'
        data = bytearray(path.read_bytes())
        data[len(MAGIC)+1+FRAME.size+8] ^= 1
        path.write_bytes(data)
        self.rewrite(lambda row: row['audio_dma'].update(sha256=sha(data)))
        with self.assertRaises(AssertionError):
            self.check()

    def test_changed_endpoint_state_rejected(self):
        path = self.prefix / 'state.bin'
        path.write_bytes(path.read_bytes()+b'x')
        with self.assertRaises(AssertionError):
            self.check()

    def test_changed_authority_rejected(self):
        self.rewrite(lambda row: row['authority'].update(core_sha256='different'))
        with self.assertRaises(AssertionError):
            self.check()

    def test_lost_pcm_chunk_rejected(self):
        (self.prefix / 'audio_chunks.jsonl').write_text('')
        with self.assertRaises(AssertionError):
            self.check()


if __name__ == '__main__':
    unittest.main()
