"""Generate the semantic C words read by frame 7992 line packet 12."""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[1]
records = json.loads((ROOT / "build/run060_dma_frame7992_wide.json").read_text())["selected_records"]
starts = [i for i, record in enumerate(records) if record["addr"] == "DFF058"]
packet = records[starts[16]:starts[17]]
words = [int(record["dat"], 16) for record in packet
         if record["type"] == 5 and record["extra"] == 34]
if len(words) != 99:
    raise ValueError(f"expected 99 line C words, got {len(words)}")
text = [
    "#ifndef FA18_RUN060_PACKET12_C_SOURCE_H",
    "#define FA18_RUN060_PACKET12_C_SOURCE_H",
    "#include <stdint.h>",
    "static const uint16_t fa18_run060_packet12_c_source[99] = {",
    "    " + ", ".join(f"0x{word:04x}" for word in words),
    "};",
    "#endif",
    "",
]
(ROOT / "port/run060_packet12_c_source.h").write_text("\n".join(text))
print({"words": len(words)})
