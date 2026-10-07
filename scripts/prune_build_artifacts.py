#!/usr/bin/env python3
"""Keep disposable build artifacts within a bounded on-disk cache.

Active compiler outputs, fetched dependencies and compressed reference RAM are
protected. Raw captures and obsolete GNU oracle executables are removed
oldest-first. This tool only operates inside the repository's build roots.
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
DISPOSABLE_SUFFIXES = {".dat", ".rgb", ".idx", ".index8", ".rgb444",
                       ".ram", ".bin", ".jsonl", ".wav", ".ppm", ".png"}
BUILD_ROOTS = (ROOT / "build", ROOT / "port/build")


def is_disposable(path: Path, relative: Path, minimum: int) -> bool:
    if path.stat().st_size < minimum:
        return False
    if any(part in PROTECTED_DIRS for part in relative.parts[:-1]):
        return False
    if any(part.startswith("ram-") for part in relative.parts[:-1]):
        return False  # A comparison may be consuming this live temporary run.
    # Hundreds of separately linked historical GNU probes can consume GiB.
    # CMake executables and the playable reference runners remain protected.
    if relative.parent == Path("recomp") and path.suffix.lower() == ".exe":
        return path.stem not in {"fa18_recomp", "fa18_romfree"}
    if path.suffix.lower() in PROTECTED_SUFFIXES:
        return False
    return path.suffix.lower() in DISPOSABLE_SUFFIXES


def files_without_links(build: Path):
    """Do not follow Windows junctions or symlinks outside the build tree."""
    for directory, dirs, names in os.walk(build, followlinks=False):
        dirs[:] = [name for name in dirs if not is_link(Path(directory) / name)]
        for name in names:
            path = Path(directory) / name
            if not is_link(path):
                yield path


def is_link(path: Path) -> bool:
    import stat
    try:
        info = path.lstat()
    except OSError:
        return True  # A concurrent temporary run may already have removed it.
    return path.is_symlink() or bool(getattr(info, "st_file_attributes", 0) &
                                   getattr(stat, "FILE_ATTRIBUTE_REPARSE_POINT", 0))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    parser.add_argument("--max-gib", type=float,
                        default=float(os.environ.get("FA18_BUILD_MAX_GIB", "4")))
    parser.add_argument("--min-file-mib", type=float, default=0.5)
    parser.add_argument("--keep", type=Path, action="append", default=[],
                        help="protect a currently used file or directory")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--quiet", action="store_true")
    args = parser.parse_args()

    build = args.build_dir.resolve()
    if build not in [path.absolute() for path in BUILD_ROOTS]:
        parser.error("--build-dir must be this repository's build or port/build directory")
    if not build.is_dir():
        return 0
    if is_link(build):
        parser.error("build directory must not be a junction or symlink")
    if not 0 < args.max_gib < float("inf") or not 0 <= args.min_file_mib < float("inf"):
        parser.error("budgets must be positive")
    limit = int(args.max_gib * (1 << 30))
    minimum = int(args.min_file_mib * (1 << 20))
    keeps = [path.resolve() for path in args.keep]

    total = 0
    candidates: list[tuple[int, int, str, Path, int]] = []
    for path in files_without_links(build):
        try:
            stat = path.stat()
        except OSError:
            continue
        total += stat.st_size
        relative = path.relative_to(build)
        if any(path == keep or keep in path.parents for keep in keeps):
            continue
        try:
            disposable = is_disposable(path, relative, minimum)
        except OSError:
            continue
        if disposable:
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
            except OSError as error:
                print(f"cannot remove {relative}: {error}")
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
              "protected, busy or small files remain")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
