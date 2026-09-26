"""Extract per-blit destination writes from a widened Engine9000 DMA capture."""
from pathlib import Path
import argparse, json

PAGE_FIRST = 0x12BC0
PAGE_LAST = 0x1A8C0

def main():
    p = argparse.ArgumentParser()
    p.add_argument("capture", type=Path)
    p.add_argument("output", type=Path)
    a = p.parse_args()
    records = json.loads(a.capture.read_text())["selected_records"]
    starts = [i for i, r in enumerate(records) if r["addr"] == "DFF058"]
    jobs = []
    for job, start in enumerate(starts):
        end = starts[job + 1] if job + 1 < len(starts) else len(records)
        writes = []
        for r in records[start:end]:
            address = int(r["addr"], 16)
            if r["type"] == 5 and PAGE_FIRST <= address < PAGE_LAST:
                writes.append({"address": r["addr"], "data": r["dat"],
                               "vpos": r["vpos"], "hpos": r["hpos"],
                               "extra": r["extra"]})
        jobs.append({"job": job, "submission_index": records[start]["index"],
                     "writes": writes})
    a.output.write_text(json.dumps({"count": len(jobs), "jobs": jobs}, indent=2) + "\n")
    print(json.dumps({"count": len(jobs), "writes": sum(len(j["writes"]) for j in jobs)}))

if __name__ == "__main__":
    main()
