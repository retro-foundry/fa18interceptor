"""Generate the frame 7991 plane snapshot and frame 7992 line write oracle."""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[1]
BASES = (0x12BC0, 0x14B00, 0x16A40, 0x18980)
chip = (ROOT / "build/run060_chip_frames/frame7991.chip").read_bytes()
jobs = json.loads((ROOT / "build/run060_dma_blits7992_wide.json").read_text())["jobs"][4:]
traces = json.loads((ROOT / "build/run060_dma_writes7992.json").read_text())["jobs"][4:]

states = []
for job, trace in zip(jobs, traces):
    plane = max(i for i, base in enumerate(BASES) if job["d"] >= base)
    final = {}
    for item in trace["writes"]:
        final[int(item["address"], 16) - BASES[plane]] = int(item["data"], 16)
    states.append((plane, final))

max_writes = max(len(final) for _, final in states)
out = [
    "#ifndef FA18_RUN060_FRAME7991_LINE_ORACLE_H",
    "#define FA18_RUN060_FRAME7991_LINE_ORACLE_H",
    "",
    "#include <stdint.h>",
    "",
    "enum { FA18_RUN060_LINE_PLANE_BYTES = 8000, FA18_RUN060_LINE_MAX_WRITES = %d };" % max_writes,
    "typedef struct { uint16_t offset; uint16_t value; } FA18LineOracleWrite;",
    "",
    "static const uint8_t fa18_run060_frame7991_line_planes[4][FA18_RUN060_LINE_PLANE_BYTES] = {",
]
for base in BASES:
    plane = chip[base:base + 8000]
    out.append("    {")
    for i in range(0, len(plane), 16):
        out.append("        " + ", ".join("0x%02x" % b for b in plane[i:i + 16]) + ",")
    out.append("    },")
out += [
    "};", "",
    "static const uint8_t fa18_run060_line_oracle_planes[32] = {",
    "    " + ", ".join(str(p) for p, _ in states) + ",", "};", "",
    "static const uint16_t fa18_run060_line_oracle_write_counts[32] = {",
    "    " + ", ".join(str(len(final)) for _, final in states) + ",", "};", "",
    "static const FA18LineOracleWrite fa18_run060_line_oracle_writes[32][FA18_RUN060_LINE_MAX_WRITES] = {",
]
for _, final in states:
    out.append("    {")
    for offset, value in final.items():
        out.append("        {0x%04x, 0x%04x}," % (offset, value))
    out.append("    },")
out += ["};", "", "#endif", ""]
(ROOT / "port/run060_frame7991_line_oracle.h").write_text("\n".join(out))
print({"jobs": len(states), "max_writes": max_writes})
