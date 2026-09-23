"""Export selector-proven static M-map packet streams for visual inspection.

Each decoded batch becomes one OBJ line group.  OBJ's middle coordinate is zero
only to carry the proven two-word source pairs in a conventional 3-D viewer;
it is not a reconstructed game-space height or terrain mesh.
"""
from __future__ import annotations

import argparse
import json
import math
from pathlib import Path

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[1]
SIZE, MARGIN = 1000, 68


def round_down(value: int) -> int:
    return math.floor(value / 512) * 512


def round_up(value: int) -> int:
    return math.ceil(value / 512) * 512


def batches(report: dict) -> list[dict]:
    result = []
    for stream in report["streams"]:
        for index, batch in enumerate(stream["batches"], start=1):
            result.append({"stream": stream["start"], "batch": index, "pairs": batch["pairs"]})
    return result


def write_obj(rows: list[dict], output: Path) -> None:
    lines = [
        "# Selector-proven static M-map packet streams from segment 68.",
        "# OBJ coordinate convention: (source_x, 0, source_y).",
        "# The zero middle coordinate is a viewer carrier only; no faces or inter-batch joins are inferred.",
    ]
    base = 1
    for row in rows:
        lines.append(f"o packet_{row['stream'][1:]}_batch_{row['batch']:02d}")
        lines.extend(f"v {pair[0]} 0 {pair[1]}" for pair in row["pairs"])
        if len(row["pairs"]) > 1:
            lines.append("l " + " ".join(str(base + item) for item in range(len(row["pairs"]))))
        base += len(row["pairs"])
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_png(rows: list[dict], output: Path) -> dict:
    pairs = [pair for row in rows for pair in row["pairs"]]
    minimum_x, maximum_x = round_down(min(pair[0] for pair in pairs)), round_up(max(pair[0] for pair in pairs))
    minimum_y, maximum_y = round_down(min(pair[1] for pair in pairs)), round_up(max(pair[1] for pair in pairs))
    span = max(maximum_x - minimum_x, maximum_y - minimum_y, 1)

    def project(pair: list[int]) -> tuple[float, float]:
        return (MARGIN + (pair[0] - minimum_x) * (SIZE - 2 * MARGIN) / span,
                SIZE - MARGIN - (pair[1] - minimum_y) * (SIZE - 2 * MARGIN) / span)

    image = Image.new("RGB", (SIZE, SIZE), "#10151c")
    draw = ImageDraw.Draw(image)
    for value in range(minimum_x, maximum_x + 1, 512):
        x, _ = project([value, minimum_y])
        draw.line((x, MARGIN, x, SIZE - MARGIN), fill="#2b3542")
        draw.text((x + 2, SIZE - MARGIN + 6), str(value), fill="#8fa4b8")
    for value in range(minimum_y, maximum_y + 1, 512):
        _, y = project([minimum_x, value])
        draw.line((MARGIN, y, SIZE - MARGIN, y), fill="#2b3542")
        draw.text((6, y - 6), str(value), fill="#8fa4b8")
    for row in rows:
        points = [project(pair) for pair in row["pairs"]]
        if len(points) > 1:
            draw.line(points, fill="#55d6be", width=1)
        for x, y in points:
            draw.ellipse((x - 1, y - 1, x + 1, y + 1), fill="#ffb454")
    draw.rectangle((MARGIN, MARGIN, SIZE - MARGIN, SIZE - MARGIN), outline="#d5e0ea")
    draw.text((MARGIN, 16), "Selector-proven M-map static packet streams (raw source pairs)", fill="#f1f5f9")
    draw.text((MARGIN, 34), "Lines retain only same-batch order; no closure, faces, placement, or unobserved packets inferred.", fill="#cbd5e1")
    output.parent.mkdir(parents=True, exist_ok=True)
    image.save(output)
    return {"x": [minimum_x, maximum_x], "y": [minimum_y, maximum_y]}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path,
                        default=ROOT / "analysis/data/static_m_map_packet_streams.json")
    parser.add_argument("--output-obj", type=Path,
                        default=ROOT / "analysis/exports/static_m_map_packet_streams.obj")
    parser.add_argument("--output-png", type=Path,
                        default=ROOT / "analysis/plots/static_m_map_packet_streams.png")
    parser.add_argument("--replace", action="store_true")
    args = parser.parse_args()
    if not args.replace:
        for path in (args.output_obj, args.output_png):
            if path.exists():
                raise FileExistsError(path)
    report = json.loads(args.input.read_text(encoding="utf-8"))
    rows = batches(report)
    write_obj(rows, args.output_obj)
    bounds = write_png(rows, args.output_png)
    print(json.dumps({"streams": len(report["streams"]), "batches": len(rows),
                      "pairs": sum(len(row["pairs"]) for row in rows), "bounds": bounds}))


if __name__ == "__main__":
    main()
