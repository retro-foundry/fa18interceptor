"""Verify transferred M-map Golden Gate anchors against the red bridge pixels."""
from __future__ import annotations

import json
from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
MAP_ORIGIN = (40, 16)
SAMPLES = (
    ("run003", ROOT / "analysis/visuals/run003_m_map_display.png", (194, 59.5)),
    ("run035", ROOT / "analysis/visuals/run035_m_map_display.png", (336, 95.5)),
)


def main() -> None:
    output = ROOT / "analysis/data/m_map_golden_gate_anchor_validation.json"
    if output.exists():
        raise FileExistsError("refusing to overwrite validation evidence")
    rows = []
    for name, path, (x, y) in SAMPLES:
        image = Image.open(path).convert("RGB")
        red = [(px, py) for py in range(image.height) for px in range(image.width)
               if image.getpixel((px, py)) == (136, 0, 0)]
        if not red:
            raise RuntimeError(f"{name}: no exact red bridge pixels")
        bounds = (min(px for px, _ in red), min(py for _, py in red),
                  max(px for px, _ in red), max(py for _, py in red))
        screen_anchor = (x + MAP_ORIGIN[0], y + MAP_ORIGIN[1])
        hit = bounds[0] <= screen_anchor[0] <= bounds[2] and bounds[1] <= screen_anchor[1] <= bounds[3]
        if not hit:
            raise AssertionError(f"{name}: anchor {screen_anchor} outside red bounds {bounds}")
        rows.append({"capture": name, "map_anchor": [x, y], "screen_anchor": list(screen_anchor),
                     "red_rgb": "#880000", "red_pixel_count": len(red), "red_bounds": list(bounds), "anchor_hits_bounds": hit})
    report = {"classification": "Golden_Gate_anchor_to_red_M_map_pixel_validation", "map_origin": list(MAP_ORIGIN),
              "samples": rows,
              "qualification": "This validates the anchor against the displayed red bridge-line bounds. It does not identify all bridge geometry or convert the map to absolute world coordinates."}
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"samples": len(rows), "output": str(output)}))


if __name__ == "__main__":
    main()
