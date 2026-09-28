"""Register recreated routines in port/game/glue/ports.c and ports_glue.h.

  python tools/recomp/register_ports.py "comment" C1E4A6=sort_by_depth:300 ...

Each argument is ADDRESS=c_function:cycles, where cycles is the instruction
time to charge in ON mode (blitter waits are charged by wait_blitter). The
glue function is glue_ADDRESS unless given as ADDRESS=c_function:cycles:glue.
"""
from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def main() -> None:
    comment, specs = sys.argv[1], sys.argv[2:]
    rows, decls = [], []
    header = (ROOT / "port/game/glue/ports_glue.h").read_text()
    table = (ROOT / "port/game/glue/ports.c").read_text()
    for spec in specs:
        address, rest = spec.split("=", 1)
        parts = rest.split(":")
        name, cycles = parts[0], int(parts[1])
        glue = parts[2] if len(parts) > 2 else f"glue_{address.upper()}"
        if f"{{0x{address.upper()}," in table:
            raise SystemExit(f"{address} is already registered")
        rows.append(f'    {{0x{address.upper()}, {glue}, "{name}", {cycles}}},\n')
        if f"int {glue}(void);" not in header and f"int {glue}(void);" not in "".join(decls):
            decls.append(f"int {glue}(void);\n")
    if decls:
        header = header.replace("#endif", f"/* {comment} */\n" + "".join(decls) + "\n#endif", 1)
        (ROOT / "port/game/glue/ports_glue.h").write_text(header)
    table = table.replace("    {0, 0, 0, 0}, /* sentinel",
                          f"    /* {comment} */\n" + "".join(rows) + "    {0, 0, 0, 0}, /* sentinel", 1)
    (ROOT / "port/game/glue/ports.c").write_text(table)
    print(f"registered {len(rows)}")


if __name__ == "__main__":
    main()
