#!/usr/bin/env python3
"""Keep disposable build artifacts within a bounded on-disk cache.

Compiler outputs and fetched dependencies are protected. Large replay streams,
traces, captures, and diagnostics are removed oldest-first when the build tree
exceeds the configured budget. Canonical shadow streams are removed last.
"""

from __future__ import annotations

import argparse
import os
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PROTECTED_DIRS = {"obj", "CMakeFiles", "_deps", "Testing"}
PROTECTED_SUFFIXES = {
    ".a", ".dll", ".exe", ".exp", ".ilk", ".lib", ".ninja_deps",
    ".ninja_log", ".o", ".obj", ".pdb",
}


def is_disposable(path: Path, relative: Path, minimum: int) -> bool:
    if path.stat().st_size < minimum:
        return False
    if any(part in PROTECTED_DIRS for part in relative.parts[:-1]):
        return False
    if path.suffix.lower() in PROTECTED_SUFFIXES:
        return False
    return True


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    parser.add_argument("--max-gib", type=float,
                        default=float(os.environ.get("FA18_BUILD_MAX_GIB", "12")))
    parser.add_argument("--min-file-mib", type=float, default=8)
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--quiet", action="store_true")
    args = parser.parse_args()

    build = args.build_dir.resolve()
    if not build.is_dir():
        return 0
    limit = int(args.max_gib * (1 << 30))
    minimum = int(args.min_file_mib * (1 << 20))
    if limit <= 0 or minimum < 0:
        parser.error("budgets must be positive")

    total = 0
    candidates: list[tuple[int, int, str, Path, int]] = []
    for path in build.rglob("*"):
        if not path.is_file():
            continue
        try:
            stat = path.stat()
        except OSError:
            continue
        total += stat.st_size
        relative = path.relative_to(build)
        if is_disposable(path, relative, minimum):
            canonical = (relative.parent == Path("recomp") and
                         relative.name.startswith("frames_shadow_") and
                         relative.suffix == ".bin")
            candidates.append((1 if canonical else 0, stat.st_mtime_ns,
                               relative.as_posix(), path, stat.st_size))

    before = total
    removed = 0
    removed_bytes = 0
    for _canonical, _mtime, relative, path, size in sorted(candidates):
        if total <= limit:
            break
        if not args.quiet:
            verb = "would remove" if args.dry_run else "removed"
            print(f"{verb} {relative} ({size / (1 << 30):.2f} GiB)")
        if not args.dry_run:
            try:
                path.unlink()
            except FileNotFoundError:
                continue
        total -= size
        removed += 1
        removed_bytes += size

    if not args.quiet:
        action = "would reclaim" if args.dry_run else "reclaimed"
        print(f"build cache: {before / (1 << 30):.2f} GiB -> "
              f"{total / (1 << 30):.2f} GiB; {action} "
              f"{removed_bytes / (1 << 30):.2f} GiB in {removed} files")
    if total > limit:
        print(f"build cache remains above {args.max_gib:g} GiB because only "
              "compiler outputs or small files remain")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
