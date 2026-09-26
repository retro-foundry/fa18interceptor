"""Generate semantic line-job descriptors from the widened run060 DMA inventory."""
from pathlib import Path
import argparse, json

BASES = (0x12BC0, 0x14B00, 0x16A40, 0x18980)

def main():
    p = argparse.ArgumentParser()
    p.add_argument("inventory", type=Path)
    p.add_argument("output", type=Path)
    a = p.parse_args()
    jobs = json.loads(a.inventory.read_text())["jobs"][4:]
    rows = []
    for job in jobs:
        destination = job["d"]
        matches = [(plane, destination - base) for plane, base in enumerate(BASES)
                   if base <= destination]
        matches = matches[-1:]
        if len(matches) != 1:
            raise ValueError(f"destination ${destination:06X} does not map to one page")
        plane, offset = matches[0]
        if offset > 0xffff:
            raise ValueError("semantic offset does not fit")
        rows.append((job, plane, offset))
    out = ["#ifndef FA18_RUN060_FRAME7992_LINE_PACKETS_H",
           "#define FA18_RUN060_FRAME7992_LINE_PACKETS_H", "",
           '#include "line.h"', "",
           "enum { FA18_RUN060_FRAME7992_LINE_PACKET_COUNT = %d };" % len(rows),
           "static const FA18LineBlitJob fa18_run060_frame7992_line_packets[%d] = {" % len(rows)]
    for j, plane, offset in rows:
        width, height = j["size"] & 0x3f, j["size"] >> 6
        values = [hex(j[k]) for k in
                  ("bltcon0", "bltcon1", "first_mask", "last_mask", "a", "adat", "bdat")]
        values += ["0"]
        values += [hex(j[k]) for k in ("amod", "bmod", "cmod", "dmod")]
        values += [str(width), str(height), str(plane), "0x%04X" % offset]
        out.append("    {" + ", ".join(values) + "},")
    out += ["};", "", "#endif", ""]
    a.output.write_text("\n".join(out))

if __name__ == "__main__":
    main()
