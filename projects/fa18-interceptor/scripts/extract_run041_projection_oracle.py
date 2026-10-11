"""Extract the two bounded C2ECxx outcomes from the sealed run041 source trace."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def signed_word(value: int) -> int:
    value &= 0xFFFF
    return value - 0x10000 if value & 0x8000 else value


def trunc_divide(numerator: int, denominator: int) -> int:
    magnitude = abs(numerator) // abs(denominator)
    return -magnitude if (numerator < 0) != (denominator < 0) else magnitude


def pc(row: dict) -> int:
    return int(row["pc"][1:], 16)


def unique_index(rows: list[dict], address: int) -> int:
    matches = [index for index, row in enumerate(rows) if pc(row) == address]
    if len(matches) != 1:
        raise ValueError(f"expected one ${address:06X} row, found {len(matches)}")
    return matches[0]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path,
                        default=ROOT / "build/run041_c35a98_source_interval/trace.jsonl")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/run041_projection_oracle.json")
    args = parser.parse_args()
    rows = [json.loads(line) for line in args.trace.open(encoding="utf-8")]

    accepted_entry = unique_index(rows, 0xC2EC90)
    projection = unique_index(rows, 0xC2ECC6)
    store = unique_index(rows, 0xC2ED00)
    accepted_return = unique_index(rows, 0xC2ED6A)
    rejected_entry = unique_index(rows, 0xC2EC9C)
    reject_store = unique_index(rows, 0xC2EC84)
    rejected_return = unique_index(rows, 0xC2EC8E)
    if not (accepted_entry < projection < store < accepted_return
            < rejected_entry < reject_store < rejected_return):
        raise ValueError("projection calls do not have the expected trace order")
    if pc(rows[accepted_entry - 1]) != 0xC33D34 or pc(rows[accepted_return + 1]) != 0xC33D3A:
        raise ValueError("accepted call/return boundary changed")
    if pc(rows[rejected_entry - 1]) != 0xC0DB3A or pc(rows[rejected_return + 1]) != 0xC0DB40:
        raise ValueError("rejected call/return boundary changed")

    inputs = rows[projection]["registers"]
    x, y, depth = (signed_word(inputs[name]) for name in ("d0", "d1", "d2"))
    if depth <= 0:
        raise ValueError("accepted projection requires positive signed depth")
    screen_x = min(319, max(0, 160 + trunc_divide(x * 160, depth)))
    screen_y = min(179, max(0, 90 + trunc_divide(y * 90, depth)))
    expected = [319 - screen_x, 180 - screen_y]
    actual = [signed_word(rows[store]["registers"][name]) for name in ("d0", "d1")]
    if actual != expected:
        raise ValueError(f"projected pair {actual} differs from arithmetic {expected}")
    if signed_word(rows[accepted_return]["registers"]["d7"]) != -1:
        raise ValueError("negative-D7 return path changed")

    rejected = rows[rejected_entry]["registers"]
    if signed_word(rejected["d0"]) < signed_word(rejected["d2"]):
        raise ValueError("expected first signed bound check to reject")
    if rows[rejected_return]["registers"]["d0"] != 0:
        raise ValueError("rejected return D0 changed")
    if rows[reject_store]["bytes"] != "23fcffffffff00c45958":
        raise ValueError("rejected-path longword store changed")
    result = {
        "authority": args.trace.relative_to(ROOT).as_posix() if args.trace.is_relative_to(ROOT) else str(args.trace),
        "classification": "two trace-backed call/return fixtures; no pixel or physical-coordinate claim",
        "accepted": {
            "caller": "$C33D34", "entry": "$C2EC90", "return_to": "$C33D3A",
            "trace_rows": [accepted_entry, accepted_return],
            "signed_input_d0_d1_d2": [x, y, depth], "unreflected_xy": [screen_x, screen_y],
            "projected_pair_C45958": actual, "return_d7": -1,
        },
        "rejected": {
            "caller": "$C0DB3A", "entry": "$C2EC9C", "return_to": "$C0DB40",
            "trace_rows": [rejected_entry, rejected_return],
            "signed_input_d0_d1_d2": [signed_word(rejected[name]) for name in ("d0", "d1", "d2")],
            "first_bound_comparison": "signed D0 >= signed D2",
            "return_d0": 0, "longword_C45958": "FFFFFFFF",
        },
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"output": str(args.output), "accepted_pair": actual}))


if __name__ == "__main__":
    main()
