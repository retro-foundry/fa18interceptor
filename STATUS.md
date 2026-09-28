# Status

Updated 2026-09-28.

## Summary

| Area | State |
| --- | --- |
| Native game | Runs from the run075 menu into flight, live in an SDL2 window at 50 Hz |
| Frame parity (run075, from the frame-392 snapshot) | 8 of 10 frames 393-402 pixel-exact; first diverging frame 398 |
| Frame parity (run075, from the menu) | frame 500: 99.6% of pixels match; frame 3000: flying, path has drifted (94%) |
| Translation | 540 routines, 32,191 instructions; ~70% of CPU cycles in translated code |
| Recreated C source | 50 routines in `port/game/`, 110,000+ calls proven over 3,000 frames |
| Known gap | CPU/DMA bus timing (the cause of the frame-398 divergence and flight drift) |

## The game program

- One Amiga Hunk executable (`F-18 Interceptor` on the ADF): 185 hunks,
  286 KB CODE, 1.5 KB DATA, 11.5 KB BSS, loaded into Slow RAM at `$C00000+`
  (`analysis/hunk_runtime_resolved.json`).
- It is compiled C (Lattice-style `LINK A6` frames and stack arguments) plus
  hand-written assembly for the renderer, so most routines map to one C
  function each.
- C startup at `$C0DEB0`; `main()` at `$C0E27E`; the main loop at `$C15D96`
  calls the update `$C0EFD4` each iteration. The loop is CPU-paced: one
  iteration lasts as many video frames as the 68000 needs.
- The game uses Kickstart heavily (interrupt servers, graphics.library blitter
  ownership, `WaitBOVP`); about half of all executed instructions are in ROM.

## Emulator and recordings

- Engine9000 fork (UAE core) driven by `scripts/engine9000_bridge.py`:
  deterministic replay, savestates, RAM dumps, screenshots, instruction traces
  and custom-register write logs.
- Sealed recordings in `captures/`. run075 is the main target: menu, key `1`
  selects the demo at frame 230, cockpit from frame ~392, flight through
  frame 21,069.

## Native machine and translation

- `port/machine/`: bus and memory map, blitter (area, fill, descending, line;
  busy timing), Copper, bitplane display, CIAs, interrupts, UAE savestate
  loader, keyboard/mouse/replay input. Blitter output matches the real Chip
  RAM exactly for replayed register writes.
- `tools/recomp/recomp.py`: whole-program translation seeded from traces and
  the fallback log, decoded with Musashi's own tables. Kickstart ROM runs on
  the Musashi interpreter.
- Translated and interpreter-only runs end with byte-identical RAM and
  registers over 3,000 frames; gcc and MSVC builds match each other.

## Recreated C source

| File | Routines |
| --- | --- |
| `render_line.c` | `setup_line`, `draw_line` (`$C2FA7E`), `reset_line_style` |
| `render_polygon.c` | polygon edge (`$C305AA`), mask compositing and clearing |
| `render_state.c`, `render_page.c`, `render_span.c` | blit starts, state blocks, draw page, span bounds |
| `fixed_math.c` | `sin_cos`, `y_rotation_matrix`, `rounded_divide`, `attenuate_offset` |
| `audio.c` | voice output, master volume fade, voice programs, channel interrupts |
| `text.c`, `numbers.c` | glyph plotting, packed BCD |
| `control_records.c` | control-record fields, selection, rate class, player reset |
| `post_input.c`, `notify.c`, `stages.c`, `view.c`, `player_input.c`, `screen_frame.c` | stage callbacks, cadence, zoom, mouse buttons, frame lists |

Every routine passes the shadow proof (all live registers, flags, memory and
custom-chip writes identical to the original on every call) and the poison
check. Ranked candidates: `python tools/recomp/port_candidates.py` (86 ready).

## Analysis assets

- Memory map: `analysis/memory_map.md`.
- 425 routine reports in `analysis/routines/`.
- 1,176 byte-exact assembly slices in `source_amiga/observed/`
  (`python scripts/verify_reconstructions.py`), 51,256 bytes.
- Raw P-code for 122 captures in `pcode/raw/` (16% of CODE bytes).
- Coverage accounting: `analysis/coverage.json`.

## The earlier port

`fa18_port` (the top-level `port/*.c`, built by `port/CMakeLists.txt`) was a
bottom-up port of about 400 routine slices with 208 contract tests. It never
rendered the flight view (it stopped at frame 273, blocked on CPU-paced
timing) and is superseded. Its modules are reused where they pass the shadow
proof; so far, packed BCD and the rounded divide.
