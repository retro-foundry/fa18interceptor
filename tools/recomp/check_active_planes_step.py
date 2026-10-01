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
    group_names = ("planes", "audio", "glyphs", "input", "page", "notify",
                   "command", "buffers", "polygon", "postflight")
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
        "polygon": ["C30466", "C304B2"],
        "postflight": ["C31226"],
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
    subprocess.run([str(executable), str(args.cases), args.group], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
