"""Exercise disk-backed cockpit artwork, immutable images and populated drawing."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def field(data, address, length):
    offset = address if address < 0x80000 else address - 0xC00000 + 0x80000
    return data[offset:offset+length]


def value(data, address, width=4):
    return int.from_bytes(field(data, address, width), 'big')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--source-initial', type=Path)
    args = parser.parse_args()
    adf = ROOT / 'local/media/fa18.adf'
    seal = hashlib.sha256(adf.read_bytes()).digest()
    oracles = {}
    for name in ('cockpit_assets', 'hud'):
        target = ROOT / f'build/recomp/native_{name}_oracle.exe'
        subprocess.run(['python', 'scripts/build_recomp.py', '--output', str(target.relative_to(ROOT)),
                        '--main', f'tools/native/native_{name}_oracle.c'], cwd=ROOT, check=True)
        oracles[name] = target
    with tempfile.TemporaryDirectory(prefix='native-cockpit-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        warmup = work / 'intro.e9k'
        warmup.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        initial_path = work / 'initial.dat'
        subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '1', '--adf', str(adf),
                        '--save-dir', str(work / 'initial-pilot'), '--data-out', str(initial_path)],
                       cwd=ROOT, check=True, capture_output=True, text=True, timeout=10)
        initial = initial_path.read_bytes()
        reference = args.source_initial.read_bytes() if args.source_initial else None
        for slot, header, cache, width, height in ((0xC1AB08, 0xC1AB0C, 0xC1AB20, 320, 55),
                                                 (0xC1AB38, 0xC1AB3C, 0xC1AB50, 288, 12)):
            assert value(initial, slot) and value(initial, header, 2) == width
            assert value(initial, header+2, 2) == height
            for plane in range(4):
                pixels = field(initial, value(initial, cache+4*plane), width*height//8)
                assert any(pixels), 'cockpit image plane is empty'
                if reference:
                    assert pixels == field(reference, value(reference, cache+4*plane), len(pixels)), 'original cockpit plane mismatch'
            if reference:
                assert field(initial, header, 20) == field(reference, header, 20), 'original BMHD mismatch'
        mask = value(initial, 0xC1AB34)
        assert any(field(initial, mask, 432)), 'panel mask missing'
        if reference:
            assert field(initial, mask, 432) == field(reference, value(reference, 0xC1AB34), 432)
        # Hunk 62 is the immutable mode/mark/compass image package. Its source
        # payload must survive clearing, drawing and swapping both display pages.
        embedded = field(initial, 0x12988, 464)
        assert any(embedded), 'embedded instrument images were cleared during startup'
        if reference:
            assert embedded == field(reference, 0x12988, 464)
        subprocess.run([str(oracles['cockpit_assets']), str(initial_path)], cwd=ROOT, check=True, timeout=15)
        for iterations in (2400, 3000):
            path = work / f'{iterations}.dat'
            result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '30000',
                '--adf', str(adf), '--save-dir', str(work / f'pilot-{iterations}'),
                '--input', str(ROOT / 'captures/native/demo01/input.fa18in'), '--iterations', str(iterations),
                '--replay', str(warmup), '--data-out', str(path)],
                cwd=ROOT, check=True, capture_output=True, text=True, timeout=35)
            stats = json.loads(result.stdout)
            assert stats['hud_frames'] > 0 and stats['stage'] == 'C10DAE', stats
            assert not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
            data = path.read_bytes()
            assert field(data, 0x12988, 464) == embedded, 'rendering overwrote instrument images'
            assert field(data, 0x64000, 0x4000) == field(initial, 0x64000, 0x4000), 'rendering overwrote cockpit assets'
            subprocess.run([str(oracles['hud']), str(path)], cwd=ROOT, check=True, timeout=20)
            print(f'{iterations}: populated cockpit; {stats["hud_frames"]} HUD/panel passes')
    assert hashlib.sha256(adf.read_bytes()).digest() == seal
    print('Disk-backed cockpit artwork, image preservation and populated source drawing pass')


if __name__ == '__main__':
    main()
