"""Build a labelled contact sheet from the renderer-observed model plots."""
from __future__ import annotations

from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
PLOTS = ROOT / "analysis" / "plots"
OUTPUT = PLOTS / "model_identification_gallery.png"

ITEMS = (
    ("External aircraft: C3515E -> C34A9A/C34A9C (42 pre-cull faces)",
     "external_aircraft_c34a9_preclip_complete_face_sheet.png"),
    ("Carrier/deck candidate: C39D2A -> C3925C/C3925E (10 pre-cull faces)",
     "c39d2a_c3925_preclip_complete_face_sheet.png"),
    ("Golden Gate assembled batches: C35932 + C3B720", "golden_gate_assembled_transform_batches_model_sheet.png"),
    ("Golden Gate bridge family: C355D8/C355D6 pre-clip", "golden_gate_c355_preclip_complete_combined_sheet.png"),
    ("Later bridge candidate: C362A2 -> C36298", "later_bridge_c36298_model_sheet.png"),
    ("Unidentified bridge-checkpoint family: C3B4FE", "bridge_checkpoint_c3b4fe_model_sheet.png"),
    ("Unidentified bridge-checkpoint family: C3B50A", "bridge_checkpoint_c3b50a_model_sheet.png"),
    ("Unidentified external-scene family: C38F98", "external_scene_c38f98_preclip_face_sheet.png"),
    ("Unidentified Golden Gate checkpoint family: C45BEA", "golden_gate_c45bea_preclip_face_sheet.png"),
    ("Compact polyhedral family: C3A94C/C3A94E (6 pre-cull faces)",
     "frame12600_c3a94e_preclip_face_sheet.png"),
)


def font(size: int):
    return ImageFont.truetype("C:/Windows/Fonts/consola.ttf", size)


def main() -> None:
    thumb_width, thumb_height = 800, 338
    columns, label_height, gap = 2, 46, 22
    rows = (len(ITEMS) + columns - 1) // columns
    image = Image.new("RGB", (columns * thumb_width + (columns + 1) * gap,
                              rows * (thumb_height + label_height) + (rows + 1) * gap + 58), "#0d1117")
    draw = ImageDraw.Draw(image)
    draw.text((gap, 14), "Renderer-observed model identification gallery", fill="#f1f5f9", font=font(24))
    for index, (label, filename) in enumerate(ITEMS):
        source = PLOTS / filename
        if not source.exists():
            raise FileNotFoundError(source)
        row, column = divmod(index, columns)
        left = gap + column * (thumb_width + gap)
        top = 58 + gap + row * (thumb_height + label_height + gap)
        draw.rectangle((left, top, left + thumb_width, top + label_height + thumb_height), fill="#151c24", outline="#506070")
        draw.text((left + 12, top + 12), label, fill="#f1f5f9", font=font(14))
        rendered = Image.open(source).convert("RGB")
        rendered.thumbnail((thumb_width - 8, thumb_height - 4))
        image.paste(rendered, (left + 4, top + label_height + 2))
    image.save(OUTPUT)
    print(f"wrote {OUTPUT.relative_to(ROOT)} ({len(ITEMS)} sheets)")


if __name__ == "__main__":
    main()
