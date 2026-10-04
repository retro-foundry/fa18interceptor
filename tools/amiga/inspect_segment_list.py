"""Audit a DOS LoadSeg chain against an executable's Hunk inventory.

Oracle tool only: RAM supplies placement evidence, never runtime initialization.
The output contains layout and digests, not captured memory or ROM bytes.
No SDK, game addresses, emulator executable, or host pointer layout is required.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path


def uae_chunks(state: bytes) -> dict[bytes, bytes]:
    chunks = {}
    offset = 0
    while offset + 12 <= len(state):
        if state[offset:offset + 4] == bytes(4):
            offset += 4
            continue
        tag = state[offset:offset + 4]
        if tag == b"END ":
            break
        size, flags = struct.unpack_from(">II", state, offset + 4)
        if size < 12 or size > len(state) - offset:
            raise ValueError(f"invalid UAE chunk {tag!r} at {offset:#x}")
        if flags & 1 and tag in (b"ASF ", b"CRAM", b"BRAM"):
            raise ValueError(f"compressed UAE chunk {tag!r} is unsupported")
        if tag in chunks and tag in (b"ASF ", b"CRAM", b"BRAM"):
            raise ValueError(f"duplicate UAE chunk {tag!r}")
        chunks[tag] = state[offset + 12:offset + size]
        offset += (size + 3) & ~3
    if b"ASF " not in chunks:
        raise ValueError("not a UAE savestate")
    return chunks


class GuestMemory:
    def __init__(self, banks: list[tuple[int, bytes]]):
        self.banks = sorted(banks)
        for i, (base, data) in enumerate(self.banks):
            if base < 0 or base + len(data) > 0x1000000:
                raise ValueError("bank outside 24-bit guest memory")
            if i and self.banks[i - 1][0] + len(self.banks[i - 1][1]) > base:
                raise ValueError("overlapping guest banks")

    def read(self, address: int, size: int) -> bytes:
        for base, data in self.banks:
            if base <= address and 0 <= size <= len(data) - (address - base):
                return data[address - base:address - base + size]
        raise ValueError(f"guest range {address:#08x}+{size:#x} is not in captured RAM")

    def word32(self, address: int) -> int:
        return struct.unpack(">I", self.read(address, 4))[0]


def inspect(memory: GuestMemory, executable: bytes, inventory: dict,
            segment_list: int) -> dict:
    segments = inventory["segments"]
    if len(segments) != len(inventory["header_sizes_longs"]):
        raise ValueError("Hunk count differs from allocation table")
    rows, visited, occupied = [], set(), []
    address = segment_list * 4
    for index, segment in enumerate(segments):
        if not address:
            raise ValueError(f"segment chain ends before hunk {index}")
        if address in visited:
            raise ValueError(f"segment chain cycle at {address:#08x}")
        visited.add(address)
        base = address + 4
        allocated = memory.word32(address - 4)
        allocation_word = inventory["header_sizes_longs"][index]
        size = (allocation_word & 0x3FFFFFFF) * 4
        if allocated < size + 8:
            raise ValueError(f"hunk {index}: allocation {allocated} is smaller than {size + 8}")
        lo, hi = address - 4, address - 4 + allocated
        memory.read(lo, allocated)
        if any(lo < end and start < hi for start, end in occupied):
            raise ValueError(f"hunk {index}: allocation overlaps another segment")
        occupied.append((lo, hi))
        payload_size = segment["size_bytes"]
        if payload_size > size:
            raise ValueError(f"hunk {index}: payload exceeds header allocation")
        start = segment["payload_file_offset"]
        if segment["kind"] == "BSS":
            raw = bytes(payload_size)
        else:
            raw = executable[start:start + payload_size]
            if len(raw) != payload_size:
                raise ValueError(f"hunk {index}: truncated executable payload")
        relocated = set()
        for group in segment["reloc32"]:
            if not 0 <= group["target_segment"] < len(segments):
                raise ValueError(f"hunk {index}: relocation target outside image")
            for offset in group["offsets"]:
                if offset & 1 or not 0 <= offset <= payload_size - 4:
                    raise ValueError(f"hunk {index}: invalid relocation offset {offset:#x}")
                if any(byte in relocated for byte in range(offset, offset + 4)):
                    raise ValueError(f"hunk {index}: overlapping relocation fields")
                relocated.update(range(offset, offset + 4))
        live = memory.read(base, payload_size)
        next_pointer = memory.word32(address)
        rows.append({
            "index": index, "kind": segment["kind"], "payload_base": base,
            "payload_size": payload_size, "allocation_size": allocated,
            "header_size": size, "memory_flags": allocation_word >> 30,
            "next_segment_bptr": next_pointer,
            "disk_payload_sha256": hashlib.sha256(raw).hexdigest(),
            "nonrelocated_changed_bytes": sum(a != b for offset, (a, b) in
                enumerate(zip(raw, live)) if offset not in relocated),
        })
        address = next_pointer * 4
    if address:
        raise ValueError("segment chain continues beyond the executable's last hunk")
    relocations = 0
    for index, segment in enumerate(segments):
        for group in segment["reloc32"]:
            target = rows[group["target_segment"]]["payload_base"]
            for offset in group["offsets"]:
                raw = struct.unpack_from(">I", executable, segment["payload_file_offset"] + offset)[0]
                actual = memory.word32(rows[index]["payload_base"] + offset)
                if actual != (raw + target) & 0xFFFFFFFF:
                    raise ValueError(f"hunk {index}: relocation at {offset:#x} differs from disk + target")
                relocations += 1
    return {"schema": "amiga.segment_layout.v1", "segment_list_bptr": segment_list,
            "segments": rows, "verified_relocations": relocations,
            "captured_ram_is_runtime_input": False}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--state", required=True, type=Path)
    parser.add_argument("--executable", required=True, type=Path)
    parser.add_argument("--inventory", required=True, type=Path)
    parser.add_argument("--segment-list", required=True, type=lambda x: int(x, 0))
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    state, executable = args.state.read_bytes(), args.executable.read_bytes()
    chunks = uae_chunks(state)
    memory = GuestMemory([(0, chunks[b"CRAM"]), (0xC00000, chunks[b"BRAM"])])
    result = inspect(memory, executable, json.loads(args.inventory.read_text()), args.segment_list)
    result["evidence"] = {"state_sha256": hashlib.sha256(state).hexdigest(),
                          "executable_sha256": hashlib.sha256(executable).hexdigest(),
                          "inventory_sha256": hashlib.sha256(args.inventory.read_bytes()).hexdigest()}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    print(f"{len(result['segments'])} linked hunks; {result['verified_relocations']} exact relocations")


if __name__ == "__main__":
    main()
