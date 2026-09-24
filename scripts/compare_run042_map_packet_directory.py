"""Join run042's newly observed M-map headers to static directory and stream evidence.

This is deliberately a provenance and grammar report.  Directory cell indices
remain local selector indices; neither they nor the signed packet pairs are
promoted to world coordinates, terrain ownership, or physical-distance LOD.
"""
from __future__ import annotations

import argparse
import json
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def header_set(report: dict) -> set[str]:
    return {row["header"] for row in report["packet_streams"]}


def direct_observations(report: dict, header: str) -> list[dict]:
    return [{"frame": row["frame"], "trace_index": row["trace_index"],
             "route": row["route"], "selected_stream": row["selected_stream"]}
            for row in report["packet_streams"] if row["header"] == header]


def stream_summary(streams: dict[str, dict], header: str) -> list[dict]:
    rows = []
    for stream in streams.values():
        if header not in stream["headers"]:
            continue
        rows.append({"start": stream["start"], "roles": stream["roles"],
                     "batches": len(stream["batches"]),
                     "pairs": sum(batch["count"] for batch in stream["batches"]),
                     "terminator": stream["terminator"]})
    return sorted(rows, key=lambda row: row["start"])


def markdown(report: dict) -> str:
    lines = [
        "# Run042 new M-map headers: directory and packet grammar",
        "",
        "Classification: **scenario-observed header selection joined to byte-decoded",
        "wide-directory provenance and static packet grammar**.",
        "",
        "Run042 directly enters the eight listed headers at `$C2AF00`; none is a",
        "direct entry in the bounded run037 stable-map sample. Each has one or more",
        "nonnegative targets in the byte-decoded 32×32 wide directory, and each inline",
        "and alternate stream completes under the exact `$C2AF46` count/threshold grammar.",
        "The cell coordinates are local selector indices, not world coordinates. This",
        "does not identify terrain, coastline ownership, physical extent, or distance LOD.",
        "",
        f"Authority: `{report['run042_inventory']}`, `{report['run037_inventory']}`,",
        f"`{report['wide_directory']}`, and `{report['static_streams']}`.",
        "",
        "| Header | run042 entries / frames | route | wide cells | static streams (role: pairs; terminator) |",
        "| --- | --- | --- | ---: | --- |",
    ]
    for row in report["headers"]:
        frames = ", ".join(str(value) for value in row["run042_frames"])
        routes = ", ".join(f"{name}×{count}" for name, count in row["route_counts"].items())
        streams = "; ".join(
            f"`{stream['start']}` ({'/'.join(stream['roles'])}: {stream['pairs']}; `{stream['terminator']}`)"
            for stream in row["streams"])
        lines.append(f"| `{row['header']}` | {row['run042_entry_count']} / {frames} | {routes} | "
                     f"{len(row['wide_cells'])} | {streams} |")
    lines.extend(["", "## Exact wide-directory cells", "",
                  "Only cells with a nonnegative target word are listed. A cell may share a",
                  "header with other cells; this reuse is static selector provenance, not a",
                  "measured map area.", "",
                  "| Header | `(x,y)` cells |", "| --- | --- |"])
    for row in report["headers"]:
        cells = ", ".join(f"`({cell['x']},{cell['y']})`" for cell in row["wide_cells"])
        lines.append(f"| `{row['header']}` | {cells} |")
    lines.extend(["", "## Evidence boundary", "",
                  "All run042 entries select the inline route with `D7 = 0` in this bounded",
                  "transition trace. The report records complete static alternate streams because",
                  "the byte-exact selector can choose them in other observed scenarios; it does",
                  "not claim that run042 rendered those alternates. Pair counts describe decoded",
                  "packet format, not faces, paths, or map geometry.", ""])
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run042", type=Path, required=True)
    parser.add_argument("--run037", type=Path, required=True)
    parser.add_argument("--wide-directory", type=Path, required=True)
    parser.add_argument("--static-streams", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    markdown_path = args.output.with_suffix(".md")
    if not args.replace and (args.output.exists() or markdown_path.exists()):
        raise FileExistsError(args.output)
    run042 = json.loads(args.run042.read_text(encoding="utf-8"))
    run037 = json.loads(args.run037.read_text(encoding="utf-8"))
    directory = json.loads(args.wide_directory.read_text(encoding="utf-8"))
    static = json.loads(args.static_streams.read_text(encoding="utf-8"))
    new_headers = sorted(header_set(run042) - header_set(run037))
    streams = {row["start"]: row for row in static["streams"]}
    rows = []
    for header in new_headers:
        observations = direct_observations(run042, header)
        cells = [row for row in directory["cells"]
                 if row.get("target") == header and row["classification"] == "nonnegative_target_word"]
        grammar = stream_summary(streams, header)
        # A header is allowed to alias its alternate pointer to header + 4;
        # static_streams then represents both roles with one stream record.
        if not observations or not cells or not 1 <= len(grammar) <= 2:
            raise ValueError(f"incomplete evidence join for {header}")
        route_counts = Counter(row["route"] for row in observations)
        rows.append({"header": header, "run042_entry_count": len(observations),
                     "run042_frames": sorted({row["frame"] for row in observations}),
                     "route_counts": dict(sorted(route_counts.items())),
                     "observations": observations, "wide_cells": cells, "streams": grammar})
    report = {"classification": "run042_new_headers_joined_to_wide_directory_and_static_packet_grammar",
              "run042_inventory": str(args.run042.as_posix()),
              "run037_inventory": str(args.run037.as_posix()),
              "wide_directory": str(args.wide_directory.as_posix()),
              "static_streams": str(args.static_streams.as_posix()),
              "headers": rows,
              "qualification": "Header selection, directory-cell provenance, and packet grammar are proven within their stated authorities. Local directory indices and signed packet pairs have no promoted world-position, terrain, coastline, or physical-distance LOD meaning."}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    markdown_path.write_text(markdown(report), encoding="utf-8")
    print(json.dumps({"new_headers": len(rows), "wide_cells": sum(len(row["wide_cells"]) for row in rows)}))


if __name__ == "__main__":
    main()
