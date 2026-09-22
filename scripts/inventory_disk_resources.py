"""Hash and classify the game-owned files in the preserved ADF."""
from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import subprocess
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
RESOURCES = (
    ("F-18 Interceptor", "hunk_executable"),
    ("F-18 Interceptor.info", "workbench_icon_container"),
    ("config", "game_configuration_data"),
    ("pix/frnt5", "ILBM_graphics"),
    ("pix/inst5", "ILBM_graphics"),
    ("pix/splsh", "ILBM_graphics"),
    ("text/text201", "text_resource"), ("text/textcpt", "text_resource"),
    ("text/textctl", "text_resource"), ("text/textegn", "text_resource"),
    ("text/textegn2", "text_resource"), ("text/textger", "text_resource"),
    ("text/texti0a", "text_resource"), ("text/texti0b", "text_resource"),
    ("text/texti1", "text_resource"), ("text/texti2", "text_resource"),
    ("text/texti3", "text_resource"), ("text/texti4", "text_resource"),
    ("text/texti5", "text_resource"), ("text/textply", "text_resource"),
    ("text/texttre", "text_resource"), ("text/textwnd", "text_resource"),
)


def read_file(xdftool: str, disk: Path, source: str) -> bytes:
    with tempfile.TemporaryDirectory(prefix="fa18-adf-resource-") as temporary:
        target = Path(temporary) / "resource.bin"
        subprocess.run([xdftool, str(disk), "read", source, str(target)], check=True,
                       capture_output=True, text=True)
        return target.read_bytes()


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--disk", type=Path, default=ROOT / "local" / "media" / "fa18.adf")
    parser.add_argument("--xdftool", default="xdftool.exe")
    parser.add_argument("--output", type=Path, default=ROOT / "analysis" / "disk_game_resources.json")
    parser.add_argument("--markdown", type=Path, default=ROOT / "analysis" / "disk_game_resources.md")
    args = parser.parse_args()
    if not args.disk.is_file():
        raise FileNotFoundError(args.disk)
    xdftool = shutil.which(args.xdftool) or args.xdftool
    rows = []
    for path, classification in RESOURCES:
        data = read_file(xdftool, args.disk, path)
        row = {"adf_path": path, "classification": classification,
               "size_bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()}
        if classification == "workbench_icon_container":
            row["format"] = "Amiga DiskObject"
            row["format_magic"] = data[:2].hex().upper()
            row["format_version"] = int.from_bytes(data[2:4], "big")
            row["separation"] = (
                "Workbench desktop icon container; not one of the game's pix/ ILBM resources "
                "and not evidence for a game renderer input.")
        rows.append(row)
    manifest = {"authority_disk": str(args.disk),
                "authority_disk_sha256": hashlib.sha256(args.disk.read_bytes()).hexdigest(),
                "scope": "game-owned executable, graphics, text, configuration, and icon files; excludes AmigaOS support tree",
                "resources": rows}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(manifest, indent=2) + "\n")
    grouped = {}
    for row in rows:
        grouped.setdefault(row["classification"], []).append(row)
    lines = ["# Game disk resource inventory", "", manifest["scope"], ""]
    for classification, group in grouped.items():
        lines += [f"## {classification}", "", "| ADF path | Bytes | SHA-256 |", "| --- | ---: | --- |"]
        lines += [f"| `{row['adf_path']}` | {row['size_bytes']:,} | `{row['sha256']}` |" for row in group]
        for row in group:
            if "separation" in row:
                lines += ["", f"`{row['adf_path']}`: {row['format']} magic `0x{row['format_magic']}`, version {row['format_version']}. {row['separation']}"]
        lines.append("")
    lines += ["The executable remains in the Hunk/code-data boundary inventory. ILBMs are detailed in `analysis/disk_graphics_assets.md`; text resources remain opaque data until their consumers are traced.", ""]
    args.markdown.parent.mkdir(parents=True, exist_ok=True)
    args.markdown.write_text("\n".join(lines))
    print(f"wrote {args.output.relative_to(ROOT)}")
    print(f"wrote {args.markdown.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
