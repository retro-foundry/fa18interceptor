"""Validate original sample requests and actual native stereo PCM publication.

Uses a targeted handler oracle and native menu/selection, never a full original
replay. Hardware fetch/filter latency and bit-exact recorded audio remain open.
"""
import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile
import wave

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    runner = parser.parse_args().runner.resolve()
    oracle = ROOT / 'build/recomp/native_sample_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_sample_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-samples-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'intro.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 49 0 0 1\nF 3002 K 49 0 0 0\n')
        output, data = work / 'selection.wav', work / 'selection.dat'
        result = subprocess.run([str(runner), '--headless', '--frames', '3100',
                                 '--replay', str(replay), '--save-dir', str(work / 'pilot'),
                                 '--wav', str(output), '--data-out', str(data)],
                                cwd=ROOT, check=True, capture_output=True, text=True, timeout=20)
        stats = json.loads(result.stdout)
        assert stats['sample_frames'] == stats['frames'] * 960, stats
        assert stats['sample_requests'] > 0 and stats['nonzero_sample_frames'] > 0, stats
        assert stats['voice_ticks'] == stats['frames'] and not stats['audio_device'], stats
        assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        with wave.open(str(output), 'rb') as audio:
            assert (audio.getnchannels(), audio.getsampwidth(), audio.getframerate()) == (2, 2, 48000)
            assert audio.getnframes() == stats['sample_frames']
            pcm = audio.readframes(audio.getnframes())
            assert len(pcm) == stats['sample_frames'] * 4
            nonzero = sum(pcm[i:i+4] != b'\0\0\0\0' for i in range(0, len(pcm), 4))
            assert nonzero == stats['nonzero_sample_frames'], (nonzero, stats)
        subprocess.run([str(oracle), str(data)], cwd=ROOT, check=True, timeout=20)
        # Exercise SDL device creation and queued PCM without relying on CI's
        # speaker availability. This is a host API check, not an audio oracle.
        environment = dict(os.environ, SDL_AUDIODRIVER='dummy')
        result = subprocess.run([str(runner), '--frames', '120', '--save-dir', str(work / 'device')],
                                cwd=ROOT, check=True, capture_output=True, text=True,
                                timeout=10, env=environment)
        device = json.loads(result.stdout)
        assert device['audio_device'] and device['nonzero_sample_frames'] > 0, device
        result = subprocess.run([str(runner), '--headless', '--frames', '1',
                                 '--save-dir', str(work / 'bad-wave'),
                                 '--wav', str(work / 'missing' / 'capture.wav')],
                                cwd=ROOT, capture_output=True, text=True, timeout=10)
        assert result.returncode != 0 and 'Cannot create PCM capture' in result.stderr
    print('Native disk-backed stereo WAV, SDL publication, source sample handler and invalid-output failure pass; bit-exact audio timing remains open')


if __name__ == '__main__':
    main()
