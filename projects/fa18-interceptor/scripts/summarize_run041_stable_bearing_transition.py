"""Summarize bounded C1F4AC source sets across the sealed run041 approach."""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SAMPLES = [
    (5000, ROOT / "build/run041_frame05000_12f_trace/trace.jsonl", "trace"),
    (5750, ROOT / "build/run041_frame05750_matrix_entries/matrix_transform_entries.json", "entries"),
    (6000, ROOT / "build/run041_frame06000_matrix_entries/matrix_transform_entries.json", "entries"),
    (6250, ROOT / "build/run041_frame06250_12f_trace_retry/trace.jsonl", "trace"),
]
RED_PIXELS = {5000: 72, 5750: 166, 6000: 296, 6250: 674}


def main() -> None:
    output = ROOT / "analysis/data/run041_stable_bearing_transition_bracket.json"
    if output.exists() or output.with_suffix(".md").exists():
        raise FileExistsError(output)
    rows = []
    for frame, path, kind in SAMPLES:
        if kind == "trace":
            entries = [json.loads(line) for line in path.read_text(encoding="utf-8").splitlines()]
            sources = [f"${row['registers']['a1'] & 0xFFFFFF:06X}" for row in entries if row["pc"] == 0xC1F4AC]
        else:
            entries = json.loads(path.read_text(encoding="utf-8"))["entries"]
            sources = [row["a1"] for row in entries]
        rows.append({"replay_frame": frame, "red_pixels": RED_PIXELS[frame],
                     "entry_count": len(sources), "sources": sorted(set(sources)), "authority": str(path.relative_to(ROOT)).replace("\\", "/")})
    report = {"classification": "stable_bearing_landmark_detail_transition_bracket_not_distance_threshold",
              "samples": rows,
              "qualification": "Run041 keeps the landmark near the viewport centre while it grows. These bounded source samples locate changing renderer input sets, but do not measure physical distance or isolate all camera variables; they are not physical-distance LOD proof."}
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Run041 stable-bearing detail transition bracket", "",
             "Classification: **scenario-backed source-family transition bracket; not physical-distance LOD proof**.", "",
             report["qualification"], "", "| Replay frame | red pixels | `$C1F4AC` entries | distinct immutable inputs |", "| ---: | ---: | ---: | --- |"]
    for row in rows:
        lines.append(f"| {row['replay_frame']:,} | {row['red_pixels']} | {row['entry_count']} | " + ", ".join(f"`{source}`" for source in row["sources"]) + " |")
    lines.extend(["", "The frame-5,750 and frame-6,000 samples show mixed input sets between the earlier ten-source and later three-source endpoints. The source change is therefore bracketed across the measured landmark growth, but no selector threshold or physical range is established.", ""])
    output.with_suffix(".md").write_text("\n".join(lines), encoding="utf-8")
    print(json.dumps({"samples": len(rows), "endpoint_sources": [len(rows[0]['sources']), len(rows[-1]['sources'])]}))


if __name__ == "__main__":
    main()
