# C port handoff

One page by rule (PORT.md, "Rules that change"). The previous 1,700-line
handoff is in git history before commit `97dd4d3c`.

## Goal

Recreated, readable C source for the whole game (PORT.md, "Deliverable").
The translated code (Stage A) running on the machine layer (Stage B) is the
scaffold. It must reach frame parity with Engine9000 (Stage C) while
routines are rewritten as readable C under differential tests (Stage D).

## Current state (2026-09-28)

| Check | Result |
| --- | --- |
| run075 frames 393-402 from the frame-392 snapshot | 8/10 pixel-exact; first diverging frame **398** |
| run075 from the menu (frame 0) with the recorded input, frame 500 | 99.6% pixels match |
| run075 from the menu, frame 3000 | flying and rendering correctly; flight path drifted (94%) |
| translated vs interpreted, 3,000 frames | byte-identical RAM and registers |
| blitter vs real register writes (frame 393) | byte-identical Chip RAM |
| gcc build vs MSVC build | identical |

- 540 generated routines, 32,191 instructions; 70% of CPU cycles in
  generated code over the 3,000-frame run. The rest is mostly Kickstart ROM.
- `fa18_recomp --window` runs at 50 Hz with live keyboard and mouse input.
  Click to capture the mouse; F12 releases it.

## Next blockers

1. **Timing parity.** The game code runs from Slow RAM, which shares the
   chip bus on an A500. The CPU should stall while bitplane, Copper and
   blitter DMA hold the bus. Model the DMA slot allocation (UAE `custom.c`,
   `blitter.c`) and CPU wait states for Chip/Slow/custom accesses, then
   re-run `scripts/recomp_parity.py`. This fixes frame 398 and the long-run
   flight drift.
2. **Stage D: readable C.** Start with the routines that already have
   hand-ported counterparts in `port/` (line/polygon/projection/matrix
   modules). Swap each into the dispatch table and prove it with a
   differential test (same machine state in, same memory, registers and
   hardware writes out). Keep parity unchanged.

## Commands

```sh
cmake -S port/recomp -B build/recomp-cmake && cmake --build build/recomp-cmake --config Release
build/recomp-cmake/Release/fa18_recomp.exe --state captures/run075/restored-state.bin \
    --rom local/system/kick13.rom --replay captures/run075/playback.e9k --window --frames 0
sh scripts/build_recomp.sh                                # headless gcc build
python scripts/recomp_parity.py --start 392 --frames 10   # the metric
python tools/recomp/recomp.py --trace <trace.jsonl> --seeds <fallback.json>   # regenerate
python scripts/recomp_lockstep.py <trace.jsonl> 120000 build/recomp/fa18_recomp.exe \
    --state <state.bin> --rom local/system/kick13.rom --frames 10 --no-recomp
build/recomp/blit_replay.exe CHIP.bin WRITES.txt OUT.bin  # blitter differential
```

Snapshots and traces come from `scripts/engine9000_bridge.py`
(`--frames N`, `--trace-frames N`). Oracle frames are cached in
`build/recomp/oracle/`.

## Backlog

- Kickstart ROM calls: inventory the entry points used, then replace them
  with C (OS high-level emulation).
- Cold start from the ADF, instead of a snapshot (needs disk DMA or a
  trackdisk replacement).
- Seed the generator from the existing `pcode/raw/*` exports; no new Ghidra
  work is needed.
- Audio (Paula) is not modelled yet.
