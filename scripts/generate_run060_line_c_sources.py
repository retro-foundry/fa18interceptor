"""Generate semantic C streams for all frame 7992 line submissions."""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[1]
records = json.loads((ROOT / "build/run060_dma_frame7992_wide.json").read_text())["selected_records"]
starts = [i for i, record in enumerate(records) if record["addr"] == "DFF058"]
streams = []
for index in range(4, 36):
    packet = records[starts[index]:starts[index + 1] if index + 1 < len(starts) else len(records)]
    words = [int(record["dat"], 16) for record in packet
             if record["type"] == 5 and record["extra"] == 34]
    streams.append(words)
if not all(streams) or max(map(len, streams)) > 133:
    raise ValueError("unexpected line C stream lengths")
out = [
    "#ifndef FA18_RUN060_LINE_C_SOURCES_H",
    "#define FA18_RUN060_LINE_C_SOURCES_H",
    "#include <stdint.h>",
    "enum { FA18_RUN060_LINE_C_SOURCE_MAX = 133 };",
    "static const uint16_t fa18_run060_line_c_source_counts[32] = {",
    "    " + ", ".join(str(len(stream)) for stream in streams),
    "};",
    "static const uint16_t fa18_run060_line_c_sources[32][FA18_RUN060_LINE_C_SOURCE_MAX] = {",
]
for stream in streams:
    out.append("    {")
    out.append("        " + ", ".join(f"0x{word:04x}" for word in stream) + ",")
    out.append("    },")
out += ["};", "#endif", ""]
(ROOT / "port/run060_line_c_sources.h").write_text("\n".join(out))
print({"streams": len(streams), "lengths": [len(stream) for stream in streams]})
