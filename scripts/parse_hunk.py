"""Conservatively inventory an Amiga Hunk executable without flattening segments."""
import argparse
import json
import struct
from pathlib import Path

HUNK_HEADER = 0x3F3
HUNK_CODE = 0x3E9
HUNK_DATA = 0x3EA
HUNK_BSS = 0x3EB
HUNK_RELOC32 = 0x3EC
HUNK_RELOC16 = 0x3ED
HUNK_RELOC8 = 0x3EE
HUNK_EXT = 0x3EF
HUNK_SYMBOL = 0x3F0
HUNK_DEBUG = 0x3F1
HUNK_END = 0x3F2


class Reader:
    def __init__(self, data):
        self.data, self.offset = data, 0

    def u32(self):
        if self.offset + 4 > len(self.data):
            raise ValueError(f'Unexpected EOF at {self.offset:#x}')
        value = struct.unpack_from('>I', self.data, self.offset)[0]
        self.offset += 4
        return value

    def bytes(self, size):
        if self.offset + size > len(self.data):
            raise ValueError(f'Unexpected EOF at {self.offset:#x}')
        value = self.data[self.offset:self.offset + size]
        self.offset += size
        return value


def read_reloc(reader):
    groups = []
    while True:
        entries = reader.u32()
        if entries == 0:
            return groups
        target = reader.u32()
        offsets = [reader.u32() for _ in range(entries)]
        groups.append({'target_segment': target, 'offsets': offsets})


def skip_symbols(reader):
    count = 0
    while True:
        name_longs = reader.u32()
        if name_longs == 0:
            return count
        reader.bytes(name_longs * 4)
        reader.u32()
        count += 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    data = args.input.read_bytes()
    reader = Reader(data)
    if reader.u32() != HUNK_HEADER:
        raise ValueError('Expected HUNK_HEADER')
    resident_names = []
    while True:
        name_longs = reader.u32()
        if name_longs == 0:
            break
        resident_names.append(reader.bytes(name_longs * 4).rstrip(b'\0').decode('latin1'))
    table_size, first_hunk, last_hunk = reader.u32(), reader.u32(), reader.u32()
    sizes = [reader.u32() for _ in range(table_size)]
    segments = []
    current = None
    while reader.offset < len(data):
        block_offset = reader.offset
        block = reader.u32() & 0x3fffffff
        if block in (HUNK_CODE, HUNK_DATA, HUNK_BSS):
            if current is not None:
                raise ValueError(f'New segment before HUNK_END at {block_offset:#x}')
            size_longs = reader.u32()
            payload_offset = reader.offset
            payload_size = size_longs * 4
            if block != HUNK_BSS:
                reader.bytes(payload_size)
            current = {'index': len(segments), 'kind': {HUNK_CODE: 'CODE', HUNK_DATA: 'DATA', HUNK_BSS: 'BSS'}[block],
                       'header_file_offset': block_offset, 'payload_file_offset': payload_offset,
                       'size_longs': size_longs, 'size_bytes': payload_size, 'reloc32_count': 0,
                       'reloc16_count': 0, 'reloc8_count': 0, 'reloc32': [], 'symbol_count': 0}
        elif block in (HUNK_RELOC32, HUNK_RELOC16, HUNK_RELOC8):
            if current is None:
                raise ValueError(f'Relocation outside segment at {block_offset:#x}')
            groups = read_reloc(reader)
            current[{HUNK_RELOC32: 'reloc32_count', HUNK_RELOC16: 'reloc16_count', HUNK_RELOC8: 'reloc8_count'}[block]] += sum(len(g['offsets']) for g in groups)
            if block == HUNK_RELOC32:
                current['reloc32'].extend(groups)
        elif block == HUNK_SYMBOL:
            if current is None:
                raise ValueError(f'Symbol block outside segment at {block_offset:#x}')
            current['symbol_count'] += skip_symbols(reader)
        elif block == HUNK_DEBUG:
            reader.bytes(reader.u32() * 4)
        elif block == HUNK_END:
            if current is None:
                raise ValueError(f'HUNK_END without segment at {block_offset:#x}')
            segments.append(current)
            current = None
        elif block == HUNK_EXT:
            raise ValueError(f'HUNK_EXT at {block_offset:#x}: parser intentionally refuses to flatten external references')
        else:
            raise ValueError(f'Unsupported Hunk block {block:#x} at {block_offset:#x}')
    if current is not None:
        raise ValueError('Unterminated final segment')
    if len(segments) != table_size:
        raise ValueError(f'Header says {table_size} segments, parsed {len(segments)}')
    report = {'input': str(args.input), 'input_size': len(data), 'resident_names': resident_names,
              'first_hunk': first_hunk, 'last_hunk': last_hunk, 'header_sizes_longs': sizes,
              'segments': segments}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({'segments': len(segments),
                      'kinds': {kind: sum(s['kind'] == kind for s in segments) for kind in ('CODE', 'DATA', 'BSS')},
                      'output': str(args.output)}, indent=2))


if __name__ == '__main__':
    main()
