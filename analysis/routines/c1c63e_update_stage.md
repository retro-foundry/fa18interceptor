# `$C1C63E` observed update stage (Hunk 8 +`$2AE`)

Classification: **structural**. This routine is a bounded, expensive callee in
the `$C0EFD4` update sequence. Its semantic role is still unknown.

## Evidence packet

- Restore: `captures/baseline_menu/state.bin`.
- Playback: `local/start_demo.e9k`, with no event after menu selection.
- Breakpoint: `$C1C63E`, armed at frame 600 and hit at frame 607.
- Exit: return to `$C0F01C` after 7,774 instructions.
- Guard: no input event occurs after the breakpoint.
- Ghidra packet: `pcode/raw/no_key_c1c63e/`, containing 3,095 observed RAM
  starts and 19,758 P-code operations. Hunk annotation maps 2,999 starts; 96
  are explicitly unmapped rather than attributed to a game segment.

The first observed instructions compare bytes at `$C45854` and `$C45855`, then
operate on longwords at `$C45A66` and `$C45A6E`. Their field meanings are
unknown.

The byte-exact observed setup prefix through the `$C22C80` call is
`source_amiga/observed/prepare_c1c63e_update_stage.asm`
(`$C1C63E-$C1C6BB`, 126 bytes). It deliberately retains neutral stage-field
names pending broader cross-scenario evidence.

## Observed structure

The packet contains 74 observed RAM call targets. The most frequent game-RAM
targets are `$C2E6DA` (10 calls), `$C2E5F6` (6), `$C2E47A` (4), `$C15138` (4),
and `$C25B66`, `$C26EBE`, `$C2651E`, `$C230E8`, `$C231A2` (3 each).

Thirty-one observed instructions directly reference custom-chip registers.
They include a read from `$DFF00A` and accesses to `$DFF034`, `$DFF09A`, and
`$DFF09C`. This establishes hardware interaction inside the stage but not a
display, audio, or interrupt-management interpretation for the whole routine.

This routine returns before `$C0EFD4` writes its next observed marker `$0020`
to `$C45AD4`. It is a suitable parent boundary for separating the earlier input
phase from the later update sequence.

## run060 active-root instance

The sealed run060 long context enters this stage from `$C0F016` and follows
`$C1C6B6 -> $C22C80 -> $C25B66`. That instance selects root `$C46184`, updates
its moving pose fields, publishes its angle tuple, and composes its orientation
matrix. This promotes that **instance** to a flight-pose update boundary, while
the routine as a whole remains a generic multi-record update stage. See
`../data/run060_active_flight_update_chain.md`.
