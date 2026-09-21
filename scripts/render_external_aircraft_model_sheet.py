"""Render the trace-proven C34A9A polygon family as a readable PNG model sheet."""
from __future__ import annotations

import json
import argparse
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_INPUT = ROOT / "build" / "run031_frame7500_external_polygon_submissions" / "polygon_submissions.json"
DEFAULT_OUTPUT = ROOT / "analysis" / "plots" / "external_aircraft_c3515e_c34a9a_model_sheet.png"


def font(size: int):
    return ImageFont.truetype("C:/Windows/Fonts/consola.ttf", size)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, default=DEFAULT_INPUT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--context", action="append")
    parser.add_argument("--line-input", type=Path,
                        help="optional C212B0 line-submission JSON to overlay")
    parser.add_argument("--line-context", action="append",
                        help="C212B0 A5 context(s); defaults to --context")
    parser.add_argument("--instance-source", action="append",
                        help="select one source batch from collect_matrix_instance_geometry output")
    parser.add_argument("--occurrence", type=int,
                        help="select one occurrence within an instance-source batch")
    parser.add_argument("--title", default="External-aircraft model candidate: $C3515E -> $C34A9A")
    parser.add_argument("--fit", action="store_true", help="centre and scale each view to its observed geometry")
    args = parser.parse_args()
    args.input = args.input.resolve()
    args.output = args.output.resolve()
    contexts = args.context or ["$C34A9A"]
    report = json.loads(args.input.read_text(encoding="utf-8"))
    embedded_lines = []
    if "instances" in report:
        if not args.instance_source:
            raise ValueError("--instance-source is required for instance-geometry input")
        requested_sources = {source.upper() for source in args.instance_source}
        instances = [item for item in report["instances"]
                     if item["source"].upper() in requested_sources]
        if args.occurrence is not None:
            instances = [item for item in instances if item.get("occurrence") == args.occurrence]
        if not instances:
            raise ValueError(f"instance source(s) {sorted(requested_sources)} not found")
        polygons = [submission["triples"] for instance in instances for submission in instance["polygons"]
                    if submission["context_a5"] in contexts]
        line_contexts = args.line_context or contexts
        embedded_lines = [tuple(tuple(point) for point in segment["triples"])
                          for instance in instances for submission in instance["lines"] if submission["context_a5"] in line_contexts
                          for segment in submission["segments"]]
    else:
        polygons = [submission["triples"] for submission in report["submissions"]
                    if submission["context"]["a5"] in contexts]
    if not polygons:
        raise ValueError(f"no selected-context polygons in {args.input}")
    line_segments = list(embedded_lines)
    if args.line_input:
        line_report = json.loads(args.line_input.resolve().read_text(encoding="utf-8"))
        line_contexts = args.line_context or contexts
        for submission in line_report["submissions"]:
            if submission["context"]["a5"] in line_contexts:
                line_segments.extend(tuple(tuple(point) for point in segment["triples"])
                                     for segment in submission["segments"])
    points = [tuple(point) for polygon in polygons for point in polygon]
    points.extend(point for segment in line_segments for point in segment)
    edge_count = sum(len(polygon) for polygon in polygons)
    extent = max(abs(value) for point in points for value in point)
    width, height, panel, margin = 1800, 760, 520, 60
    image = Image.new("RGB", (width, height), "#101419")
    draw = ImageDraw.Draw(image)
    title_font, text_font, small_font = font(28), font(18), font(15)
    draw.text((32, 24), args.title, fill="#f1f5f9", font=title_font)
    draw.text(
        (32, 64),
        f"{len(polygons)} filled-polygon outlines / {edge_count} polygon edges / {len(line_segments)} line segments / view-dependent transformed coordinates",
        fill="#aab7c4",
        font=text_font,
    )
    views = (("X-Y", 0, 1), ("X-Z", 0, 2), ("Y-Z", 1, 2))
    colours = ("#65d5ff", "#ffcc66", "#8ee28e", "#ff8fab", "#c9a7ff", "#f7e36b",
               "#70e0c4", "#ffad70", "#a4c2f4", "#f4a4d7", "#d8f4a4")
    for index, (label, horizontal, vertical) in enumerate(views):
        left, top = 40 + index * 590, 150
        centre_x, centre_y = left + panel / 2, top + panel / 2
        if args.fit:
            values_x = [point[horizontal] for point in points]
            values_y = [point[vertical] for point in points]
            model_x = (min(values_x) + max(values_x)) / 2
            model_y = (min(values_y) + max(values_y)) / 2
            view_extent = max(1, max(max(values_x) - min(values_x), max(values_y) - min(values_y)) / 2)
        else:
            model_x = model_y = 0
            view_extent = extent
        scale = (panel - 2 * margin) / (2 * view_extent * 1.1)
        draw.rectangle((left, top, left + panel, top + panel), fill="#151c24", outline="#506070", width=2)
        origin_x, origin_y = centre_x - model_x * scale, centre_y + model_y * scale
        if left + margin <= origin_y <= top + panel - margin:
            draw.line((left + margin, origin_y, left + panel - margin, origin_y), fill="#536579", width=1)
        if top + margin <= origin_x <= left + panel - margin:
            draw.line((origin_x, top + margin, origin_x, top + panel - margin), fill="#536579", width=1)
        draw.text((left + 12, top + 12), label, fill="#f1f5f9", font=title_font)
        draw.text((left + 12, top + panel - 30), f"view span {view_extent * 2:.0f}", fill="#aab7c4", font=small_font)
        for polygon_index, polygon in enumerate(polygons):
            projected = [(centre_x + (point[horizontal] - model_x) * scale,
                          centre_y - (point[vertical] - model_y) * scale)
                         for point in polygon]
            projected.append(projected[0])
            draw.line(projected, fill=colours[polygon_index % len(colours)], width=3, joint="curve")
            for x, y in projected[:-1]:
                draw.ellipse((x - 3, y - 3, x + 3, y + 3), fill="#ffffff")
        for first, second in line_segments:
            draw.line((centre_x + (first[horizontal] - model_x) * scale,
                       centre_y - (first[vertical] - model_y) * scale,
                       centre_x + (second[horizontal] - model_x) * scale,
                       centre_y - (second[vertical] - model_y) * scale),
                      fill="#e8edf3", width=2)
    draw.text((32, 705), "Connectivity is renderer-observed; coloured outlines are polygons and white strokes are C212B0 lines. No missing faces or links are inferred.", fill="#aab7c4", font=text_font)
    args.output.parent.mkdir(exist_ok=True)
    image.save(args.output)
    print(f"wrote {args.output.relative_to(ROOT)} ({len(polygons)} polygons)")


if __name__ == "__main__":
    main()
