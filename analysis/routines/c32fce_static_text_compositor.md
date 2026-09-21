# `$C32FCE`: static-text byte compositor

Classification: **scenario-backed text-to-glyph dataflow**.  This is a
renderer/dataflow name, not a mission-state or message-selector claim.

## Evidence packet

- Authority: `build/run024_qualification_transition_trace/trace.jsonl`.
- At chipset frame 590, trace rows 1,021--1,052 take this sequence with no
  later input in the stepped interval.
- `$C330D8` advances the current layout cursor `A1` by four and `$C330DA`
  advances text cursor `A2` by one.  `$C330DC` saves `A1/A2/A4` to
  `$C456FE`.
- `$C32F54` restores `A1=$C410D2`, `A2=$C3F31E`, and `A4=$0EB2`.
- `$C32FCE` executes `MOVE.B (A2),D4`, reading byte `$45`.  The baseline
  Slow-RAM inventory identifies `$C3F31E` as `E` within
  `R5 ... QUALIFICATION: REQUIRED FOR MISSIONS` at `$C3F302`.
- `$C33002` normalizes the byte by subtracting `$20`; `$C3305A` uses the
  resulting doubled index with the glyph-offset table based at `$C3D8FC`.

This is direct runtime evidence that the main menu's qualification line is
consumed by the text compositor while the game transitions to the visible
qualification screen.  It does not establish what selected `$C3F31D` as the
initial cursor, nor whether that menu choice changes a persistent status byte.

## Controlled selectable-missions observation

The key-6 run029 derivative reaches `$C32FCE` at frame 277 with selector state
`D0=$0040` and `A2=$C3F40A`.  The entry reads byte `$4C` (`L`), which is inside
the static payload `SELECTABLE MISSIONS` beginning at `$C3F401`, and takes the
normal branch to `$C33002`.  This proves that the visible submenu heading uses
the same static-text/glyph path as the qualification line.

The byte-exact setup entry is
`source_amiga/observed/prepare_static_text_glyph.asm`.  It does not identify
the producer of the selector-64 queue word.
