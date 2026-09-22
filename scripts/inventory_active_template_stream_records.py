"""Join selected static streams to the template records the copier actually reads."""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRACE = ROOT / "build/run033_origin_control_trace/trace.jsonl"
SLOW = ROOT / "build/run033_origin_control_trace/slow.bin"
SLOW_BASE = 0xC00000
SELECTOR_ENTRY_PC = 0xC1D3F4
STREAM_START_PC = 0xC1D442
COPY_HEADER_READ_PC = 0xC1D488


def address(value: int) -> str:
    return f"${value:06X}"


def main() -> None:
    trace = [json.loads(line) for line in TRACE.read_text(encoding="utf-8").splitlines()]
    slow = SLOW.read_bytes()
    starts = [index for index, row in enumerate(trace) if row["pc"] == SELECTOR_ENTRY_PC]
    rows = []
    for start, end in zip(starts, starts[1:] + [len(trace)]):
        packet = trace[start:end]
        stream_start = next((row for row in packet if row["pc"] == STREAM_START_PC), None)
        if stream_start is None:
            continue
        copies = []
        for row in packet:
            if row["pc"] != COPY_HEADER_READ_PC:
                continue
            source = row["registers"]["a5"]
            offset = source - SLOW_BASE
            header = slow[offset]
            copies.append({
                "static_source": address(source),
                "header": f"${header:02X}",
                "word_1": f"${int.from_bytes(slow[offset + 1:offset + 3], 'big'):04X}",
                "word_2": f"${int.from_bytes(slow[offset + 3:offset + 5], 'big'):04X}",
            })
        if not copies:
            continue
        rows.append({
            "selector_trace_index": packet[0]["index"],
            "workspace_band": address(packet[0]["registers"]["a3"]),
            "selector_index": f"${packet[0]['registers']['d0'] & 0xFFFF:04X}",
            "row_key": f"${packet[0]['registers']['d1'] & 0xFFFF:04X}",
            "static_stream": address(stream_start["registers"]["a5"]),
            "copied_record_count": len(copies),
            "records": copies,
        })
    payload = {
        "authority": {
            "trace": str(TRACE.relative_to(ROOT)).replace("\\", "/"),
            "slow_snapshot": str(SLOW.relative_to(ROOT)).replace("\\", "/"),
            "selector_entry_pc": address(SELECTOR_ENTRY_PC),
            "stream_start_pc": address(STREAM_START_PC),
            "copy_header_read_pc": address(COPY_HEADER_READ_PC),
        },
        "classification": "scenario_backed_static_stream_to_template_record_inventory_not_global_map_export",
        "active_streams": rows,
    }
    json_path = ROOT / "analysis/data/active_terrain_template_stream_records.json"
    markdown_path = ROOT / "analysis/data/active_terrain_template_stream_records.md"
    json_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Active terrain-template stream records",
        "",
        "Classification: **scenario-backed static stream-to-template-record inventory**. "
        "Each row joins a selected `$C1D3F4` stream to the exact `$C1D488` source records "
        "that `$C1D442-$C1D4C2` copied into a mutable workspace. It is not a complete "
        "global map or raw vertex export.",
        "",
        "Authority: `build/run033_origin_control_trace/trace.jsonl` and `slow.bin`. "
        "The stream address is captured at `$C1D442`; each listed source is captured before "
        "the `(A5)+` header read at `$C1D488`.",
        "",
        f"The bounded control window has **{len(rows)}** selected streams that copy "
        f"**{sum(row['copied_record_count'] for row in rows)}** template records.",
        "",
        "| workspace band | group selector / row key | static stream | copied records | source records (`header`, `word_1`, `word_2`) |",
        "| --- | --- | --- | ---: | --- |",
    ]
    for row in rows:
        records = "; ".join(f"{record['static_source']} ({record['header']}, "
                            f"{record['word_1']}, {record['word_2']})"
                            for record in row["records"])
        lines.append(f"| {row['workspace_band']} | {row['selector_index']} / {row['row_key']} | "
                     f"{row['static_stream']} | {row['copied_record_count']} | {records} |")
    lines += [
        "",
        "The same static stream can be selected for different workspace bands, and the "
        "template words remain inputs to the placement builder rather than direct global "
        "coordinates. The inventory identifies cell-content candidates only for this sealed "
        "origin window; it must not be extrapolated into a complete terrain export.",
        "",
    ]
    markdown_path.write_text("\n".join(lines), encoding="utf-8")
    print(f"wrote {json_path.relative_to(ROOT)} and {markdown_path.relative_to(ROOT)} "
          f"({len(rows)} streams, {sum(row['copied_record_count'] for row in rows)} records)")


if __name__ == "__main__":
    main()
