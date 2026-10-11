"""Render captured M-map vector emissions without tracing its completed bitmap.

The SVG is intentionally a renderer-input view: every <line> comes from a
C2FA7E entry in the sealed transition trace.  The Golden Gate callout is the
one scenario-backed landmark join currently available: its two source/control
contexts are the independently correlated $C3559A/$C355D2 bridge lines.

This does not replay C304F4 polygon fill jobs.  Their line-mode parameters are
captured, but their complete source-to-pixel/fill-colour contract is not yet
decoded; representing the finished bitplanes as polygons here would be a
bitmap conversion rather than a vector extraction.
"""
from __future__ import annotations

import argparse
import html
import json
from pathlib import Path


LINE_EMIT = 0xC2FA7E
GOLDEN_GATE_CONTEXTS = {0xC3559A, 0xC355D2}


def signed_word(value: int) -> int:
    value &= 0xFFFF
    return value - 0x10000 if value & 0x8000 else value


def hex6(value: int) -> str:
    return f"${value & 0xFFFFFF:06X}"


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--trace", type=Path,
                        default=Path("build/run003_m_map_appearance_trace/trace.jsonl"))
    parser.add_argument("--svg", type=Path,
                        default=Path("analysis/visuals/run003_m_map_renderer_vectors.svg"))
    parser.add_argument("--output", type=Path,
                        default=Path("analysis/data/run003_m_map_renderer_vectors.json"))
    args = parser.parse_args()
    if args.svg.exists() or args.output.exists():
        raise FileExistsError("refusing to overwrite existing vector evidence output")

    rows = [json.loads(line) for line in args.trace.read_text(encoding="utf-8").splitlines()]
    vectors = []
    for row in rows:
        if row["pc"] != LINE_EMIT:
            continue
        registers = row["registers"]
        endpoints = [signed_word(registers[f"d{index}"]) for index in range(4)]
        context = registers["a5"] & 0xFFFFFF
        vectors.append({
            "trace_index": row["index"], "frame": row["frame"],
            "context": hex6(context), "endpoints": endpoints,
            "golden_gate_context": context in GOLDEN_GATE_CONTEXTS,
        })
    if not vectors:
        raise RuntimeError("no C2FA7E vector emissions in supplied trace")

    landmark_vectors = [item for item in vectors if item["golden_gate_context"]]
    if len(landmark_vectors) != 2:
        raise RuntimeError(f"expected two Golden Gate vector contexts, found {len(landmark_vectors)}")
    landmark_x = sum((item["endpoints"][0] + item["endpoints"][2]) / 2 for item in landmark_vectors) / len(landmark_vectors)
    landmark_y = sum((item["endpoints"][1] + item["endpoints"][3]) / 2 for item in landmark_vectors) / len(landmark_vectors)

    report = {
        "scope": "C2FA7E vector emissions in sealed run003 M-map transition",
        "authority": str(args.trace),
        "coordinate_space": "320x180 renderer screen coordinates",
        "vector_count": len(vectors),
        "vectors": vectors,
        "landmarks": [{
            "name": "Golden Gate", "evidence": "$C3559A/$C355D2 bridge contexts",
            "map_anchor": [landmark_x, landmark_y],
            "qualification": "Source/control identity is scenario-backed; anchor is this run003 M-map view only.",
        }],
        "qualification": (
            "The SVG is a direct vector-emission view, not a bitplane trace. It omits C304F4 polygon fills because "
            "their source-to-fill-colour contract remains unresolved. No other landmark position is claimed."
        ),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")

    lines = [
        '<svg xmlns="http://www.w3.org/2000/svg" width="960" height="540" viewBox="0 0 320 180">',
        '<title>run003 M-map renderer vector emissions</title>',
        '<desc>Only captured C2FA7E vectors. Golden Gate label is tied to C3559A/C355D2.</desc>',
        '<rect width="320" height="180" fill="#003366"/>',
        '<g fill="none" stroke="#d0d0d0" stroke-width="0.75" vector-effect="non-scaling-stroke">',
    ]
    for item in vectors:
        x1, y1, x2, y2 = item["endpoints"]
        context = html.escape(item["context"])
        lines.append(f'<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}"><title>{context}, trace {item["trace_index"]}</title></line>')
    lines.extend([
        '</g>',
        f'<circle cx="{landmark_x:.2f}" cy="{landmark_y:.2f}" r="3" fill="#e53935" stroke="#fff" stroke-width="0.7"/>',
        f'<path d="M {landmark_x + 3:.2f} {landmark_y - 3:.2f} L {landmark_x + 26:.2f} {landmark_y - 17:.2f}" stroke="#e53935" stroke-width="0.8"/>',
        f'<text x="{landmark_x + 28:.2f}" y="{landmark_y - 18:.2f}" fill="#ffffff" font-family="sans-serif" font-size="7">Golden Gate</text>',
        '<text x="4" y="174" fill="#b8c7d9" font-family="sans-serif" font-size="5">Captured renderer vectors only — polygon fills not yet decoded</text>',
        '</svg>',
    ])
    args.svg.parent.mkdir(parents=True, exist_ok=True)
    args.svg.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(json.dumps({"vectors": len(vectors), "golden_gate_anchor": [landmark_x, landmark_y]}))


if __name__ == "__main__":
    main()
