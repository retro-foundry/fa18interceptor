"""Generate A/B area sources from captured DMA words and odd row strides."""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[1]
records = json.loads((ROOT / "build/run060_dma_frame7992_wide.json").read_text())["selected_records"]
starts = [i for i, record in enumerate(records) if record["addr"] == "DFF058"]
jobs = []
for index in range(4):
    packet = records[starts[index]:starts[index + 1]]
    a = [int(r["dat"], 16) for r in packet if r["type"] == 5 and r["extra"] == 0]
    b = [int(r["dat"], 16) for r in packet if r["type"] == 5 and r["extra"] == 1]
    if len(a) != 216 or len(b) != 216:
        raise ValueError((index, len(a), len(b)))
    jobs.append((a, b))

def rows(words):
    result = []
    for row in range(12):
        data = []
        for word in words[row * 18:(row + 1) * 18]:
            data += [word >> 8, word & 0xff]
        data.append(0)
        result.append(data)
    return result

def emit(rows_value):
    return ",\n".join("    {" + ", ".join(f"0x{x:02x}" for x in row) + "}" for row in rows_value)

a_source = rows(jobs[0][0])
b_sources = [rows(b) for _, b in jobs]
out = [
    "#ifndef FA18_RUN060_FRAME7992_AREA_ASSETS_H", "#define FA18_RUN060_FRAME7992_AREA_ASSETS_H", "",
    "#include <stdint.h>", "enum { FA18_RUN060_AREA_ROWS = 12, FA18_RUN060_AREA_ROW_BYTES = 37 };", "",
    "static const uint8_t fa18_run060_frame7992_a_source[12][37] = {", emit(a_source), "};",
    "static const uint8_t fa18_run060_frame7992_b_source[4][12][37] = {",
]
out.append(",\n".join("    {\n" + emit(asset) + "\n    }" for asset in b_sources))
out += ["};", "", "#endif", ""]
(ROOT / "port/run060_frame7992_area_assets.h").write_text("\n".join(out))
print({"jobs": len(jobs), "words_per_job": 216})
