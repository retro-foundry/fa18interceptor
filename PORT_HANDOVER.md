# Port handover (2026-09-27)

Read this first when continuing the native C port. It records where the port
restarted, the rules it runs under, and what the previous session found.

## Rules

- `fa18_port` may use only its own C code, data read from the ADF at run time
  (the executable's tables, `pix/*` ILBM pictures, `text/*`), and the recorded
  control inputs (`captures/run075/playback.e9k`). It never reads or compiles
  in emulator output.
- Named C structs and a 320x200 chunky buffer. No Amiga memory model, no
  blitter or Copper emulation. Reproduce the visible result directly.
- Do not run Ghidra or its import/export scripts; the user has no clearance for
  it on this account. Use `scripts/disasm_game.py` (Capstone), the
  reconstructed asm in `source_amiga/observed/`, `analysis/`, and Engine9000
  traces.
- `scripts/check_native_build.py` (runs before every `fa18_port` build and as
  a ctest) and `scripts/native_frame_count.py` (the progress measure) are
  owned by the user. Don't edit them or `port/native_data_allowlist.txt`.
- Progress is one number: consecutive exact frames from run075 frame 200,
  printed as `NATIVE_FRAME_COUNT=` by `python scripts/native_frame_count.py`.

## Repository state

- Branch `coverage-accounting`, local head `4ba0a99`. The frame-gate loop
  (344 commits from frame 7993 on) was removed and `port/run075_tail_deltas.h`
  was purged from history. Backup of the old history: local tag
  `backup/before-gate-cleanup`.
- **Not yet pushed.** The user runs:
  `git push --force-with-lease=coverage-accounting:4bc91de origin coverage-accounting`
- `port/run075_tail_deltas.h` exists locally only, ignored via
  `.git/info/exclude`.
- Uncommitted: `scripts/check_native_build.py`, `scripts/native_frame_count.py`,
  `scripts/disasm_game.py`, `port/native_data_allowlist.txt`, the check wiring
  in `port/CMakeLists.txt`, new `port/disk.c`/`port/disk.h`, this file, plus
  older reverse-engineering edits to README.md, TRANSFER_NEW_CHAT.md and two
  `analysis/` coverage files that predate the port work.
- The check currently fails (15 recorded-data files, 61 problems), so
  `fa18_port` does not build. `fa18_glyph_contract_test` does not link
  (`display.c` needs `blit_job.c`); the other contract tests pass.

## Tools

- `python scripts/disasm_game.py` writes `build/disasm/segNNN.s` (all CODE
  hunks, relocations applied at the verified runtime bases, index in
  `build/disasm/index.txt`). `python scripts/disasm_game.py C0E27E 200` prints
  from an address.
- The executable is `local/extracted/f18_interceptor` (also `F-18 Interceptor`
  on the ADF): 185 hunks, 286 KB of CODE, 1.5 KB DATA, 11.5 KB BSS. Runtime
  bases come from `analysis/hunk_runtime_resolved.json` (120 segments; the
  rest get synthetic bases from `$C80000`).
- The ADF is OFS (`DOS\0`), root block 880. Files: `F-18 Interceptor`,
  `pix/frnt5`, `pix/inst5` (cockpit panel), `pix/splsh`, `text/*`, `config`.
  See `analysis/disk_game_resources.md`.
- Oracle: `build/port_run075_demo.fa18`, FA18RGB4 v1, frames 200..20987.
  Its checksums are standard Adler-32 of each frame's little-endian bytes.

## Findings so far

- The code is compiled C (Lattice-style `link a6` frames, stack arguments),
  so each routine decompiles to one C function and absolute addresses become
  named globals.
- Entry `$C0DEB0` is C startup; `$C53640` parses the command line and calls
  `main()` at `$C0E27E`. `main` loads resources (`$C0E078`), opens the
  display (`$C24DB0` returns `$C560` on success), sets palettes and calls into
  the game.
- Main loop `$C15D96`: calls the update `$C0EFD4`, `DisownBlitter`
  (`$C53FC0`), then `$C1612C`, which begins with `WaitBOVP`. Each iteration
  lasts as many video frames as the 68000 needs. See
  `analysis/routines/c15d80_outer_update_loop.md`.
- Menu text: `$C0FBE0` queues message codes 6, 100..109 at `$C4574A`. Codes
  resolve through the relative-word table at `$C3ED0A` to descriptors
  (4 header bytes, then the string). See
  `analysis/run024_flight_return_text_selection.md` and
  `analysis/routines/c32fce_static_text_compositor.md`.
- In the oracle, frames change at 234, 235, 236 (key `1` selects the demo),
  273 (screen clears), then 392 onward (cockpit zoom). 3D frames update every
  3 to 13 video frames.

## Open problem: timing

Because the loop is CPU-paced, the video frame on which each 3D image appears,
and possibly the elapsed time the simulation integrates, depends on 68000
speed. A native port cannot reproduce that exactly from code alone. Before
porting the flight path, confirm whether the update reads a vblank counter,
then raise with the user: either an input file giving each loop iteration's
start frame (recorded once from Engine9000, describing machine speed rather
than content; this would need a `--timing FILE` addition to the frame-count
script, which the user owns), or a relaxed comparison over presented images
in order. The menu frames (200..~391) should be unaffected.

## Next steps

1. The disk-backed executable skeleton is complete and committed. `fa18_port`
   accepts `--adf FILE --replay FILE --headless --to N --dump-rgb444 -`, or
   starts an SDL window, while `scripts/check_native_build.py` passes.
2. `$C0FBE0` and the positive `$C32D24` selector path are ported. The initial
   records are resolved from the executable's `$C3ED0A` relative table; the
   bounded `$C32F54-$C33168` compositor reads its palette, layout, and glyph
   data from `$C08490`, `$C41066`, and `$C3D8FC` at runtime.
3. `$C32C3A-$C32CB2` is ported for the inline follow-up after selector 109.
   It skips the prior NUL, reads the `$04,$00,$A2` glyph/attribute/layout
   triplet at `$C3F3D1`, then renders `SHIFT ESC ... RETURNS YOU TO THIS MENU`
   from `$C3F3D4` at `$19CA + $1E0 = $1BAA`. Frames 200..233 match exactly
   (`NATIVE_FRAME_COUNT=34`).
4. The first numeric menu command is ported: its press follows the observed
   `$C1BD78-$C1BDEC` mode-$7F route and its release follows `$C0FD10-$C0FDCE`.
   `$C2FD44-$C2FD56` clears four 32-pixel planar chunks per iteration; 1,144
   completed first-loop iterations are visible in frame 234, before the
   pending clear completes for frame 235. Frames 200..235 match exactly
   (`NATIVE_FRAME_COUNT=36`).
5. Frame 236 is the first mismatch: trace and render selector 101's queued
   `DEMO` label through the existing message-record and glyph path, rather
   than restoring a recorded label bitmap.
6. Continue one original routine/contract per commit, with
   `NATIVE_FRAME_COUNT` in each commit message. The CPU-paced flight timing
   issue remains a separate blocker before the 3D path.
