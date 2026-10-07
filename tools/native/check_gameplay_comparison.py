"""Verify gameplay comparison strictness using an actual accepted RAM pair.

These mutations test the verifier, never supply native game behavior. Equivalent
buffer allocation must pass; wrong presentation, input, physics or drawing must
fail, including a changed bit in the timer-driven cockpit information line.
"""
import argparse
from pathlib import Path

from check_gameplay_checkpoint import compare_gameplay, compare_record_state, integer


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
                          (0xC458DA, 'boundary/control C458DA'),
                          (0xC45C3E, 'observer'),
                          (0xC45C20, 'camera matrix'),
                          (0xC45BD8, 'view matrix')):
        changed = bytearray(native)
        changed[offset(address)] ^= 1
        assert any(value.startswith(kind) for value in compare_gameplay(source, changed)), hex(address)
    # Matching kinematics cannot conceal a different record flag/countdown.
    # Check exact changed-byte evidence even when the baseline already carries
    # separately reported cold-start record differences.
    for slot, field in ((0, 4), (0, 0x4C), (15, 0x7E)):
        changed = bytearray(native)
        address = 0xC46184 + slot * 512 + field
        changed[offset(address)] = source[offset(address)] ^ 1
        record = next(item for item in compare_record_state(source, changed) if item['slot'] == slot)
        assert any(value.startswith(f'+{field:02X}:') for value in record['differences']), (slot, field)
    print('Gameplay comparator: equivalent allocation passes; wrong frame/table, HUD bit, motion, '
          'mouse position, key-release phase, game tick, observer and camera/view matrices reject')
    print('Full record diagnostics detect player flags/countdowns and the last record independently of page/kinematic matches')


if __name__ == '__main__':
    main()
