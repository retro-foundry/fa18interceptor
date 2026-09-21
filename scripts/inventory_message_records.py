"""Decode a bounded range of static message-selector records from Slow RAM.

This is a table/layout inventory.  It does not assert that every selector is
reachable in a particular menu state or that a record represents game state.
"""

import argparse
import json
from pathlib import Path


SLOW_BASE = 0xC00000
RELATIVE_TABLE = 0xC3ED0A


def read_word(data, address, signed=False):
    offset = address - SLOW_BASE
    return int.from_bytes(data[offset:offset + 2], "big", signed=signed)


def payload_until_nul(data, address):
    offset = address - SLOW_BASE
    end = data.find(b"\0", offset)
    if end < 0:
        raise ValueError(f"record payload at ${address:06X} has no Slow-RAM terminator")
    return data[offset:end]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--slow", type=Path,
                        default=Path("captures/baseline_menu/slow.bin"))
    parser.add_argument("--first-code", type=int, default=1)
    parser.add_argument("--last-code", type=int, default=110)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.first_code < 1 or args.last_code < args.first_code:
        raise ValueError("selector-code range must be positive and ordered")

    data = args.slow.read_bytes()
    if len(data) != 0x80000:
        raise ValueError(f"expected 512 KiB Slow RAM snapshot, got {len(data):,} bytes")

    records = []
    for code in range(args.first_code, args.last_code + 1):
        table_address = RELATIVE_TABLE + 2 * (code - 1)
        if not SLOW_BASE <= table_address <= SLOW_BASE + len(data) - 2:
            raise ValueError(f"selector {code} table word lies outside Slow RAM")
        descriptor = RELATIVE_TABLE + read_word(data, table_address, signed=True)
        if not SLOW_BASE <= descriptor <= SLOW_BASE + len(data) - 4:
            raise ValueError(f"selector {code} descriptor ${descriptor:06X} lies outside Slow RAM")
        raw_payload = payload_until_nul(data, descriptor + 4)
        records.append({
            "code": code,
            "table_address": f"${table_address:06X}",
            "descriptor": f"${descriptor:06X}",
            "header": data[descriptor - SLOW_BASE:descriptor - SLOW_BASE + 4].hex(" "),
            "payload_hex": raw_payload.hex(" "),
            "payload_ascii": raw_payload.decode("ascii", "replace"),
        })

    report = {
        "authority": str(args.slow).replace("\\", "/"),
        "relative_table": f"${RELATIVE_TABLE:06X}",
        "selector_codes": [args.first_code, args.last_code],
        "records": records,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    print(f"wrote {args.output}: {len(records)} records")


if __name__ == "__main__":
    main()
