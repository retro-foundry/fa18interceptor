#!/usr/bin/env python3
"""Feed a port's run log back into its manifest.

The recompiler discovers code by following control flow it can see. Computed
jumps, jump tables and interrupt vectors can leave a first translation with
gaps. Running the port reports addresses the dispatcher
reached with no compiled block; adding those to the manifest and regenerating
can close those gaps when matching loaded bytes exist. Qualify the actual
scenario and fallback counts after discovery converges.

This is the convergence loop from AMIGA_RECOMP.md 26.2, and it is the normal
way a port is brought up.

    arecomp recompile recomp/game.toml -o build/recomp
    ... build and run, capturing stderr to run.log ...
    python tools/converge.py recomp/game.toml run.log --snapshot mem.bin --dry-run
    arecomp recompile recomp/game.toml -o build/recomp    # after reviewed metadata

Addresses in memory that is entirely zero in the snapshot are reported but not
added. Empty or out-of-bank windows require actual loaded-byte evidence;
nonzero windows are only candidates. --output retains the input manifest,
and --snapshot-base maps guest addresses to the supplied dump correctly.
"""

import argparse
import re
import sys
from pathlib import Path

TARGET = re.compile(r"no compiled block at ([0-9a-fA-F]+)")
ENTRY_BLOCK = re.compile(r"^[ \t]*entry_points\s*=\s*\[(.*?)\]", re.S | re.M)
CODE_SECTION = re.compile(r"(?m)^[ \t]*\[code\][ \t]*(?:#.*)?$")
SECTION = re.compile(r"(?m)^[ \t]*\[")


def read_targets(path):
    targets = set()
    with open(path, "r", encoding="utf-8", errors="replace") as handle:
        for line in handle:
            match = TARGET.search(line)
            if match:
                targets.add(int(match.group(1), 16))
    return targets


def existing_entry_points(text):
    section = CODE_SECTION.search(text)
    if not section:
        return set(), None
    next_section = SECTION.search(text, section.end())
    match = ENTRY_BLOCK.search(text, section.end(), next_section.start() if next_section else len(text))
    if not match:
        return set(), match
    values = re.sub(r"#[^\n]*", "", match.group(1))
    return {int(a.strip()[1:], 16) if a.strip().startswith('$') else int(a.strip(), 0)
            for a in values.split(',') if a.strip()}, match


def looks_like_code(snapshot, address, window=16, base=0):
    """False when the snapshot holds nothing at this address.

    A game that loads code from disk leaves those regions empty at capture
    time. Translating them would compile zeros.
    """
    if snapshot is None:
        return True
    offset = address - base
    if offset < 0 or offset + window > len(snapshot):
        return False
    return any(snapshot[offset:offset + window])


def format_entry_points(addresses, per_line=6):
    lines = []
    ordered = sorted(addresses)
    for i in range(0, len(ordered), per_line):
        chunk = ", ".join(f"0x{a:08x}" for a in ordered[i:i + per_line])
        lines.append("  " + chunk + ",")
    return "entry_points = [\n" + "\n".join(lines).rstrip(",") + "\n]"


def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("manifest")
    parser.add_argument("log", help="stderr from a run of the port")
    parser.add_argument("--snapshot",
                        help="the snapshot the port was built from; used to "
                             "reject addresses that hold nothing")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--snapshot-base", type=lambda value: int(value, 0), default=0,
                        help="guest base of the snapshot bank (default zero)")
    parser.add_argument("--output", type=Path,
                        help="write candidates to a derived manifest instead of editing input")
    args = parser.parse_args()

    snapshot = None
    if args.snapshot:
        with open(args.snapshot, "rb") as handle:
            snapshot = handle.read()

    targets = read_targets(args.log)
    if not targets:
        print("no unresolved targets in the supplied log; qualify scenario scope and fallback statistics separately")
        return 0

    text = open(args.manifest, encoding="utf-8").read()
    known, match = existing_entry_points(text)

    fresh = {a for a in targets if a not in known}
    usable = {a for a in fresh if looks_like_code(snapshot, a, base=args.snapshot_base)}
    empty = fresh - usable

    print(f"log reports        : {len(targets)} distinct unresolved targets")
    print(f"already recorded   : {len(targets & known)}")
    print(f"new and translatable: {len(usable)}")
    if empty:
        low, high = min(empty), max(empty)
        print(f"new empty/outside bank: {len(empty)}  (${low:06x}..${high:06x})")
        print("  Capture the actual loaded bytes before translating these targets.")

    if not usable:
        print("\nnothing to add")
        return 0

    merged = known | usable
    block = format_entry_points(merged)

    if match:
        text = text[:match.start()] + block + text[match.end():]
    elif CODE_SECTION.search(text):
        section = CODE_SECTION.search(text)
        text = text[:section.end()] + '\n' + block + text[section.end():]
    else:
        text = text.rstrip() + (
            "\n\n# Entry points observed in a trace run: addresses execution\n"
            "# reached that static discovery could not predict. Metadata, not\n"
            "# inference (AMIGA_RECOMP.md 26.2, 26.3).\n[code]\n" + block + "\n")

    if args.dry_run:
        print("\n(dry run: manifest not written)")
        return 0

    destination = args.output or Path(args.manifest)
    if args.output and destination.resolve() == Path(args.manifest).resolve():
        parser.error('--output must name a different derived manifest.')
    if args.output and destination.resolve().parent != Path(args.manifest).resolve().parent:
        parser.error('Derived manifest must stay beside its input so relative asset paths remain valid.')
    destination.write_text(text, encoding='utf8')
    print(f"\n{destination}: now records {len(merged)} entry points")
    print("Regenerate the port and run it again; repeat until this reports none.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
