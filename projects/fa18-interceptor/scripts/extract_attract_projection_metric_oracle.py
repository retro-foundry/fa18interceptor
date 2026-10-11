"""Prove one attract-mode C1C636 -> C2AAD2 projection-depth handoff."""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRACE = ROOT / "build/attract_cockpit_1800_tenframe_trace/trace.jsonl"
PUBLISH = 0xC1C636
METRIC_READ = 0xC2AAD2
DEPTH_TEXT = "$c45a78"


def signed_long(value: int) -> int:
    return value - 0x100000000 if value & 0x80000000 else value


def main() -> None:
    rows = [json.loads(line) for line in TRACE.read_text(encoding="utf-8").splitlines()]
    publishes = [index for index, row in enumerate(rows) if row["pc"] == PUBLISH]
    metrics = [index for index, row in enumerate(rows) if row["pc"] == METRIC_READ]
    if len(publishes) != 1 or len(metrics) != 1 or publishes[0] >= metrics[0]:
        raise ValueError("expected one ordered C1C636 publisher and C2AAD2 consumer")
    publisher_index, metric_index = publishes[0], metrics[0]
    publisher, metric, metric_after = rows[publisher_index], rows[metric_index], rows[metric_index + 1]
    accesses = [
        {"index": row["index"], "frame": row["frame"], "pc": f"${row['pc']:06X}", "asm": row["asm"]}
        for row in rows[publisher_index:metric_index + 1]
        if DEPTH_TEXT in row["asm"].lower()
    ]
    intervening_writes = [access for access in accesses[1:-1] if ", $c45a78" in access["asm"].lower()]
    if intervening_writes:
        raise ValueError(f"unexpected intervening depth writes: {intervening_writes}")
    written = publisher["registers"]["d1"]
    consumed = metric_after["registers"]["d0"]
    if written != consumed:
        raise ValueError(f"depth handoff mismatch: wrote {written:#x}, consumed {consumed:#x}")
    payload = {
        "classification": "same-replay producer-to-consumer projection-depth handoff; no physical-distance claim",
        "authority": TRACE.relative_to(ROOT).as_posix(),
        "publisher": {"index": publisher["index"], "frame": publisher["frame"], "pc": "$C1C636",
                      "d1_long_unsigned": written, "d1_long_signed": signed_long(written),
                      "a2": f"${publisher['registers']['a2'] & 0xFFFFFF:06X}",
                      "input_field_offsets": ["+$14", "+$18", "+$1C"]},
        "consumer": {"index": metric["index"], "frame": metric["frame"], "pc": "$C2AAD2",
                     "d0_after_unsigned": consumed, "d0_after_signed": signed_long(consumed)},
        "depth_accesses": accesses,
        "intervening_depth_writes": intervening_writes,
    }
    output = ROOT / "analysis/data/attract_projection_depth_metric_oracle.json"
    output.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    markdown = ROOT / "analysis/data/attract_projection_depth_metric_oracle.md"
    markdown.write_text(
        "# Attract projection-depth metric oracle\n\n"
        "Classification: **same-replay producer-to-consumer dataflow**.  This proves a renderer "
        "projection component handoff, not physical distance, object identity, or LOD.\n\n"
        f"Authority: `{TRACE.relative_to(ROOT).as_posix()}`. Regenerate with "
        "`python scripts/extract_attract_projection_metric_oracle.py`.\n\n"
        f"At trace index {publisher['index']} (frame {publisher['frame']}), `$C1C636` stores "
        f"`D1={signed_long(written)}` to `$C45A78`. At index {metric['index']} "
        f"(frame {metric['frame']}), `$C2AAD2` reads that longword; the immediately following "
        f"trace row has `D0={signed_long(consumed)}`. The {len(accesses) - 2} intervening "
        "accesses are reads, and no intervening `$C45A78` write occurs.\n\n"
        f"Immediately before the publication tail, `A2=${publisher['registers']['a2'] & 0xFFFFFF:06X}`; "
        "the direct arm reads its `+$14/+$18/+$1C` fields. This is the established mutable "
        "control-record-bank root, not immutable terrain-template data.\n\n"
        "Therefore this replay proves `$C1C636 -> $C45A78 -> $C2AAD2` with value parity. "
        "The producer's selected control-record role, camera context, and any physical interpretation remain open.\n",
        encoding="utf-8")
    print(json.dumps({"output": output.relative_to(ROOT).as_posix(), "value": signed_long(written),
                      "intervening_reads": len(accesses) - 2}))


if __name__ == "__main__":
    main()
