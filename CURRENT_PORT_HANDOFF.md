# C port handoff

One page by rule (PORT.md, "Rules that change"). The previous 1,700-line
handoff is in git history before commit `97dd4d3c`.

## Goal

Translated original code (Stage A) running on the native machine layer
(Stage B) reaches frame parity with Engine9000 on every sealed run (Stage C),
then becomes readable C routine by routine (Stage D).

## Current state (2026-09-28)

| Run | Start | Frames | First diverging frame | Exact frames |
| --- | --- | --- | --- | --- |
| run075 | 392 (snapshot) | 393-402 | **398** | 7 / 10 |

- Frame 402 draws the cockpit, sky and runway from the translated path; 81.5%
  of pixels match the oracle.
- Translated and interpreter-only runs end with byte-identical RAM,
  registers and frames (`--ram-out` + `cmp`).
- 448 generated routines, 30,026 instructions. About 42% of CPU cycles run
  in generated code; most of the rest is Kickstart ROM (interpreted).
- Lockstep against the real instruction trace matches the first 5,888
  instructions, then splits on blitter-busy polling (timing).

## Next blocker

**Chip-bus contention.** The game runs from Slow RAM, which shares the chip
bus on an A500. The real CPU stalls while the blitter and bitplane DMA use
the bus; the native CPU never stalls. Native runs about one frame ahead: it
skips the real blank frame 398 and shows the instruments at 400 instead of
401. Model the CPU wait states for Chip/Slow/custom accesses during blits
(UAE `blitter.c` / `custom.c` DMA slot allocation is the reference), then
re-run the parity metric.

## Commands

```sh
sh scripts/build_recomp.sh                          # build (gcc)
python scripts/recomp_parity.py --start 392 --frames 10  # the metric
python tools/recomp/recomp.py --trace <trace.jsonl> [--seeds fallback.json]  # regenerate
python scripts/recomp_lockstep.py <trace.jsonl> 120000 build/recomp/fa18_recomp.exe \
    --state <state.bin> --rom local/system/kick13.rom --frames 10 --no-recomp
```

Snapshots and traces come from `scripts/engine9000_bridge.py`
(`--frames N`, `--trace-frames N`). Oracle frames are cached in
`build/recomp/oracle/`.

## Backlog (measured)

- Interpreted game instructions per 10 frames: ~6,800. Feed
  `--fallback-log` output back to the generator as `--seeds`. Also seed
  from the existing `pcode/raw/*` exports; no new Ghidra work is needed.
- Kickstart ROM calls: inventory the entry points the game uses, then
  replace them with C (OS high-level emulation).
- Cold start from the ADF, instead of a snapshot.
- Keyboard/mouse replay events are not yet fed into `fa18_recomp`.
- MSVC/CMake target (currently gcc via `scripts/build_recomp.sh`).
