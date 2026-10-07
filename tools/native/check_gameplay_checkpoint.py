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


def compare_record_state(source, native):
    """Report every byte in the sixteen $A4-byte flight-record cores.

    Include flags and countdowns. This separate scope must not be inferred
    from matching drawing pages or the narrower player kinematic check.
    Cached hull vertices following each core are renderer state, not included.
    """
    records = []
    for slot in range(16):
        address = 0xC46184 + slot * 512
        original, actual = span(source, address, 0xA4), span(native, address, 0xA4)
        changes = [f'+{offset:02X}:{a:02X}!={b:02X}'
                   for offset, (a, b) in enumerate(zip(original, actual)) if a != b]
        if changes:
            records.append(dict(slot=slot, differences=changes))
    return records


def compare_gameplay(source, native):
    """Compare fixed phase alignment, complete source pages and named flight fields."""
    assert len(source) in (0x100000, 0x100048), 'expected original RAM export'
    assert len(native) == 0x100000, 'expected native RAM export'
    differences = []
    for address, size in ((0xC1820C, 4), (0xC458DA, 2), (0xC458DE, 2),
                          (0xC45776, 4), (0xC4582E, 3)):
        original, actual = span(source, address, size), span(native, address, size)
        if original != actual:
            differences.append(f'boundary/control {address:06X}: {original.hex()} != {actual.hex()}')
    # Geometry comes from the original ViewPort, never a similarity crop.
    width = integer(source, 0xC1822A + 0x18, 2)
    height = integer(source, 0xC1822A + 0x1A, 2)
    assert (width, height) == (320, 200), (width, height)
    plane_bytes = ((width + 15) // 16) * 2 * height
    source_draw, native_draw = integer(source, 0xC4566C, 2), integer(native, 0xC4566C, 2)
    for name, data, page in (('source', source, source_draw), ('native', native, native_draw)):
        assert page in (0, 1), f'{name} has invalid draw page {page}'
        # C2F558 selects the table from DRAW_PAGE before entering C0EFD4.
        assert integer(data, 0xC456B6, 4) == 0xC4566E + 16 * page, f'{name} active drawing table differs'
    # C1612C publishes the completed page then flips DRAW_PAGE. At these
    # boundaries the opposite page is displayed. Loading may perform a
    # different number of swaps; physical buffer numbers are not gameplay.
    for role in range(2):
        source_page, native_page = source_draw ^ role, native_draw ^ role
        page_name = 'draw' if role == 0 else 'display'
        for plane in range(4):
            original_address = 0xC4566E + 16 * source_page + 4 * plane
            native_address = 0xC4566E + 16 * native_page + 4 * plane
            original = span(source, integer(source, original_address, 4), plane_bytes)
            actual = span(native, integer(native, native_address, 4), plane_bytes)
            changed = [i for i, (a, b) in enumerate(zip(original, actual)) if a != b]
            if changed:
                first = changed[0]
                differences.append(f'page {page_name} plane {plane} '
                                   f'(source {source_page}/native {native_page}): {len(changed)} differing bytes; '
                                   f'first byte {first} (x={first % 40 * 8}, y={first // 40}) '
                                   f'{original[first]:02X} != {actual[first]:02X}')
    # The attached outside camera is part of the gameplay view, including
    # its world position and the matrices consumed by model projection.
    for name, address, size in (('observer', 0xC45C32, 24), ('camera matrix', 0xC45C20, 18),
                               ('view matrix', 0xC45BD8, 18), ('view pan/rotate', 0xC45A94, 4),
                               ('view attitude', 0xC45A88, 12), ('view side', 0xC458B2, 1)):
        original, actual = span(source, address, size), span(native, address, size)
        if original != actual:
            differences.append(f'{name}: {original.hex()} != {actual.hex()}')
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
        native = Path(str(prefix) + '.entry.dat').read_bytes()
        differences = compare_gameplay(source, native)
        assert not differences, '\n'.join(differences)
        records = compare_record_state(source, native)
        print(f'Full flight-record cores (including flags/countdowns): {16 - len(records)}/16 match')
        for record in records:
            print(f"Record {record['slot']}: {', '.join(record['differences'])}")
        print(f'Aligned gameplay checkpoint: game tick {stats["frame_saved_tick"]}, native update {args.iteration}; '
              'both 320x200 four-plane pages, camera and player motion/pose/matrices match original bytes')
        print(f'Physical draw buffers: source {integer(source, 0xC4566C, 2)}, native {integer(native, 0xC4566C, 2)}; '
              'compared by source-defined draw/display roles')
    print('One independent-run gameplay checkpoint accepted; complete sequence and time alignment remain unverified')


if __name__ == '__main__':
    main()
