"""Locate frame-level mutations in a runtime memory region during a sealed replay.

This is a sampler, not an instruction writer trace.  It is useful when the
debugger's CPU watchpoint does not observe a region known to differ between
saved checkpoints.  With --stride and --sample-size it compares only stable
prefix fields of each record, avoiding per-frame scratch fields.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

from engine9000_bridge import Engine, ROOT
from profile_window import read_events


def parse_hex(value: str) -> int:
    return int(value, 0)


def sampled_bytes(engine: Engine, address: int, size: int, stride: int | None,
                  sample_size: int | None) -> bytes:
    raw = engine.memory(address, size)
    if stride is None:
        return raw
    assert sample_size is not None
    return b"".join(raw[offset:offset + sample_size] for offset in range(0, size, stride))


def changed_ranges(before: bytes, after: bytes, stride: int | None,
                   sample_size: int | None) -> list[dict]:
    changed = [index for index, (left, right) in enumerate(zip(before, after)) if left != right]
    if stride is None:
        return [{"offset": f"${index:X}", "before": f"${before[index]:02X}",
                 "after": f"${after[index]:02X}"} for index in changed]
    assert sample_size is not None
    records = sorted({index // sample_size for index in changed})
    return [{"record_index": index, "record_offset": f"${index * stride:X}",
             "changed_prefix_byte_count": sum(
                 before[index * sample_size + field] != after[index * sample_size + field]
                 for field in range(sample_size))}
            for index in records]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--restore", type=Path, required=True)
    parser.add_argument("--playback", type=Path, required=True)
    parser.add_argument("--address", type=parse_hex, required=True)
    parser.add_argument("--size", type=parse_hex, required=True)
    parser.add_argument("--frames", type=int, required=True)
    parser.add_argument("--stride", type=parse_hex)
    parser.add_argument("--sample-size", type=parse_hex)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", type=Path, default=ROOT / "local" / "fa18.uae")
    args = parser.parse_args()
    if args.output.exists():
        raise FileExistsError(args.output)
    if (args.stride is None) != (args.sample_size is None):
        parser.error("--stride and --sample-size must be supplied together")
    if args.stride is not None and (args.stride <= 0 or args.sample_size <= 0 or
                                    args.sample_size > args.stride or args.size % args.stride):
        parser.error("require 0 < sample-size <= stride and size divisible by stride")

    args.output.mkdir(parents=True)
    events = read_events(args.playback)
    engine = Engine(args.config.resolve(), args.output / "saves")
    try:
        engine.core.retro_run()
        state = args.restore.read_bytes()
        if not engine.core.retro_unserialize(state, len(state)):
            raise RuntimeError("Core rejected save state")
        previous = sampled_bytes(engine, args.address, args.size, args.stride, args.sample_size)
        initial_digest = hashlib.sha256(previous).hexdigest()
        mutations = []
        for frame in range(1, args.frames + 1):
            for kind, values in events.get(frame, []):
                engine.event(kind, values)
            engine.core.retro_run()
            current = sampled_bytes(engine, args.address, args.size, args.stride, args.sample_size)
            if current != previous:
                mutations.append({"frame": frame,
                                  "changed": changed_ranges(previous, current, args.stride, args.sample_size),
                                  "before_sha256": hashlib.sha256(previous).hexdigest(),
                                  "after_sha256": hashlib.sha256(current).hexdigest()})
                previous = current
        payload = {"authority": {"restore": str(args.restore), "playback": str(args.playback)},
                   "address": f"${args.address:06X}", "size": args.size,
                   "stride": args.stride, "sample_size": args.sample_size,
                   "frames": args.frames, "initial_sha256": initial_digest,
                   "final_sha256": hashlib.sha256(previous).hexdigest(), "mutations": mutations}
        (args.output / "memory_region_mutations.json").write_text(json.dumps(payload, indent=2) + "\n")
        print(json.dumps({"mutating_frames": len(mutations),
                          "first_frame": mutations[0]["frame"] if mutations else None,
                          "output": str(args.output)}))
    finally:
        engine.core.retro_unload_game()
        engine.core.retro_deinit()


if __name__ == "__main__":
    main()
