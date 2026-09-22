"""Summarize traced polygon/line rendering that targets a prepared map page."""
from __future__ import annotations

import argparse
import collections
import json
from pathlib import Path


POLYGON_SUBMIT = 0xC2FF48
LINE_EMIT = 0xC2FA7E
SPAN_BLIT = 0xC304F4
POLYGON_RETURN = 0xC24D66
CONTROL_WALK = 0xC1F6F8
LINE_PLANE_BLITS = {0xC2FBE6, 0xC2FC4E, 0xC2FCB6, 0xC2FD1C}


def hex_address(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def write_report(output: Path, report: dict[str, object]) -> None:
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Map display renderer census",
        "",
        "Classification: **scenario-backed prepared-map-page renderer output**.",
        "",
        report["qualification"],
        "",
        f"- Polygon wrapper entries (`$C2FF48`): {report['polygon_wrapper_entries']}",
        f"- Line emitter entries (`$C2FA7E`): {report['line_emitter_entries']}",
        f"- Blitter jobs with a pending-map-page pointer: {report['map_page_blitter_jobs']}",
        f"- Direct span jobs (`$C304F4`): {report['span_blitter_jobs']}",
        f"- Line-plane jobs (`$C2FBE6/$C2FC4E/$C2FCB6/$C2FD1C`): {report['line_plane_blitter_jobs']}",
        "",
        "## Polygon wrapper entries by frame",
        "",
        "| Frame | Entries | `$C2FA7E` within wrapper return path | `$C304F4` within wrapper return path |",
        "| ---: | ---: | ---: | ---: |",
    ]
    for frame, values in report["polygon_route_by_frame"].items():
        lines.append(f"| {frame} | {values['entries']} | {values['line_routes']} | {values['span_routes']} |")
    lines.extend([
        "",
        "## Static control entries and bounded primitive outputs",
        "",
        "| Trace frame | Preceding transform input (`A1`) | Control entry (`A1`) | Lines to next control entry | Polygon line routes | Polygon span routes |",
        "| ---: | --- | --- | ---: | ---: | ---: |",
    ])
    for row in report["control_to_primitive"]:
        lines.append(
            f"| {row['frame']} | `{row['transform_input']}` | `{row['control_entry']}` | {row['line_entries']} | "
            f"{row['polygon_line_routes']} | {row['polygon_span_routes']} |")
    lines.extend([
        "",
        "## `$C2FF48` entry contexts",
        "",
        "| Context at wrapper entry | Count |",
        "| --- | ---: |",
    ])
    for context, count in report["polygon_contexts"].items():
        lines.append(f"| `{context}` | {count} |")
    lines.append("")
    output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path, required=True)
    parser.add_argument("--blitter-jobs", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if args.output.exists() or args.output.with_suffix(".md").exists():
        raise FileExistsError(args.output)

    trace = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    jobs = json.loads(args.blitter_jobs.read_text(encoding="utf-8"))["jobs"]
    polygon_indices = [index for index, row in enumerate(trace) if row["pc"] == POLYGON_SUBMIT]
    line_indices = [index for index, row in enumerate(trace) if row["pc"] == LINE_EMIT]
    control_indices = [index for index, row in enumerate(trace) if row["pc"] == CONTROL_WALK]
    route_by_frame: dict[int, dict[str, int]] = collections.defaultdict(
        lambda: {"entries": 0, "line_routes": 0, "span_routes": 0})
    contexts: collections.Counter[str] = collections.Counter()
    for item, index in enumerate(polygon_indices):
        row = trace[index]
        frame = row["frame"]
        route_by_frame[frame]["entries"] += 1
        contexts[hex_address(row["registers"]["a5"])] += 1
        end = next((cursor for cursor in range(index + 1, len(trace))
                    if trace[cursor]["pc"] == POLYGON_RETURN), len(trace))
        route = {entry["pc"] for entry in trace[index + 1:end]}
        route_by_frame[frame]["line_routes"] += int(LINE_EMIT in route)
        route_by_frame[frame]["span_routes"] += int(SPAN_BLIT in route)
    active_jobs = [job for job in jobs if job["active_display_channels"]]
    span_jobs = [job for job in active_jobs if int(job["trigger_pc"][1:], 16) == SPAN_BLIT]
    line_jobs = [job for job in active_jobs
                 if int(job["trigger_pc"][1:], 16) in LINE_PLANE_BLITS]
    control_to_primitive = []
    for item, index in enumerate(control_indices):
        end = control_indices[item + 1] if item + 1 < len(control_indices) else len(trace)
        interval = trace[index:end]
        polygon_routes = []
        for polygon_index in (cursor for cursor in range(index, end)
                              if trace[cursor]["pc"] == POLYGON_SUBMIT):
            return_index = next((cursor for cursor in range(polygon_index + 1, end)
                                 if trace[cursor]["pc"] == POLYGON_RETURN), end)
            route = {entry["pc"] for entry in trace[polygon_index + 1:return_index]}
            polygon_routes.append(route)
        preceding_transforms = [entry for entry in trace[:index]
                                if entry["pc"] == 0xC1F4AC]
        transform_input = (hex_address(preceding_transforms[-1]["registers"]["a1"])
                           if preceding_transforms else None)
        control_to_primitive.append({
            "trace_index": trace[index]["index"],
            "frame": trace[index]["frame"],
            "control_entry": hex_address(trace[index]["registers"]["a1"]),
            "transform_input": transform_input,
            "line_entries": sum(entry["pc"] == LINE_EMIT for entry in interval),
            "polygon_line_routes": sum(LINE_EMIT in route for route in polygon_routes),
            "polygon_span_routes": sum(SPAN_BLIT in route for route in polygon_routes),
        })
    report = {
        "scope": "trace entries and CPU blitter jobs while the map transition prepares the pending map page",
        "trace": str(args.trace),
        "blitter_jobs": str(args.blitter_jobs),
        "polygon_wrapper_entries": len(polygon_indices),
        "line_emitter_entries": len(line_indices),
        "map_page_blitter_jobs": len(active_jobs),
        "span_blitter_jobs": len(span_jobs),
        "line_plane_blitter_jobs": len(line_jobs),
        "polygon_route_by_frame": {str(frame): values for frame, values in sorted(route_by_frame.items())},
        "polygon_contexts": dict(sorted(contexts.items())),
        "control_to_primitive": control_to_primitive,
        "qualification": (
            "The supplied job inventory identifies CPU blits whose pointers fall in the prepared map page; "
            "the trace identifies renderer entries in the same bounded transition. Polygon routes are bounded "
            "from wrapper entry to its observed `$C24D66` return; control rows are bounded to the next walker "
            "entry (or trace end). This proves renderer output "
            "to that mutable display page, not an immutable terrain mesh, a coastline-pixel-to-record match, "
            "or a complete world-map extraction."
        ),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    write_report(args.output, report)
    print(json.dumps({key: report[key] for key in (
        "polygon_wrapper_entries", "line_emitter_entries", "map_page_blitter_jobs",
        "span_blitter_jobs", "line_plane_blitter_jobs")}, indent=2))


if __name__ == "__main__":
    main()
