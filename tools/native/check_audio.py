"""Compare C50158 voices and exercise their PAL callback in the native runner.

No original full replay is run. Sample requests and PCM publication have their
own check_samples.py boundary.
"""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

from check_demo import value

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    runner = parser.parse_args().runner.resolve()
    oracle = ROOT / 'build/recomp/native_audio_oracle.exe'
    subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(oracle.relative_to(ROOT)),
                    '--main', 'tools/native/native_audio_oracle.c'], cwd=ROOT, check=True)
    with tempfile.TemporaryDirectory(prefix='native-audio-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'intro.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n'
                          'F 3000 K 49 0 0 1\nF 3002 K 49 0 0 0\n')
        snapshots = []
        for frames in (3001, 3002, 3008):
            path = work / f'{frames}.dat'
            result = subprocess.run([str(runner), '--headless', '--frames', str(frames),
                                     '--replay', str(replay), '--save-dir', str(work / str(frames)),
                                     '--data-out', str(path)], cwd=ROOT, check=True,
                                    capture_output=True, text=True, timeout=15)
            stats, data = json.loads(result.stdout), path.read_bytes()
            assert stats['voice_ticks'] == frames and stats['voice_publications'] > 0, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            snapshots.append((stats, data))
            if frames == 3001:
                subprocess.run([str(oracle), str(path)], cwd=ROOT, check=True, timeout=20)
        first, second, finished = snapshots
        assert first[0]['display_pending'] and second[0]['display_pending']
        for key in ('update_iterations', 'record_updates', 'hud_frames', 'glyphs', 'game_tick'):
            assert first[0][key] == second[0][key], (key, first[0], second[0])
        assert second[0]['voice_publications'] == first[0]['voice_publications'] + 3
        # C1718E fades first: C501E0 then publishes the new master high word.
        assert first[0]['voice_levels'][:2] == [[358, 31], [358, 31]], first[0]
        assert second[0]['voice_levels'][:2] == [[358, 30], [358, 30]], second[0]
        voice = value(first[1], 0xC0A448, 4)  # square-wave tone, SOUND_VOICES[4]
        assert value(first[1], 0xC4FE44, 4) == voice
        assert value(second[1], voice + 0x34, 4) > value(first[1], voice + 0x34, 4)
        assert value(finished[1], 0xC4FE44, 4) == 0, 'native tone failed to release channel 3'
        assert value(finished[1], voice + 0x2C, 4) == 0
        assert value(finished[1], voice + 0x34, 4) == 96
        assert value(finished[1], voice + 0x24, 4) == 0
        assert value(finished[1], voice + 0x28, 4) == 0
    print('Native PAL voice programs advance during display waits, observe faded volume and release completed tones')


if __name__ == '__main__':
    main()
