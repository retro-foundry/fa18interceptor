"""Decode a `$C4574A` message-selector sequence from a Slow RAM snapshot."""

import argparse
import json
from pathlib import Path


SLOW_BASE = 0xC00000
SEQUENCE = 0xC4574A
RELATIVE_TABLE = 0xC3ED0A


def read_word(data, address, signed=False):
    offset = address - SLOW_BASE
    return int.from_bytes(data[offset:offset + 2], "big", signed=signed)


def read_payload(data, address):
    offset = address - SLOW_BASE
    payload = data[offset:].split(b"\0", 1)[0]
    return payload.decode("ascii", "replace")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("snapshot", type=Path,
                        help="Slow RAM snapshot, normally slow.bin")
    parser.add_argument("--max-words", type=int, default=64)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    data = args.snapshot.read_bytes()
    if len(data) != 0x80000:
        raise ValueError(f"expected 512 KiB Slow RAM snapshot, got {len(data):,} bytes")

    entries = []
    for word_index in range(args.max_words):
        offset = word_index * 2
        code = read_word(data, SEQUENCE + offset)
        if code == 0:
            break
        table_address = RELATIVE_TABLE + 2 * (code - 1)
        table_offset = table_address - SLOW_BASE
        if code < 1 or table_offset < 0 or table_offset + 2 > len(data):
            raise ValueError(f"selector {code} at ${SEQUENCE + offset:06X} has no in-range table word")
        descriptor = RELATIVE_TABLE + read_word(data, table_address, signed=True)
        descriptor_offset = descriptor - SLOW_BASE
        if descriptor_offset < 0 or descriptor_offset + 4 > len(data):
            raise ValueError(f"selector {code} resolves outside Slow RAM: ${descriptor:06X}")
        entries.append({
            "sequence_offset": f"${offset:02X}",
            "code": code,
            "table_address": f"${table_address:06X}",
            "descriptor": f"${descriptor:06X}",
            "header": data[descriptor_offset:descriptor_offset + 4].hex(" "),
            "payload": read_payload(data, descriptor + 4),
        })
    else:
        raise ValueError(f"no zero terminator within {args.max_words} selector words")

    result = {
        "authority": str(args.snapshot).replace("\\", "/"),
        "sequence_base": f"${SEQUENCE:06X}",
        "relative_table": f"${RELATIVE_TABLE:06X}",
        "entries": entries,
        "terminator_offset": f"${len(entries) * 2:02X}",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    print(f"wrote {args.output}: {len(entries)} selectors")


if __name__ == "__main__":
    main()
