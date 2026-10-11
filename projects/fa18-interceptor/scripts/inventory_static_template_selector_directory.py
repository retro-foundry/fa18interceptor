"""Inventory the fixed 256-entry relative selector directory at $C42390."""
from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SLOW = ROOT / "build/run033_origin_control_trace/slow.bin"
SLOW_BASE = 0xC00000
TABLE_BASE = 0xC42390
TABLE_ENTRIES = 256
TABLE_END = TABLE_BASE + TABLE_ENTRIES * 2


def address(value: int) -> str:
    return f"${value:06X}"


def main() -> None:
    slow = SLOW.read_bytes()
    entries = []
    for index in range(TABLE_ENTRIES):
        table_word = TABLE_BASE + index * 2
        offset = table_word - SLOW_BASE
        relative = int.from_bytes(slow[offset:offset + 2], "big", signed=True)
        target = table_word + relative
        target_word = int.from_bytes(slow[target - SLOW_BASE:target - SLOW_BASE + 2], "big", signed=True)
        entries.append({
            "selector_index": f"${index:02X}",
            "table_word": address(table_word),
            "relative_word": f"${relative & 0xFFFF:04X}",
            "target": address(target),
            "target_first_word_signed": target_word,
            "target_gate": "reject" if target_word < 0 else "nonnegative_candidate",
        })
    payload = {
        "authority": {
            "slow_snapshot": str(SLOW.relative_to(ROOT)).replace("\\", "/"),
            "table_base": address(TABLE_BASE),
            "table_end_exclusive": address(TABLE_END),
            "entry_count": TABLE_ENTRIES,
            "consumer": "$C1D400-$C1D408",
        },
        "classification": "static_relative_selector_directory_not_global_coordinate_or_lod_table",
        "entries": entries,
    }
    json_path = ROOT / "analysis/data/static_template_selector_directory.json"
    markdown_path = ROOT / "analysis/data/static_template_selector_directory.md"
    json_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    rejected = sum(row["target_gate"] == "reject" for row in entries)
    lines = [
        "# Static template-selector directory",
        "",
        "Classification: **static structural directory with scenario-backed consumer**. "
        "This is the complete 256-word relative-offset table consumed by `$C1D400-$C1D408`; "
        "it is not a global terrain-coordinate table or an LOD table.",
        "",
        "Authority: `build/run033_origin_control_trace/slow.bin`. `$C1D400` doubles the "
        "caller's selector value and `$C1D402` adds the signed word at "
        "`$C42390 + selector*2` to the table-word address. `$C1D406-$C1D408` rejects "
        "targets whose first word is negative. The 256 entries occupy `$C42390-$C4258F`; "
        "`$C42590` begins an independently decoded count/threshold/pointer group record.",
        "",
        f"The table has **{TABLE_ENTRIES}** entries; **{rejected}** have a negative first "
        "word and take the observed rejection gate. Nonnegative entries are only candidates: "
        "their later bit gate and row-key search still determine whether a template stream is used.",
        "",
        "| selector index | table word | signed relative word | target | target first word | first gate |",
        "| --- | --- | --- | --- | ---: | --- |",
    ]
    for row in entries:
        lines.append(f"| {row['selector_index']} | {row['table_word']} | {row['relative_word']} | "
                     f"{row['target']} | {row['target_first_word_signed']} | {row['target_gate']} |")
    lines += [
        "",
        "The observed terrain-page window uses only a small subset of this directory. "
        "See [the controlled origin probe](origin_selector_mutation_probe.md) for the "
        "two independent selector axes and [the per-call group inventories]"
        "(static_template_selector_groups_origin_control.md) for traced stream choices.",
        "",
    ]
    markdown_path.write_text("\n".join(lines), encoding="utf-8")
    print(f"wrote {json_path.relative_to(ROOT)} and {markdown_path.relative_to(ROOT)} "
          f"({TABLE_ENTRIES} entries, {rejected} rejected at first gate)")


if __name__ == "__main__":
    main()
