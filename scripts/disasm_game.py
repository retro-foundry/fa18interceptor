"""Disassemble the F/A-18 executable at its verified runtime addresses.

Reads local/extracted/f18_interceptor and analysis/hunk_runtime_resolved.json,
applies every 32-bit relocation with the resolved segment bases, and writes one
linear-sweep listing per CODE segment to build/disasm/seg<NN>.s plus an
address index. Segments without a resolved base get a synthetic base above
$C80000 so their listings still read consistently.

    python scripts/disasm_game.py            # write all listings
    python scripts/disasm_game.py C32D24 40  # print 40 instructions from $C32D24
"""
from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

import capstone

ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / "local/extracted/f18_interceptor"
OUT = ROOT / "build/disasm"


def parse_hunks(data: bytes) -> list[dict]:
    off = 0

    def u32() -> int:
        nonlocal off
        value = struct.unpack_from(">I", data, off)[0]
        off += 4
        return value

    assert u32() == 0x3F3
    while u32():
        pass
    count, first, last = u32(), u32(), u32()
    sizes = [u32() & 0x3FFFFFFF for _ in range(last - first + 1)]
    segments: list[dict] = []
    current = None
    while off < len(data):
        kind = u32() & 0x3FFFFFFF
        if kind in (0x3E9, 0x3EA):
            longs = u32()
            current = {"kind": "CODE" if kind == 0x3E9 else "DATA",
                       "data": bytearray(data[off:off + longs * 4]), "relocs": []}
            current["data"] += bytes(sizes[len(segments)] * 4 - longs * 4)
            off += longs * 4
            segments.append(current)
        elif kind == 0x3EB:
            u32()
            current = {"kind": "BSS", "data": bytearray(sizes[len(segments)] * 4), "relocs": []}
            segments.append(current)
        elif kind == 0x3EC:
            while True:
                n = u32()
                if not n:
                    break
                target = u32()
                for _ in range(n):
                    current["relocs"].append((u32(), target))
        elif kind == 0x3F0:
            while True:
                n = u32()
                if not n:
                    break
                off += n * 4 + 4
        elif kind == 0x3F1:
            off += u32() * 4
        elif kind == 0x3F2:
            continue
        else:
            raise ValueError(f"unknown hunk {kind:#x} at {off - 4:#x}")
    return segments


def load() -> tuple[list[dict], dict[int, int]]:
    segments = parse_hunks(EXE.read_bytes())
    resolved = json.loads((ROOT / "analysis/hunk_runtime_resolved.json").read_text())["resolved"]
    bases = {int(k): v["runtime_payload_base"] for k, v in resolved.items()}
    synthetic = 0xC80000
    for index, segment in enumerate(segments):
        if index not in bases:
            bases[index] = synthetic
            segment["synthetic"] = True
            synthetic += (len(segment["data"]) + 15) & ~15
    for segment in segments:
        for offset, target in segment["relocs"]:
            value = struct.unpack_from(">I", segment["data"], offset)[0]
            struct.pack_into(">I", segment["data"], offset, (value + bases[target]) & 0xFFFFFFFF)
    return segments, bases


def disassemble(code: bytes, base: int) -> list[tuple[int, bytes, str]]:
    md = capstone.Cs(capstone.CS_ARCH_M68K, capstone.CS_MODE_M68K_000)
    out = []
    pos = 0
    while pos < len(code):
        insn = next(md.disasm(code[pos:pos + 10], base + pos, 1), None)
        if insn is None:
            out.append((base + pos, code[pos:pos + 2], f"dc.w ${code[pos] << 8 | code[pos + 1]:04X}"))
            pos += 2
            continue
        out.append((insn.address, bytes(insn.bytes), f"{insn.mnemonic} {insn.op_str}".strip()))
        pos += insn.size
    return out


def main() -> None:
    segments, bases = load()
    if len(sys.argv) >= 2:
        address = int(sys.argv[1].lstrip("$"), 16)
        count = int(sys.argv[2]) if len(sys.argv) > 2 else 40
        for index, segment in enumerate(segments):
            base = bases[index]
            if base <= address < base + len(segment["data"]):
                listing = disassemble(bytes(segment["data"][address - base:]), address)
                for addr, raw, text in listing[:count]:
                    print(f"{addr:06X}  {raw.hex():<20} {text}")
                return
        raise SystemExit(f"${address:06X} is not in any segment")
    OUT.mkdir(parents=True, exist_ok=True)
    index_lines = []
    for index, segment in enumerate(segments):
        base = bases[index]
        tag = " synthetic" if segment.get("synthetic") else ""
        index_lines.append(f"seg{index:03d} {segment['kind']:4s} ${base:06X}-${base + len(segment['data']):06X}{tag}")
        if segment["kind"] != "CODE":
            continue
        with open(OUT / f"seg{index:03d}.s", "w") as handle:
            handle.write(f"; segment {index} {segment['kind']} base ${base:06X}{tag}\n")
            for addr, raw, text in disassemble(bytes(segment["data"]), base):
                handle.write(f"{addr:06X}  {raw.hex():<20} {text}\n")
    (OUT / "index.txt").write_text("\n".join(index_lines) + "\n")
    print(f"wrote {OUT}")


if __name__ == "__main__":
    main()
