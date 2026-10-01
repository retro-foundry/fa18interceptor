"""Compare resumable C bridge instructions with original opcodes.

Use --group audio for the voice iterator, its helpers and fading. Run from
any directory. Fixtures exercise all CCR combinations and word
boundaries; the sealed state and ROM are read only.
"""
import argparse
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash
from port_info import instructions


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=32,
                        help="fixtures per source instruction (at least 32 for every CCR)")
    parser.add_argument("--bash", default=default_bash())
    parser.add_argument("--bus", action="store_true",
                        help="include DMA bus contention in instruction timing fixtures")
    group_names = ("planes", "audio", "glyphs", "input", "page", "notify",
                   "command", "buffers", "polygon", "postflight", "followup", "faces", "regions", "map", "grid", "renderer", "sound_start", "screen_frame", "drawing", "cells", "startup", "number_field", "orientation", "tracking")
    parser.add_argument("--group", choices=group_names + ("all",), default="planes")
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("--cases must be positive")
    groups = {
        "planes": ["C2FD8C"],
        "audio": ["C4FFB0", "C4FFB4", "C50158", "C501E0", "C50212", "C24FE8"],
        "glyphs": ["C330FE", "C32806"],
        "input": ["C1715C", "C16F1C"],
        "page": ["C2F558"],
        "notify": ["C11B44"],
        "command": ["C17B08", "C17B2C", "C17EF2", "C3316A", "C3316E", "C33180", "C3318E", "C33186"],
        "buffers": ["C2FD22"],
        "polygon": ["C30466", "C304B2", "C305AA"],
        "postflight": ["C31226"],
        "followup": ["C0FA04"],
        "faces": ["C2005C"],
        "regions": ["C2B05A"],
        "map": ["C2AA9C", "C2AB34", "C2AB5A"],
        "grid": ["C279D0"],
        "renderer": ["C212B0", "C332BC", "C23CA6", "C2469E", "C246A0", "C247C0", "C248B2", "C24996",
                     "C091E0", "C091CE", "C091A8"],
        "sound_start": ["C17E4A", "C17CF6", "C17DAA", "C17C62", "C17D6E", "C18096", "C1803C", "C180FC",
                        "C50AB4", "C50B02"],
        "tracking": ["C123FA", "C25980", "C2564E"],
        "startup": ["C11312", "C11B0E", "C28722", "C287DA", "C28800", "C28AFE", "C28B34", "C28F16"],
        "number_field": ["C24E2C", "C24F76", "C25A08", "C0F56A"],
        "orientation": ["C2D954", "C2E47A", "C2E514", "C2E5F6", "C2E6DA"],
        "cells": ["C1D3F4", "C1D4E4", "C1D520", "C1D5D8"],
        "drawing": ["C2FF48", "C301F0", "C301F6", "C3040C", "C2FA78", "C2FA7E", "C2EE4A", "C2F0C6", "C2F0F4", "C2F128", "C2F156"],
        "screen_frame": ["C0D74A", "C0D752", "C0DAA0", "C0DAD0", "C0DAD4", "C0DADC", "C0DAE6",
                         "C2E758", "C2EA5A", "C2EAD0", "C2EB4C", "C2EBC2"],
    }
    entries = (entry for name in group_names for entry in groups[name]) \
        if args.group == "all" else iter(groups[args.group])
    addresses = {}
    for entry in entries:
        source = instructions(entry)
        if not source:
            raise SystemExit(f"missing original instructions for {entry}")
        for line in source:
            pc = line.split(":")[0]
            addresses.setdefault(pc, entry)
    header = "static const struct { uint32_t pc; int (*step)(void); } step_oracle_cases[] = {\n"
    header += "".join(f"    {{0x{pc}u, glue_{entry}_step}},\n" for pc, entry in sorted(addresses.items()))
    header += "};\n"
    (ROOT / "build/recomp").mkdir(parents=True, exist_ok=True)
    header_path = ROOT / "build/recomp/step_oracle_cases.h"
    if not header_path.exists() or header_path.read_text() != header:
        header_path.write_text(header)
    executable = build_oracle("active_planes_step_oracle",
                             "tools/recomp/active_planes_step_oracle.c", args.bash)
    subprocess.run([str(executable), str(args.cases), args.group,
                    "bus" if args.bus else "cpu"], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
