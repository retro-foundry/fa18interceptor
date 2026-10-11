"""Fail when the fa18_port executable contains recorded emulator output.

fa18_port may contain code, game assets taken from the disk (each one listed in
port/native_data_allowlist.txt with its provenance) and nothing else. It reads
the recorded control inputs at run time. Recorded frames, Chip RAM, blitter or
DMA captures, and logic keyed to particular frames or runs belong in contract
tests and oracle tools, never in the executable.

The check follows every source of the fa18_port target, every library the
target links from this CMakeLists.txt, and every header those files include.
It runs before each fa18_port build and as a ctest.
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

TARGET = "fa18_port"
# One table of more than this many values is recorded data or an asset.
MAX_ARRAY_VALUES = 256
# Hand-written logic files in port/ stay well below this.
MAX_FILE_LITERALS = 600
EXTERNAL_LIBRARIES = {"SDL2-static", "SDL2::SDL2", "SDL2::SDL2-static",
                      "SDL2main", "SDL2::SDL2main", "m"}
KEYWORDS = {"PRIVATE", "PUBLIC", "INTERFACE", "WIN32", "MACOSX_BUNDLE",
            "EXCLUDE_FROM_ALL", "STATIC", "SHARED", "OBJECT"}
# Build steps a native port does not need and that could generate data files.
CMAKE_BANNED = ("configure_file", "add_custom_command", "file(GENERATE",
                "file(WRITE", "file(APPEND", "include_directories",
                "target_include_directories")

RECORDED_NAME = re.compile(r"run\d{3}|frame_?\d{3}|^f\d{4}|deltas|streams|oracle|capture",
                           re.IGNORECASE)
NUMBER = re.compile(r"(?<![\w.])(?:0[xX][0-9A-Fa-f]+|\d+)[uUlL]*(?![\w.])")
FRAME_KEYED = re.compile(
    r"\b\w*frame\w*\s*(?:==|!=|<=|>=|<|>|-|\+)\s*\d{3,}"
    r"|(?<![\w.])\d{3,}[uUlL]*\s*(?:==|!=|<=|>=|<|>)\s*(?:\w+(?:->|\.))*\w*frame\w*\b",
    re.IGNORECASE)
RUN_IDENTIFIER = re.compile(r"\b\w*run\d{3}\w*\b", re.IGNORECASE)
RECORDED_PATH = re.compile(r"FA18RGB4|\.chip\b|captures[/\\]|(?:^|[/\\])build[/\\]"
                           r"|\.jsonl\b|oracle", re.IGNORECASE)
INCLUDE = re.compile(r'^\s*#\s*include\s*([<"])([^>"]+)[>"]', re.MULTILINE)
EMBED = re.compile(r"^\s*#\s*embed\b|\bincbin\b", re.MULTILINE | re.IGNORECASE)
PER_RULE_REPORT_LIMIT = 5


def mask_comments_and_strings(text: str) -> tuple[str, list[tuple[int, str]]]:
    """Blank out comments and string/char literals, keeping every offset."""
    out = list(text)
    strings: list[tuple[int, str]] = []
    i, n = 0, len(text)
    while i < n:
        if text.startswith("//", i):
            end = text.find("\n", i)
            end = n if end < 0 else end
        elif text.startswith("/*", i):
            end = text.find("*/", i + 2)
            end = n if end < 0 else end + 2
        elif text[i] in "\"'":
            quote, end = text[i], i + 1
            while end < n and text[end] != quote and text[end] != "\n":
                end += 2 if text[end] == "\\" else 1
            end = min(end + 1, n)
            if quote == '"':
                strings.append((i, text[i + 1:end - 1]))
        else:
            i += 1
            continue
        for k in range(i, end):
            if out[k] != "\n":
                out[k] = " "
        i = end
    return "".join(out), strings


def cmake_calls(text: str, command: str) -> list[list[str]]:
    """Return the argument lists of every `command(...)` call."""
    text = re.sub(r"#[^\n]*", "", text)
    pattern = re.compile(r"\b" + re.escape(command) + r"\s*\(([^)]*)\)", re.IGNORECASE)
    return [m.group(1).split() for m in pattern.finditer(text)]


class Check:
    def __init__(self, root: Path) -> None:
        self.root = root
        self.port = root / "port"
        self.problems: list[str] = []
        self.data_files: list[str] = []
        self.allowed: dict[Path, str] = {}

    def fail(self, where: str, message: str) -> None:
        self.problems.append(f"{where}: {message}")

    def rel(self, path: Path) -> str:
        return path.relative_to(self.root).as_posix()

    def load_allowlist(self) -> None:
        path = self.port / "native_data_allowlist.txt"
        if not path.exists():
            return
        for number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            name, _, provenance = (part.strip() for part in line.partition("|"))
            if not provenance:
                self.fail(f"{self.rel(path)}:{number}", f"{name} has no provenance")
                continue
            self.allowed[(self.port / name).resolve()] = provenance

    def target_sources(self, cmake: str) -> list[Path]:
        for command in CMAKE_BANNED:
            if re.search(r"\b" + re.escape(command).replace(r"\(", r"\s*\("), cmake,
                         re.IGNORECASE):
                self.fail("port/CMakeLists.txt",
                          f"uses {command.rstrip('(')}; ask the user before adding it")
        libraries = {args[0]: args[1:] for args in cmake_calls(cmake, "add_library") if args}
        sources: list[Path] = []
        pending, seen = [TARGET], set()
        while pending:
            name = pending.pop()
            if name in seen:
                continue
            seen.add(name)
            if name == TARGET:
                calls = [a[1:] for a in cmake_calls(cmake, "add_executable") if a and a[0] == name]
                if not calls:
                    self.fail("port/CMakeLists.txt", f"no add_executable({TARGET} ...)")
            else:
                calls = [libraries[name]]
            calls += [a[1:] for a in cmake_calls(cmake, "target_sources") if a and a[0] == name]
            for args in calls:
                for arg in args:
                    if arg in KEYWORDS:
                        continue
                    if "$" in arg or not arg.endswith((".c", ".h")):
                        self.fail("port/CMakeLists.txt",
                                  f"{name} source {arg!r} is not a literal .c/.h file")
                        continue
                    sources.append((self.port / arg).resolve())
            for args in cmake_calls(cmake, "target_link_libraries"):
                if not args or args[0] != name:
                    continue
                for lib in args[1:]:
                    if lib in KEYWORDS:
                        continue
                    if lib in libraries:
                        pending.append(lib)
                    elif lib not in EXTERNAL_LIBRARIES:
                        self.fail("port/CMakeLists.txt",
                                  f"{name} links unknown library {lib!r}")
        return sources

    def closure(self, sources: list[Path]) -> list[Path]:
        ordered: list[Path] = []
        pending = list(sources)
        while pending:
            path = pending.pop(0)
            if path in ordered:
                continue
            if not path.is_file():
                self.fail(self.rel(path) if path.is_relative_to(self.root) else str(path),
                          "listed source does not exist")
                continue
            ordered.append(path)
            text = path.read_text(encoding="utf-8", errors="replace")
            for match in INCLUDE.finditer(text):
                kind, name = match.groups()
                line = text.count("\n", 0, match.start()) + 1
                found = next((c.resolve() for c in (path.parent / name, self.port / name)
                              if c.is_file()), None)
                if found and self.port in found.parents:
                    pending.append(found)
                elif found:
                    self.fail(f"{self.rel(path)}:{line}",
                              f'includes "{name}" from outside port/')
                elif kind == '"':
                    self.fail(f"{self.rel(path)}:{line}",
                              f'includes "{name}", which is not a file in port/')
        return ordered

    def scan(self, path: Path) -> None:
        text = path.read_text(encoding="utf-8", errors="replace")
        where = self.rel(path)
        allowed = path in self.allowed
        if not allowed and RECORDED_NAME.search(path.name):
            # One line per recorded-data file; its guards and tables add nothing.
            self.data_files.append(f"{where}: recorded data ({path.stat().st_size:,} bytes, "
                                   f"{len(NUMBER.findall(text)):,} values)")
            return
        code, strings = mask_comments_and_strings(text)

        def line_of(offset: int) -> int:
            return text.count("\n", 0, offset) + 1

        def report(rule: str, hits: list[tuple[int, str]]) -> None:
            for offset, message in hits[:PER_RULE_REPORT_LIMIT]:
                self.fail(f"{where}:{line_of(offset)}", message)
            if len(hits) > PER_RULE_REPORT_LIMIT:
                self.fail(where, f"... and {len(hits) - PER_RULE_REPORT_LIMIT} more {rule}")

        literals = len(NUMBER.findall(code))
        big_tables = []
        for match in re.finditer(r"=\s*\{", code):
            depth, end = 0, match.end() - 1
            while end < len(code):
                depth += {"{": 1, "}": -1}.get(code[end], 0)
                if depth == 0:
                    break
                end += 1
            values = len(NUMBER.findall(code, match.end(), end))
            if values > MAX_ARRAY_VALUES:
                big_tables.append((match.start(), values))
        report("embedded files", [(m.start(), "embeds a file into the build")
                                  for m in EMBED.finditer(text)])
        report("frame-keyed expressions",
               [(m.start(), f"logic keyed to a frame number: {' '.join(m.group(0).split())}")
                for m in FRAME_KEYED.finditer(code)])
        report("recorded-output strings",
               [(offset, f"string refers to recorded output: {value[:60]!r}")
                for offset, value in strings if RECORDED_PATH.search(value)])
        if allowed:
            return
        report("run-keyed identifiers",
               [(m.start(), f"identifier names a recording: {m.group(0)}")
                for m in RUN_IDENTIFIER.finditer(code)])
        if literals > MAX_FILE_LITERALS:
            self.fail(where, f"{literals} numeric literals (limit {MAX_FILE_LITERALS}); "
                             "tables belong in an allowlisted disk asset")
        report("large initializers",
               [(offset, f"initializer with {values} values (limit {MAX_ARRAY_VALUES})")
                for offset, values in big_tables])

    def run(self) -> int:
        cmake_path = self.port / "CMakeLists.txt"
        self.load_allowlist()
        sources = self.target_sources(cmake_path.read_text(encoding="utf-8"))
        files = self.closure(sources)
        for path in files:
            self.scan(path)
        used_assets = [self.rel(p) for p in files if p in self.allowed]
        if self.problems or self.data_files:
            print(f"{TARGET} native build check FAILED: {len(self.data_files)} recorded-data "
                  f"file(s), {len(self.problems)} other problem(s), {len(files)} file(s) checked")
            if self.data_files:
                print(f"\nRecorded data compiled into {TARGET}:")
                for line in self.data_files:
                    print(f"  {line}")
            if self.problems:
                print(f"\nProblems in {TARGET} code:")
                for problem in self.problems:
                    print(f"  {problem}")
            print("\nRecorded emulator output (frames, Chip RAM, blitter/DMA captures) and\n"
                  "logic keyed to particular frames or runs may appear in *_contract_test.c\n"
                  "files and oracle tools only. Port the routine that produces the output.\n"
                  "Do not edit this check, its CMake wiring or port/native_data_allowlist.txt;\n"
                  "ask the user to add a disk asset, naming the ADF hunk and offset it comes from.")
            return 1
        print(f"{TARGET} native build check passed: {len(files)} file(s)"
              + (f", allowlisted assets: {', '.join(used_assets)}" if used_assets else ""))
        return 0


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1],
                        help="repository root (default: this script's repository)")
    args = parser.parse_args()
    sys.exit(Check(args.root.resolve()).run())


if __name__ == "__main__":
    main()
