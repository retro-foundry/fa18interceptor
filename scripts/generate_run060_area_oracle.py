"""Generate final destination words for the four frame 7992 area blits."""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[1]
BASES = (0x12BC0, 0x14B00, 0x16A40, 0x18980)
traces = json.loads((ROOT / "build/run060_dma_writes7992.json").read_text())["jobs"][:4]
jobs = json.loads((ROOT / "build/run060_dma_blits7992_wide.json").read_text())["jobs"][:4]
states = []
for job, trace in zip(jobs, traces):
    plane = max(i for i, base in enumerate(BASES) if job["d"] >= base)
    final = {}
    for item in trace["writes"]:
        final[int(item["address"], 16) - BASES[plane]] = int(item["data"], 16)
    states.append(final)
maximum = max(map(len, states))
lines = ["#ifndef FA18_RUN060_AREA_ORACLE_H", "#define FA18_RUN060_AREA_ORACLE_H", "#include <stdint.h>",
         f"enum {{ FA18_RUN060_AREA_MAX_WRITES = {maximum} }};",
         "typedef struct { uint16_t offset; uint16_t value; } FA18AreaOracleWrite;",
         "static const uint16_t fa18_run060_area_oracle_counts[4] = {",
         "    " + ", ".join(map(str, map(len, states))) + ",", "};",
         "static const FA18AreaOracleWrite fa18_run060_area_oracle_writes[4][FA18_RUN060_AREA_MAX_WRITES] = {"]
for state in states:
    lines.append("    {")
    lines.extend(f"        {{0x{offset:04x}, 0x{value:04x}}}," for offset, value in state.items())
    lines.append("    },")
lines += ["};", "#endif", ""]
(ROOT / "port/run060_area_oracle.h").write_text("\n".join(lines))
