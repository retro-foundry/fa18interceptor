"""Join sealed M-map packet transforms to their direct display/blitter work.

This is deliberately a trace-order attribution, not a source-mesh exporter.
For each C2AFE2 direct call to C246A0 it retains the preceding C2AF9C pair
reads, then finds C2FF48 polygon wrappers and C304F4 pending-page BLTSIZE
jobs before that exact call returns to C2AFE8.  The one-to-one C304F4/BLTSIZE
ordinal check is an explicit acceptance gate.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path


PAIR_ADD = 0xC2AF9C
DISPLAY_CALL = 0xC2AFE2
DISPLAY_RETURN = 0xC2AFE8
POLYGON = 0xC2FF48
PAGE_BLIT = 0xC304F4
SOURCE_START = 0xC42CA8
SOURCE_END = 0xC444F8


def word(data: bytes, address: int) -> int:
    offset = address - 0xC00000
    return int.from_bytes(data[offset:offset + 2], "big", signed=True)


def hex6(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, required=True)
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--pending-page-jobs", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--markdown", type=Path, required=True)
    args = parser.parse_args()

    rows = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    slow = args.slow.read_bytes()
    jobs = json.loads(args.pending_page_jobs.read_text(encoding="utf-8"))["jobs"]
    row_at = {row["index"]: row for row in rows}
    page_entries = [row for row in rows if row["pc"] == PAGE_BLIT]
    page_jobs = [job for job in jobs if job["trigger_pc"] == "$C304F4"]
    if len(page_entries) != len(page_jobs):
        raise RuntimeError(f"C304F4 trace/job mismatch: {len(page_entries)} entries, {len(page_jobs)} jobs")
    job_for_entry = {entry["index"]: job for entry, job in zip(page_entries, page_jobs)}

    calls = [row for row in rows if row["pc"] == DISPLAY_CALL]
    records = []
    for call_number, call in enumerate(calls):
        call_index = call["index"]
        return_index = next((row["index"] for row in rows[call_index + 1:]
                             if row["pc"] == DISPLAY_RETURN), None)
        if return_index is None:
            raise RuntimeError(f"C2AFE2 at trace index {call_index} has no C2AFE8 return")
        previous_return = max((row["index"] for row in rows[:call_index]
                               if row["pc"] == DISPLAY_RETURN), default=-1)
        pair_rows = [row for row in rows[previous_return + 1:call_index]
                     if row["pc"] == PAIR_ADD and SOURCE_START <= (row["registers"]["a3"] & 0xFFFFFF) < SOURCE_END]
        pairs = [{"source": hex6(row["registers"]["a3"]),
                  "raw_pair": [word(slow, row["registers"]["a3"] & 0xFFFFFF),
                               word(slow, (row["registers"]["a3"] & 0xFFFFFF) + 2)],
                  "workspace": hex6(row["registers"]["a5"]),
                  "trace_index": row["index"]}
                 for row in pair_rows]
        wrappers = [row for row in rows[call_index:return_index] if row["pc"] == POLYGON]
        polygon_rows = []
        for number, wrapper in enumerate(wrappers):
            end = wrappers[number + 1]["index"] if number + 1 < len(wrappers) else return_index
            blit_entries = [row for row in rows[wrapper["index"]:end] if row["pc"] == PAGE_BLIT]
            polygon_rows.append({
                "trace_index": wrapper["index"],
                "frame": wrapper["frame"],
                "workspace_context": hex6(wrapper["registers"]["a5"]),
                "pending_page_blits": [job_for_entry[row["index"]] for row in blit_entries],
            })
        if pairs or polygon_rows:
            records.append({
                "call": call_number,
                "frame": call["frame"],
                "display_call_trace_index": call_index,
                "display_return_trace_index": return_index,
                "packet_pairs": pairs,
                "polygon_submissions": polygon_rows,
            })

    polygon_attributed = [record for record in records if record["packet_pairs"] and record["polygon_submissions"]]
    page_attributed = [record for record in polygon_attributed
                       if any(poly["pending_page_blits"] for poly in record["polygon_submissions"])]
    report = {
        "scope": "direct C2AF9C packet-pair to C246A0/C2FF48/C304F4 trace-order attribution",
        "authority": {"trace": str(args.trace), "slow": str(args.slow),
                      "pending_page_jobs": str(args.pending_page_jobs)},
        "acceptance": {"C2AFE2 direct call window ends at its observed C2AFE8 return": True,
                       "C304F4 trace entries equal pending-page BLTSIZE jobs": len(page_entries),
                       "packet_source_range": "$C42CA8-$C444F7"},
        "calls_with_packet_pairs": len(records),
        "calls_reaching_polygon_submission": len(polygon_attributed),
        "calls_reaching_pending_page_blits": len(page_attributed),
        "attributions": polygon_attributed,
        "qualification": ("The report proves a packet batch reached each listed polygon wrapper and its ordered C304F4 "
                          "pending-page blits before the exact C2AFE2 call returned. It does not identify a global "
                          "terrain mesh, join separate packets into faces, or assign physical near/far meaning."),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# M-map packet-to-screen attribution", "", report["qualification"], "",
             f"- Packet-bearing C2AFE2 calls: {len(records)}", f"- Calls reaching C2FF48: {len(polygon_attributed)}",
             f"- Calls reaching pending-page BLTSIZE work: {len(page_attributed)}", "",
             "| Call | frame | pair sources | polygons | pending-page BLTSIZE jobs |", "| ---: | ---: | --- | ---: | ---: |"]
    for record in polygon_attributed:
        sources = ", ".join(pair["source"] for pair in record["packet_pairs"])
        job_count = sum(len(poly["pending_page_blits"]) for poly in record["polygon_submissions"])
        lines.append(f"| {record['display_call_trace_index']} | {record['frame']} | {sources} | {len(record['polygon_submissions'])} | {job_count} |")
    args.markdown.parent.mkdir(parents=True, exist_ok=True)
    args.markdown.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(json.dumps({"packet_calls": len(records), "polygon_calls": len(polygon_attributed),
                      "page_calls": len(page_attributed), "page_blits": sum(len(poly["pending_page_blits"]) for record in page_attributed for poly in record["polygon_submissions"])}))


if __name__ == "__main__":
    main()
