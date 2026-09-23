"""Compare immutable C1F4AC matrix inputs across sealed M-map trace windows."""
from __future__ import annotations

import argparse
import json
from pathlib import Path


ENTRY = 0xC1F4AC


def sources(path: Path) -> dict[str, object]:
    rows = [json.loads(line) for line in path.read_text(encoding="utf-8").splitlines()]
    entries = [row for row in rows if row["pc"] == ENTRY]
    by_source: dict[str, list[dict[str, int]]] = {}
    for row in entries:
        source = f"${row['registers']['a1'] & 0xFFFFFF:06X}"
        by_source.setdefault(source, []).append({"frame": row["frame"], "trace_index": row["index"]})
    return {"trace": str(path), "instruction_count": len(rows), "entries": len(entries),
            "sources": by_source}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline", type=Path, required=True)
    parser.add_argument("--extended", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--markdown", type=Path, required=True)
    args = parser.parse_args()
    baseline, extended = sources(args.baseline), sources(args.extended)
    base_set, ext_set = set(baseline["sources"]), set(extended["sources"])
    report = {
        "scope": "sealed no-input M-map C1F4AC immutable transform-source comparison",
        "entry": "$C1F4AC",
        "baseline": baseline,
        "extended": extended,
        "new_sources_in_extended_window": sorted(ext_set - base_set),
        "baseline_sources_absent_from_extended_window": sorted(base_set - ext_set),
        "qualification": ("An unchanged source set only characterizes these bounded stable-map windows. "
                          "It does not establish whole-map coverage, terrain identity, or LOD behavior."),
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = ["# Stable M-map transform-source window comparison", "", report["qualification"], "",
             f"Baseline: `{args.baseline}` — {baseline['instruction_count']:,} instructions, {baseline['entries']} `$C1F4AC` entries.",
             f"Extended: `{args.extended}` — {extended['instruction_count']:,} instructions, {extended['entries']} `$C1F4AC` entries.", "",
             "| Immutable source | baseline entries | extended entries |", "| --- | ---: | ---: |"]
    for source in sorted(base_set | ext_set):
        lines.append(f"| `{source}` | {len(baseline['sources'].get(source, []))} | {len(extended['sources'].get(source, []))} |")
    lines.extend(["", f"New sources in extended window: {', '.join(report['new_sources_in_extended_window']) or 'none'}."])
    args.markdown.parent.mkdir(parents=True, exist_ok=True)
    args.markdown.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(json.dumps({"baseline_sources": len(base_set), "extended_sources": len(ext_set),
                      "new_sources": len(ext_set - base_set)}))


if __name__ == "__main__":
    main()
