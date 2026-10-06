"""Compare an aligned independent native gameplay checkpoint with existing source RAM.

Intro/loading duration and Copper fade are outside this drawing comparison.
The native process starts from disk/input; reference RAM is read only here.
This proves one checkpoint, never a complete recorded gameplay sequence.
"""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def span(data, address, size):
    offset = address if address < 0x80000 else address - 0xC00000 + 0x80000
    assert 0 <= offset <= 0x100000 - size, (hex(address), size)
    return data[offset:offset + size]


def integer(data, address, size):
    return int.from_bytes(span(data, address, size), 'big')


def compare_gameplay(source, native):
    """Compare fixed phase alignment, complete source pages and named flight fields."""
    assert len(source) in (0x100000, 0x100048), 'expected original RAM export'
    assert len(native) == 0x100000, 'expected native RAM export'
    differences = []
    for address, size in ((0xC1820C, 4), (0xC458DA, 2), (0xC4566C, 2), (0xC458DE, 2),
                          (0xC45776, 4), (0xC4582E, 3)):
        original, actual = span(source, address, size), span(native, address, size)
        if original != actual:
            differences.append(f'boundary/control {address:06X}: {original.hex()} != {actual.hex()}')
    # Geometry comes from the original ViewPort, never a similarity crop.
    width = integer(source, 0xC1822A + 0x18, 2)
    height = integer(source, 0xC1822A + 0x1A, 2)
    assert (width, height) == (320, 200), (width, height)
    plane_bytes = ((width + 15) // 16) * 2 * height
    for page in range(2):
        for plane in range(4):
            address = 0xC4566E + 16 * page + 4 * plane
            original = span(source, integer(source, address, 4), plane_bytes)
            actual = span(native, integer(native, address, 4), plane_bytes)
            changed = [i for i, (a, b) in enumerate(zip(original, actual)) if a != b]
            if changed:
                first = changed[0]
                differences.append(f'page {page} plane {plane}: {len(changed)} differing bytes; '
                                   f'first byte {first} (x={first % 40 * 8}, y={first // 40}) '
                                   f'{original[first]:02X} != {actual[first]:02X}')
    # Explicit motion/pose/matrix scope; flags, counters and async voices are separate.
    for name, begin, end in (('position and motion', 0x0C, 0x26), ('rates', 0x38, 0x4A),
                             ('orientation', 0x66, 0x74), ('matrices', 0x80, 0xA4)):
        address = 0xC46184 + begin
        original, actual = span(source, address, end - begin), span(native, address, end - begin)
        if original != actual:
            changed = [f'+{begin + i:02X}:{a:02X}!={b:02X}'
                       for i, (a, b) in enumerate(zip(original, actual)) if a != b]
            differences.append(f'{name}: {", ".join(changed)}')
    return differences


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runner', type=Path, default=ROOT / 'build/native/fa18_native.exe')
    parser.add_argument('--input', type=Path, default=ROOT / 'captures/native/demo01/input.fa18in')
    parser.add_argument('--source', type=Path, required=True)
    parser.add_argument('--iteration', type=int, required=True,
                        help='native gameplay update at the equivalent source boundary')
    args = parser.parse_args()
    if args.iteration <= 0:
        parser.error('--iteration must be positive')
    source = args.source.read_bytes()
    assert len(source) in (0x100000, 0x100048), 'expected original RAM export'
    with tempfile.TemporaryDirectory(prefix='native-gameplay-checkpoint-', dir=ROOT / 'build') as directory:
        work = Path(directory)
        replay = work / 'intro.e9k'
        replay.write_text('E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n')
        prefix = work / 'gameplay'
        result = subprocess.run([str(args.runner.resolve()), '--headless', '--frames', '30000',
                                 '--input', str(args.input.resolve()), '--iterations', str(args.iteration + 1),
                                 '--replay', str(replay), '--save-dir', str(work / 'pilot'),
                                 '--frame-capture', str(args.iteration), str(prefix)],
                                cwd=ROOT, check=True, capture_output=True, text=True, timeout=35)
        stats = json.loads(result.stdout)
        assert stats['frame_capture_complete'] and not stats['cpu_emulation'] and not stats['chipset_emulation'], stats
        native = Path(str(prefix) + '.before.dat').read_bytes()
        differences = compare_gameplay(source, native)
        assert not differences, '\n'.join(differences)
        print(f'Aligned gameplay checkpoint: game tick {stats["frame_saved_tick"]}, native update {args.iteration}; '
              'both 320x200 four-plane pages and player motion/pose/matrices match original bytes')
    print('One independent-run gameplay checkpoint accepted; complete sequence and time alignment remain unverified')


if __name__ == '__main__':
    main()
