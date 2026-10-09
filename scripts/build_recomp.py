#!/usr/bin/env python3
"""Build the headless translated runner with shared dependency-aware objects."""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MUSASHI = Path("tools/musashi")
DEFAULT_MAIN = Path("port/recomp/recomp_main.c")
DEFAULT_OUTPUT = Path("build/recomp/fa18_recomp.exe")
CFLAGS = [
    "-O2", "-w", f"-I{MUSASHI}", "-Iport/machine", "-Iport/recomp",
    "-Iport/recomp/generated", "-Iport/game", "-Iport/game/glue", "-Iport/os",
]


def source_files(main: Path) -> list[Path]:
    fixed = [
        MUSASHI / "m68kcpu.c", MUSASHI / "m68kops.c", MUSASHI / "m68kdasm.c",
        MUSASHI / "softfloat/softfloat.c", main,
        Path("port/recomp/recomp_runtime.c"), Path("port/recomp/recomp_ports.c"),
        Path("port/recomp/loop_input.c"), Path("port/machine/machine.c"),
        Path("port/native/flight_trace.c"),
        Path("port/machine/bus.c"), Path("port/machine/blitter.c"),
        Path("port/machine/display.c"), Path("port/machine/input.c"),
        Path("port/amiga/rom_audit.c"),
        Path("port/amiga/host_keys.c"),
        Path("port/amiga/runtime_guard.c"),
        Path("port/amiga/service_dispatch.c"),
        Path("port/amiga/exec_lists.c"),
        Path("port/amiga/exec_task_services.c"),
        Path("port/amiga/exec_memory.c"),
        Path("port/amiga/exec_context.c"),
        Path("port/amiga/exec_scheduler.c"),
        Path("port/amiga/exec_interrupt_services.c"),
        Path("port/amiga/exec_bootstrap.c"),
        Path("port/amiga/host_compat.c"),
        Path("port/amiga/host_graphics.c"),
        Path("port/amiga/rgb4.c"),
        Path("port/amiga/viewport_list.c"),
        Path("port/amiga/sha256.c"),
        Path("port/romfree/media.c"),
        Path("port/amiga/guest_memory.c"),
        Path("port/amiga/hunk_loader.c"),
    ]
    globbed: list[Path] = []
    for pattern in ("port/recomp/generated/*.c", "port/game/*.c",
                    "port/game/glue/*.c", "port/os/*.c"):
        globbed.extend(sorted(Path().glob(pattern)))
    return fixed + globbed


def check_duplicate_globals() -> None:
    names = re.findall(r"^#define\s+([A-Z0-9_]+)\s+",
                       (ROOT / "port/game/globals.h").read_text(), re.MULTILINE)
    duplicates = sorted(name for name, count in Counter(names).items() if count > 1)
    if duplicates:
        raise SystemExit("globals.h defines twice: " + ", ".join(duplicates))


def ninja_path(path: Path) -> str:
    value = path.as_posix()
    if any(character in value for character in "$ :"):
        raise SystemExit(f"build path needs Ninja escaping: {value}")
    return value


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--main", type=Path, default=DEFAULT_MAIN)
    parser.add_argument("--romfree",action="store_true",help="build the clean ADF launcher")
    parser.add_argument("--replace-source", action="append", default=[], metavar="OLD=NEW")
    parser.add_argument("--jobs", type=int, default=8)
    args = parser.parse_args()
    if args.romfree:
        if args.main!=DEFAULT_MAIN:parser.error("--romfree uses the shared runner entry")
        if args.output==DEFAULT_OUTPUT:args.output=Path("build/recomp/fa18_romfree.exe")
    if args.output.is_absolute() or args.main.is_absolute():
        parser.error("source and output paths must be relative to the repository")
    if args.jobs <= 0:
        parser.error("--jobs must be positive")

    replacements: dict[Path, Path] = {}
    for value in args.replace_source:
        if "=" not in value:
            parser.error("--replace-source expects OLD=NEW")
        old, new = value.split("=", 1)
        replacements[Path(old)] = Path(new)

    check_duplicate_globals()
    subprocess.run([sys.executable, "tools/recomp/static_recomp.py", "--check"],
                   cwd=ROOT, check=True)
    original_sources = source_files(args.main)
    if args.romfree: original_sources.append(Path("port/romfree/profile.c"))
    sources = [replacements.get(source, source) for source in original_sources]
    missing = [str(source) for source in sources if not (ROOT / source).is_file()]
    if missing:
        raise SystemExit("missing build source: " + ", ".join(missing))
    unused = set(replacements) - set(original_sources)
    if unused:
        raise SystemExit("replacement source not in build: " + ", ".join(map(str, sorted(unused))))

    obj_root = Path("build/recomp/obj")
    objects = [obj_root / source.with_suffix(source.suffix + ".o") for source in sources]
    if args.romfree:
        objects[sources.index(args.main)]=obj_root/"port/recomp/recomp_main_romfree.c.o"
    ninja_file = Path("build/recomp") / (args.output.name + ".ninja")
    (ROOT / ninja_file).parent.mkdir(parents=True, exist_ok=True)
    for obj in objects:
        (ROOT / obj).parent.mkdir(parents=True, exist_ok=True)
    (ROOT / args.output).parent.mkdir(parents=True, exist_ok=True)

    lines = [
        "ninja_required_version = 1.3",
        "builddir = build/recomp",
        "cflags = " + " ".join(CFLAGS),
        "rule cc",
        "  command = gcc $cflags -MMD -MF $out.d -c $in -o $out",
        "  depfile = $out.d",
        "  deps = gcc",
        "rule cc_romfree",
        "  command = gcc $cflags -DFA18_ROMFREE_MAIN -MMD -MF $out.d -c $in -o $out",
        "  depfile = $out.d",
        "  deps = gcc",
        "rule link",
        "  command = gcc $cflags -o $out @$out.rsp",
        "  rspfile = $out.rsp",
        "  rspfile_content = $in",
    ]
    lines.extend(f"build {ninja_path(obj)}: {'cc_romfree' if args.romfree and source==args.main else 'cc'} {ninja_path(source)}"
                 for source, obj in zip(sources, objects))
    lines.append(f"build {ninja_path(args.output)}: link " +
                 " ".join(ninja_path(obj) for obj in objects))
    lines.append(f"default {ninja_path(args.output)}")
    (ROOT / ninja_file).write_text("\n".join(lines) + "\n", newline="\n")
    subprocess.run(["ninja", "-f", str(ninja_file), "-j", str(args.jobs)],
                   cwd=ROOT, check=True)
    subprocess.run([sys.executable, "scripts/prune_build_artifacts.py", "--quiet",
                    "--keep", str(args.output)], cwd=ROOT, check=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
