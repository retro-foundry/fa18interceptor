"""Decode structurally valid segment-68 packet streams rooted at traced headers.

Only direct headers observed at C2AF00 seed the walk.  The walker follows each
header's inline and alternate stream using the byte-exact C2AF46 grammar:
positive count (at most 0x12) followed by that many signed word pairs; or a
non-positive threshold word followed by a count; with FFFF as terminator.
This exposes static packet payload reachable from observed headers, not an
unconditionally rendered world map.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW_BASE = 0xC00000
START = 0xC42CA8
END = 0xC444F8
MAX_COUNT = 0x12


def address(value: int) -> str:
    return f"${value:06X}"


def word(data: bytes, address_value: int, signed: bool = False) -> int:
    offset = address_value - SLOW_BASE
    return int.from_bytes(data[offset:offset + 2], "big", signed=signed)


def long(data: bytes, address_value: int) -> int:
    offset = address_value - SLOW_BASE
    return int.from_bytes(data[offset:offset + 4], "big") & 0xFFFFFF


def parse_stream(data: bytes, start: int) -> dict:
    cursor = start
    batches = []
    while True:
        if not START <= cursor <= END - 2:
            raise ValueError(f"stream {address(start)} leaves segment at {address(cursor)}")
        first_address = cursor
        first = word(data, cursor, signed=True)
        cursor += 2
        threshold = None
        if first == -1:
            return {"start": address(start), "batches": batches,
                    "terminator": address(first_address), "end_exclusive": address(cursor)}
        if first <= 0:
            threshold = first & 0x7FFF
            if not START <= cursor <= END - 2:
                raise ValueError(f"stream {address(start)} threshold lacks count")
            count = word(data, cursor, signed=True)
            cursor += 2
        else:
            count = first
        if not 1 <= count <= MAX_COUNT:
            raise ValueError(f"stream {address(start)} has invalid count {count} at {address(first_address)}")
        payload_start = cursor
        payload_end = cursor + count * 4
        if payload_end > END:
            raise ValueError(f"stream {address(start)} payload leaves segment")
        pairs = [[word(data, item, signed=True), word(data, item + 2, signed=True)]
                 for item in range(payload_start, payload_end, 4)]
        batches.append({"prefix": address(first_address), "threshold_word": threshold,
                        "count": count, "pair_start": address(payload_start),
                        "pair_end_exclusive": address(payload_end), "pairs": pairs})
        cursor = payload_end


def candidate_headers(data: bytes, direct: set[int], selector: set[int], directory: set[int]) -> list[dict]:
    """Find grammar-compatible headers; compatibility is not execution evidence."""
    rows = []
    for header in range(START, END - 8, 2):
        alternate, inline = long(data, header), header + 4
        if not START <= alternate < END or alternate & 1:
            continue
        try:
            inline_stream = parse_stream(data, inline)
            alternate_stream = parse_stream(data, alternate)
        except ValueError:
            continue
        rows.append({"header": address(header), "inline_stream": address(inline),
                     "alternate_stream": address(alternate),
                      "observed_direct_entry": header in direct,
                      "observed_selector_sample": header in selector,
                     "proven_directory_target": header in directory,
                     "inline_pair_count": sum(batch["count"] for batch in inline_stream["batches"]),
                     "alternate_pair_count": sum(batch["count"] for batch in alternate_stream["batches"]),
                     "inline_terminator": inline_stream["terminator"],
                     "alternate_terminator": alternate_stream["terminator"]})
    return rows


def markdown(report: dict) -> str:
    streams = report["streams"]
    return "\n".join([
        "# Static segment-68 streams reachable from traced M-map headers",
        "",
        "Classification: **structurally decoded static packet payload rooted at",
        "scenario-observed headers and byte-exact directory targets**. The decoder follows the byte-exact count/threshold",
        "grammar used by `$C2AF46`; it does not claim every reachable stream rendered in",
        "one frame, represents terrain, or has global flight-map placement.",
        "",
        f"{report['direct_header_count']} direct `$C2AF00` headers plus",
        f"{report['selector_only_header_count']} additional `$C2AF40` selector-sampled",
        f"headers and {report['directory_only_header_count']} directory-only header targets expose {len(streams)} distinct inline/alternate stream starts. Their",
        f"complete structural walks contain {report['pair_count']} pair records occupying",
        f"{report['pair_bytes']} payload bytes ({report['pair_byte_fraction']:.2%} of the",
        "segment). This expands static *reachable-format* coverage beyond dynamically",
        "consumed pairs; it is deliberately reported separately from trace coverage.",
        "",
        "| Stream | roles | batches | pairs | terminator |",
        "| --- | --- | ---: | ---: | --- |",
        *[f"| `{row['start']}` | {', '.join(row['roles'])} | {len(row['batches'])} | "
          f"{sum(batch['count'] for batch in row['batches'])} | `{row['terminator']}` |"
          for row in streams],
        "",
        "A negative non-`$FFFF` prefix is retained as a threshold word because the",
        "renderer clears its sign bit, scales it by four, compares it with the live depth",
        "metric, and then reads the following count. It is not decoded as elevation or",
        "as a terrain LOD distance. A positive prefix is directly the count. All pairs",
        "remain signed two-word source values; depth is computed later by the renderer.",
        "",
        "Authority: direct-header inventories generated from sealed traces and the",
        "byte-exact `$C2AEFC-$C2AFF7` reader/stream selector reconstruction.",
        "",
        "The exact decoded streams are also available as [raw-pair inspection image]",
        "(../plots/static_m_map_packet_streams.png) and [OBJ line groups]",
        "(../exports/static_m_map_packet_streams.obj). The OBJ writes `(x, 0, y)`",
        "solely as a viewer carrier and does not infer faces or game-space elevation.",
        "",
        "## Directory-backed header coverage",
        "",
        f"A conservative even-address scan finds {report['candidate_header_count']} locations",
        f"whose leading longword points inside segment 68 and whose inline and alternate",
        f"streams both complete under the exact count/threshold grammar. {report['observed_candidate_count']}",
        "have dynamic renderer evidence or a non-reject static directory target in the sealed runs.",
        "The wide 32×32 directory supplies a byte-exact producer path for the 45",
        "otherwise unexecuted headers. Therefore no grammar-compatible header remains",
        "unrooted in the current scan; this promotes packet structure, not terrain identity",
        "or evidence that every stream rendered in one draw.",
        "",
        "The JSON retains every header and its direct, selector, and directory evidence.",
        "",
    ])


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--inventory", type=Path, action="append", required=True)
    parser.add_argument("--selector-samples", type=Path, action="append", default=[],
                        help="C2AF40 sample JSON; proves a selected header but not necessarily a C2AF00 entry")
    parser.add_argument("--directory", type=Path, action="append", default=[],
                        help="decoded C2AD relative-offset directory JSON; non-reject targets are static header evidence")
    parser.add_argument("--slow", type=Path, required=True)
    parser.add_argument("--output-json", type=Path,
                        default=ROOT / "analysis/data/static_m_map_packet_streams.json")
    parser.add_argument("--output-markdown", type=Path,
                        default=ROOT / "analysis/data/static_m_map_packet_streams.md")
    parser.add_argument("--replace", action="store_true",
                        help="replace existing generated outputs")
    args = parser.parse_args()
    if not args.replace:
        for path in (args.output_json, args.output_markdown):
            if path.exists():
                raise FileExistsError(path)
    data = args.slow.read_bytes()
    if len(data) != 0x80000:
        raise ValueError(f"expected 512 KiB slow RAM, got {len(data)} bytes")
    direct_headers: dict[int, set[str]] = {}
    selector_headers: dict[int, set[str]] = {}
    directory_headers: dict[int, set[str]] = {}
    for inventory in args.inventory:
        source = str(inventory.resolve().relative_to(ROOT))
        report = json.loads(inventory.read_text(encoding="utf-8"))
        for row in report["packet_streams"]:
            direct_headers.setdefault(int(row["header"][1:], 16), set()).add(source)
    for samples_path in args.selector_samples:
        source = str(samples_path.resolve().relative_to(ROOT))
        samples = json.loads(samples_path.read_text(encoding="utf-8"))
        for row in samples["samples"]:
            selector_headers.setdefault(int(row["header"][1:], 16), set()).add(source)
    for directory_path in args.directory:
        source = str(directory_path.resolve().relative_to(ROOT))
        directory = json.loads(directory_path.read_text(encoding="utf-8"))
        for row in directory["cells"]:
            normal_target = row.get("entry_classification") == "nonnegative_packet_entry_candidate"
            wide_target = row.get("classification") == "nonnegative_target_word"
            if not (normal_target or wide_target):
                continue
            directory_headers.setdefault(int(row["target"][1:], 16), set()).add(source)
    headers: dict[int, set[str]] = {}
    for header in set(direct_headers) | set(selector_headers) | set(directory_headers):
        headers[header] = (direct_headers.get(header, set()) | selector_headers.get(header, set())
                           | directory_headers.get(header, set()))
    streams: dict[int, dict] = {}
    for header, inventories in sorted(headers.items()):
        alternate, inline = long(data, header), header + 4
        for role, start in (("inline", inline), ("alternate", alternate)):
            if not START <= start < END:
                raise ValueError(f"{role} stream {address(start)} from header {address(header)} is outside segment")
            entry = streams.setdefault(start, parse_stream(data, start))
            entry.setdefault("roles", set()).add(role)
            entry.setdefault("headers", set()).add(header)
            entry.setdefault("inventories", set()).update(inventories)
    rows = []
    for start, entry in sorted(streams.items()):
        row = {key: value for key, value in entry.items() if key not in {"roles", "headers", "inventories"}}
        row["roles"] = sorted(entry["roles"])
        row["headers"] = [address(value) for value in sorted(entry["headers"])]
        row["inventories"] = sorted(entry["inventories"])
        rows.append(row)
    pair_addresses = {address(item) for row in rows for batch in row["batches"]
                      for item in range(int(batch["pair_start"][1:], 16),
                                        int(batch["pair_end_exclusive"][1:], 16), 4)}
    report = {"classification": "static_segment68_streams_reachable_from_traced_headers_not_complete_map",
              "segment": {"start": address(START), "end_exclusive": address(END), "bytes": END - START},
              "inventories": [str(path.resolve().relative_to(ROOT)) for path in args.inventory],
              "header_count": len(headers), "headers": [address(value) for value in sorted(headers)],
              "streams": rows, "pair_count": len(pair_addresses), "pair_bytes": len(pair_addresses) * 4,
              "pair_byte_fraction": len(pair_addresses) * 4 / (END - START)}
    candidates = candidate_headers(data, set(direct_headers), set(selector_headers), set(directory_headers))
    report["candidate_header_count"] = len(candidates)
    report["observed_candidate_count"] = sum(row["observed_direct_entry"] or row["observed_selector_sample"] or row["proven_directory_target"]
                                             for row in candidates)
    report["header_candidates"] = candidates
    report["direct_header_count"] = len(direct_headers)
    report["selector_only_header_count"] = len(set(selector_headers) - set(direct_headers))
    report["directory_only_header_count"] = len(set(directory_headers) - set(direct_headers) - set(selector_headers))
    args.output_json.parent.mkdir(parents=True, exist_ok=True)
    args.output_json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    args.output_markdown.write_text(markdown(report), encoding="utf-8")
    print(json.dumps({"headers": len(headers), "streams": len(rows), "pairs": len(pair_addresses),
                      "header_candidates": len(candidates)}))


if __name__ == "__main__":
    main()
