"""Inventory normal/wide M-map directory initializer order in traces."""
from __future__ import annotations
import argparse, json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
NORMAL, WIDE = 0xC2AB5A, 0xC2AB34

def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--trace", type=Path, action="append", required=True)
    p.add_argument("--output", type=Path, default=ROOT / "analysis/data/m_map_directory_initializer_sequence.json")
    p.add_argument("--replace", action="store_true")
    a = p.parse_args(); md = a.output.with_suffix(".md")
    if not a.replace and (a.output.exists() or md.exists()): raise FileExistsError(a.output)
    traces=[]; normal_then_wide=0
    for path in a.trace:
        events=[]
        for line in path.read_text(encoding="utf-8").splitlines():
            x=json.loads(line)
            if x["pc"] in (NORMAL,WIDE): events.append({"trace_index":x["index"],"frame":x["frame"],"initializer":"normal" if x["pc"]==NORMAL else "wide"})
        normal_then_wide += sum(one["initializer"]=="normal" and two["initializer"]=="wide" for one,two in zip(events,events[1:]))
        traces.append({"trace":str(path.resolve().relative_to(ROOT)),"events":events})
    report={"classification":"scenario_backed_map_directory_initializer_sequence","normal_then_wide_pairs":normal_then_wide,"traces":traces,"qualification":"An adjacent normal/wide event pair proves ordering only within the bounded trace. It establishes sequential renderer preparation passes, not terrain ownership, physical map extent, or LOD."}
    a.output.parent.mkdir(parents=True,exist_ok=True); a.output.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    lines=["# M-map directory initializer sequence","","Classification: **scenario-backed initializer ordering**.","",report["qualification"],"",f"Observed adjacent normal-to-wide initializer pairs: {normal_then_wide}.","","| Trace | ordered initializer events |","| --- | --- |"]
    for row in traces: lines.append(f"| `{row['trace']}` | " + ", ".join(f"{e['initializer']} (frame {e['frame']})" for e in row["events"]) + " |")
    md.write_text("\n".join(lines)+"\n",encoding="utf-8"); print(json.dumps({"pairs":normal_then_wide,"traces":len(traces)}))
if __name__ == "__main__": main()
