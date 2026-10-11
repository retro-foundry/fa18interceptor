"""Inventory static C1F6F8 stream entries observed in bridge/external captures."""
from __future__ import annotations

import json
from collections import defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CAPTURES = (
    ("external aircraft frame 7500", ROOT / "build" / "run031_frame7500_external_control_entries" / "control_stream_entries.json"),
    ("Golden Gate frame 12000", ROOT / "build" / "run031_frame12000_golden_gate_control_entries" / "control_stream_entries.json"),
    ("later bridge frame 14500", ROOT / "build" / "run031_frame14500_bridge_control_entries" / "control_stream_entries.json"),
)


def address(text: str) -> int:
    return int(text.removeprefix("$"), 16)


def main() -> None:
    manifest = json.loads((ROOT / "analysis" / "runtime_region_manifest.json").read_text())
    owners = [row for row in manifest["hunk_segments"] if "runtime_start" in row]
    rows: dict[int, dict] = {}
    for label, path in CAPTURES:
        report = json.loads(path.read_text())
        for entry in report["entries"]:
            stream = address(entry["stream"])
            owner = next((row for row in owners
                          if address(row["runtime_start"]) <= stream < address(row["runtime_end_exclusive"])), None)
            # Only original-payload regions qualify as static control-stream
            # candidates. Heap/workspace entries are retained only in source
            # collector outputs, not promoted here.
            if not owner or owner["hunk_kind"] != "CODE" or owner["classification"] not in {
                    "code_hunk_executed", "code_hunk_branch_target", "code_hunk_unclassified"}:
                continue
            row = rows.setdefault(stream, {"stream": entry["stream"], "segment": owner["segment"],
                                           "segment_range": f"{owner['runtime_start']}-{owner['runtime_end_exclusive']}",
                                           "observations": []})
            row["observations"].append({"capture": label, "host_frame": entry["host_frame"],
                                        "first_words": entry["first_words"], "caller_a5": entry["caller_a5"]})
    result = sorted(rows.values(), key=lambda row: address(row["stream"]))
    output = {"scope": "static control streams observed at C1F6F8", "streams": result,
              "qualification": "These are renderer-control entries, not vertex/model ownership claims. Dynamic C3Bxxx/C4BFxx entries are intentionally excluded."}
    (ROOT / "analysis" / "live_control_streams.json").write_text(json.dumps(output, indent=2) + "\n")
    lines = ["# Live control-stream inventory", "", output["qualification"], "",
             "| Stream | Segment | Observations | First observed words |", "| --- | ---: | ---: | --- |"]
    for row in result:
        first = row["observations"][0]
        lines.append(f"| `{row['stream']}` | {row['segment']} | {len(row['observations'])} | `{' '.join(first['first_words'])}` |")
    lines += ["", "Capture provenance is retained per row in `analysis/live_control_streams.json`. The streams are direct `$C1F6F8` entries captured before record dispatch.", ""]
    (ROOT / "analysis" / "live_control_streams.md").write_text("\n".join(lines))
    print(f"wrote {len(result)} static control-stream rows")


if __name__ == "__main__":
    main()
