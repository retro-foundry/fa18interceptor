"""Check original menu sample loading, descriptor parents and demo startup timing."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

from check_demo import field, value

ROOT = Path(__file__).resolve().parents[2]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--source-initial', type=Path)
    parser.add_argument('--source-checkpoint', type=Path)
    args = parser.parse_args()
    adf = ROOT / 'local/media/fa18.adf'
    seal = hashlib.sha256(adf.read_bytes()).digest()
    oracles = {}
    for name in ('sound_resources', 'demo', 'records'):
        target = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(target.relative_to(ROOT)),
                        '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        oracles[name] = target
    with tempfile.TemporaryDirectory(prefix='native-sound-resources-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        initial_path = work / 'initial.dat'
        subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '1', '--adf', str(adf),
                        '--save-dir', str(work / 'initial-pilot'), '--data-out', str(initial_path)],
                       cwd=ROOT, check=True, capture_output=True, text=True, timeout=10)
        initial = initial_path.read_bytes()
        reference = args.source_initial.read_bytes() if args.source_initial else None
        assert value(initial, 0xC45B5B, 1) & 0x80, 'menu sample availability missing'
        assert value(initial, 0xC45B5A, 1) & 4, 'intro sample pair missing'
        for slot, length in ((35, 568), (13, 2044), (15, 2012), (17, 32770),
                             (19, 32786), (25, 32914), (27, 32720), (33, 65590)):
            voice = value(initial, 0xC0A438+4*slot, 4)
            samples = field(initial, value(initial, voice, 4), length)
            assert value(initial, voice+4, 4) == length and any(samples)
            duplicate = value(initial, 0xC0A438+4*(slot+1), 4)
            assert value(initial, duplicate, 4) == value(initial, voice, 4), 'duplicate sample storage differs'
            assert value(initial, duplicate+4, 4) == (length | 0x80000000)
            if reference:
                source_voice = value(reference, 0xC0A438+4*slot, 4)
                assert samples == field(reference, value(reference, source_voice, 4), length), f'original sound {slot} mismatch'
        subprocess.run([str(oracles['sound_resources']), str(initial_path)], cwd=ROOT, check=True, timeout=20)
        for iterations in (2400, 4892):
            path = work / f'{iterations}.dat'
            result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '30000',
                '--adf', str(adf), '--save-dir', str(work / f'pilot-{iterations}'),
                '--input', str(ROOT / 'captures/native/demo01/input.fa18in'), '--iterations', str(iterations),
                '--replay', str(warmup), '--data-out', str(path)],
                cwd=ROOT, check=True, capture_output=True, text=True, timeout=35)
            stats = json.loads(result.stdout)
            assert stats['mode'] == 3 and stats['stage'] == 'C10DAE' and stats['hud_frames'] > 0, stats
            assert stats['replay_iterations'] == iterations and stats['replay_events'] == 2, stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            if iterations == 2400:
                subprocess.run([str(oracles['demo']), str(path)], cwd=ROOT, check=True, timeout=20)
                if args.source_checkpoint:
                    source = args.source_checkpoint.read_bytes()
                    # Source start 2401 versus native end 2400. Pending timers
                    # remain visible; no source clock is injected into the game.
                    gap = stats['game_tick']-value(source, 0xC458DA, 2)
                    assert gap == 37, f'new startup timing debt: {gap} ticks'
                    print(f'Demo startup lead is {gap} game ticks (previously 97)')
            else:
                log = value(path.read_bytes(), 0xC1AB74, 4)
                assert value(path.read_bytes(), log+62, 2) > value(initial, log+62, 2), 'recorded launches absent'
            subprocess.run([str(oracles['records']), str(path)], cwd=ROOT, check=True, timeout=20)
    assert hashlib.sha256(adf.read_bytes()).digest() == seal
    print('Menu sound resources, original descriptor contracts and full native demo pass; audio output remains pending')


if __name__ == '__main__':
    main()
