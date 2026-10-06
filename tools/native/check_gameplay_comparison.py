"""Verify gameplay comparison strictness using an actual accepted RAM pair.

These mutations test the verifier, never supply native game behavior. Equivalent
buffer allocation must pass; wrong presentation, input, physics or drawing must
fail, including a changed bit in the timer-driven cockpit information line.
"""
import argparse
from pathlib import Path

from check_gameplay_checkpoint import compare_gameplay, integer


def offset(address):
    return address if address < 0x80000 else address - 0xC00000 + 0x80000


def store(data, address, value, size):
    start = offset(address)
    data[start:start + size] = value.to_bytes(size, 'big')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, required=True)
    parser.add_argument('--native', type=Path, required=True, help='actual matching pre-input native export')
    args = parser.parse_args()
    source, native = args.source.read_bytes(), args.native.read_bytes()
    assert not compare_gameplay(source, native), 'baseline pair must independently match'
    table = offset(0xC4566E)
    changed = bytearray(native)
    changed[table:table + 16], changed[table + 16:table + 32] = (
        native[table + 16:table + 32], native[table:table + 16])
    swapped = integer(native, 0xC4566C, 2) ^ 1
    store(changed, 0xC4566C, swapped, 2)
    store(changed, 0xC456B6, 0xC4566E + 16 * swapped, 4)
    assert not compare_gameplay(source, changed), 'equivalent host allocation was rejected'
    # A coherent selection of the wrong drawing/display frame must fail.
    store(changed, 0xC4566C, swapped ^ 1, 2)
    store(changed, 0xC456B6, 0xC4566E + 16 * (swapped ^ 1), 4)
    assert any(value.startswith('page ') for value in compare_gameplay(source, changed)), (
        'wrong page role accepted; use a baseline whose two gameplay pages differ')
    changed = bytearray(native)
    store(changed, 0xC456B6, 0xC4566E + 16 * swapped, 4)
    try:
        compare_gameplay(source, changed)
    except AssertionError as error:
        assert 'active drawing table' in str(error), error
    else:
        raise AssertionError('wrong active drawing table was accepted')
    plane = integer(native, 0xC4566E + 16 * (swapped ^ 1) + 4 * 3, 4)
    for address, kind in ((plane + 7700, 'page draw plane 3'),
                          (0xC46190, 'position and motion'),
                          (0xC45776, 'boundary/control C45776'),
                          (0xC4582F, 'boundary/control C4582E'),
                          (0xC458DA, 'boundary/control C458DA')):
        changed = bytearray(native)
        changed[offset(address)] ^= 1
        assert any(value.startswith(kind) for value in compare_gameplay(source, changed)), hex(address)
    print('Gameplay comparator: equivalent allocation passes; wrong frame/table, HUD bit, motion, '
          'mouse position, key-release phase and game tick reject')


if __name__ == '__main__':
    main()
