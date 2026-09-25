"""Join each run041 C1EE14 selector input to its immediate record-loop writer."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000
WRITERS = {
    "$C1CC86": {"shift": 0xC1CBA8, "limit": 0xC1CC60,
                 "record_limit_write": 0xC1CC34, "fixed_point_call": 0xC1CC2E,
                 "route": "primary placement-record loop"},
    "$C1CFA6": {"shift": 0xC1CE66, "limit": 0xC1CF6A,
                 "record_limit_write": 0xC1CF32, "fixed_point_call": 0xC1CF2C,
                 "route": "alternate placement-record loop"},
}


def snapshot_record(snapshot: bytes, address: int) -> dict | None:
    """Decode only the fields that the observed loop reads from one record."""
    offset = address - SLOW_BASE
    if not 0 <= offset <= len(snapshot) - 24:
        return None
    word = lambda at: int.from_bytes(snapshot[offset + at:offset + at + 2], "big")
    long = lambda at: int.from_bytes(snapshot[offset + at:offset + at + 4], "big")
    signed = lambda value: value - 0x10000 if value & 0x8000 else value
    descriptor = long(2)
    descriptor_offset = descriptor - SLOW_BASE + 8
    control_pointer = None
    if 0 <= descriptor_offset <= len(snapshot) - 4:
        control_pointer = int.from_bytes(snapshot[descriptor_offset:descriptor_offset + 4], "big")
    return {
        "selector_word": word(0),
        "descriptor": descriptor,
        "coordinate_words_signed": [signed(word(at)) for at in (6, 8, 10)],
        "limit_field_plus_16": word(16),
        "descriptor_control_field_plus_8": control_pointer,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--invocations", type=Path,
                        default=ROOT / "analysis/data/run041_descriptor_stage_invocations.json")
    parser.add_argument("--output", type=Path,
                        default=ROOT / "analysis/data/run041_selector_input_provenance.json")
    args = parser.parse_args()
    args.invocations = args.invocations.resolve()
    args.output = args.output.resolve()
    source = json.loads(args.invocations.read_text(encoding="utf-8"))
    result = {
        "classification": "same-invocation register/store provenance; no physical-coordinate claim",
        "authority": args.invocations.relative_to(ROOT).as_posix(), "traces": [],
    }
    for trace in source["traces"]:
        trace_path = (ROOT / trace["trace"]).resolve()
        rows = [json.loads(line) for line in trace_path.open(encoding="utf-8")]
        snapshot = (trace_path.parent / "slow.bin").read_bytes()
        calls = []
        for call in trace["calls"]:
            route = WRITERS.get(call["call_site"])
            if route is None or call["control_pointer"] not in {"$C35568", "$C355A0"}:
                continue
            start = max(0, call["entry_index"] - 256)
            window = rows[start:call["entry_index"]]
            matches = {}
            for kind in ("shift", "limit"):
                pc = route[kind]
                found = [(start + offset, row) for offset, row in enumerate(window)
                         if row["pc"] == pc and row["frame"] == call["entry_frame"]]
                if not found:
                    raise ValueError(f"{call['call_site']} call {call['entry_index']} lacks ${pc:06X}")
                matches[kind] = found[-1]
            shift_index, shift_row = matches["shift"]
            limit_index, limit_row = matches["limit"]
            stored_shift = shift_row["registers"]["d0"] & 0xFFFF
            stored_limit = limit_row["registers"]["d1"] & 0xFFFF
            if stored_shift != call["shift"] or stored_limit != call["threshold"]:
                raise ValueError(f"input mismatch at call {call['entry_index']}")
            active_record = (limit_row["registers"]["a0"] & 0xFFFFFF) - 12
            record_limit_writes = [
                (start + offset, row) for offset, row in enumerate(window)
                if (row["pc"] == route["record_limit_write"]
                    and (row["registers"]["a0"] & 0xFFFFFF) == (limit_row["registers"]["a0"] & 0xFFFFFF))]
            record_limit_write = record_limit_writes[-1] if record_limit_writes else None
            fixed_point_calls = [
                (start + offset, row) for offset, row in enumerate(window)
                if row["pc"] == route["fixed_point_call"]]
            fixed_point_call = fixed_point_calls[-1] if fixed_point_calls else None
            if record_limit_write and record_limit_write[1]["registers"]["d1"] & 0xFFFF != stored_limit:
                raise ValueError(f"record limit write mismatch at call {call['entry_index']}")
            record = snapshot_record(snapshot, active_record)
            if record is None:
                raise ValueError(f"record ${active_record:06X} is outside the trace snapshot")
            snapshot_matches = {
                "limit_field_plus_16": record["limit_field_plus_16"] == stored_limit,
                "selector_low_nibble": (record["selector_word"] & 0x000F) == stored_shift,
                "descriptor_control_field_plus_8": (
                    record["descriptor_control_field_plus_8"] == int(call["control_pointer"][1:], 16)),
            }
            calls.append({
                "entry_frame": call["entry_frame"], "entry_index": call["entry_index"],
                "call_site": call["call_site"], "route": route["route"],
                "control_pointer": call["control_pointer"],
                "shift_store": {"pc": f"${route['shift']:06X}", "trace_index": shift_index,
                                "d0_word": stored_shift,
                                "a0": f"${shift_row['registers']['a0'] & 0xFFFFFF:06X}"},
                "limit_store": {"pc": f"${route['limit']:06X}", "trace_index": limit_index,
                                "d1_word": stored_limit,
                                "a0": f"${limit_row['registers']['a0'] & 0xFFFFFF:06X}",
                                # The loop has consumed selector (2), descriptor (4),
                                # and coordinate (6) bytes before reading +$04 here.
                                # Keep the raw loop address too, so this is an address
                                # calculation rather than a record-ownership claim.
                                "active_record": f"${active_record:06X}"},
                "snapshot_record": {
                    "selector_word": f"${record['selector_word']:04X}",
                    "descriptor": f"${record['descriptor']:06X}",
                    "coordinate_words_signed": record["coordinate_words_signed"],
                    "limit_field_plus_16": record["limit_field_plus_16"],
                    "descriptor_control_field_plus_8": f"${record['descriptor_control_field_plus_8']:06X}",
                    "matches_observed_call": snapshot_matches,
                },
                "same_record_limit_update": (
                    {"fixed_point_call_pc": f"${route['fixed_point_call']:06X}",
                     "fixed_point_call_trace_index": fixed_point_call[0] if fixed_point_call else None,
                     "record_write_pc": f"${route['record_limit_write']:06X}",
                     "record_write_trace_index": record_limit_write[0],
                     "record_write_d1_word": record_limit_write[1]["registers"]["d1"] & 0xFFFF}
                    if record_limit_write else None),
                "selected_transform_sources": call["transform_sources"],
                "termination": call["termination"],
            })
        result["traces"].append({"trace": trace["trace"], "calls": calls})
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"output": args.output.relative_to(ROOT).as_posix(),
                      "calls": [len(trace["calls"]) for trace in result["traces"]]}))


if __name__ == "__main__":
    main()
