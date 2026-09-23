"""Regenerate cumulative exact segment-68 pair-payload coverage."""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
INVENTORIES = [
    "run003_m_map_polygon_static_packets.json",
    "run035_m_map_polygon_static_packets.json",
    "run035_m_map_stable_polygon_static_packets.json",
    "run037_m_map_stable_polygon_static_packets.json",
]


def main() -> None:
    sources, headers, streams, pairs = [], set(), set(), set()
    for name in INVENTORIES:
        path = ROOT / "analysis/data" / name
        report = json.loads(path.read_text(encoding="utf-8"))
        current_headers = {row["header"] for row in report["packet_streams"]}
        current_streams = {row["selected_stream"] for row in report["packet_streams"]}
        current_pairs = {pair["address"] for batch in report["batches"]
                         for pair in batch["consumed_pairs"]}
        sources.append({"inventory": str(path.relative_to(ROOT)).replace("/", "\\\\"),
                        "direct_headers": len(current_headers),
                        "selected_streams": len(current_streams),
                        "unique_pairs": len(current_pairs)})
        headers |= current_headers
        streams |= current_streams
        pairs |= current_pairs
    segment_bytes = 0x444F8 - 0x42CA8
    output = {
        "scope": "immutable segment-68 map packet inputs exercised by listed bounded M-map inventories",
        "segment": {"start": "$C42CA8", "end_exclusive": "$C444F8", "bytes": segment_bytes},
        "sources": sources,
        "unique_direct_headers": sorted(headers),
        "unique_selected_streams": sorted(streams),
        "unique_pair_addresses": sorted(pairs),
        "unique_pair_bytes": len(pairs) * 4,
        "pair_byte_fraction": len(pairs) * 4 / segment_bytes,
        "qualification": "Coverage counts only exact pair payload bytes reached in these bounded map traces. It excludes packet headers, control words, unselected alternate streams, and all untraced segment data; it is not complete terrain-map coverage.",
    }
    path = ROOT / "analysis/data/m_map_segment68_trace_coverage.json"
    path.write_text(json.dumps(output, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({key: output[key] for key in ("unique_pair_bytes", "pair_byte_fraction")}))


if __name__ == "__main__":
    main()
