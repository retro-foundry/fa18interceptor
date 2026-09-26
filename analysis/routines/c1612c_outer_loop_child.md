# `$C1612C` complete outer-loop child

Classification: **complete structural packet**. This call immediately
precedes the `$C15DB2 -> $C15D96` outer-loop back-edge.

- Restore: `captures/baseline_menu/state.bin`.
- Playback: `local/start_demo.e9k`; no input follows the armed frame.
- Breakpoint: `$C1612C`, armed at frame 600 and hit at frame 607.
- Exit: `$C15DB2` after 1,022 stepped instructions.
- P-code: `pcode/raw/no_key_c1612c_outer_child/` (45 observed RAM starts /
  207 operations).

The packet is complete for this no-input scenario. It first calls the proved
graphics.library `WaitBOVP(A0=$C1822A)` wrapper, then handles pointers selected
through `$C4566C`. Its activity path also uses `WaitBOVP` and `WaitBlit` before
returning to the outer-loop back-edge. This identifies `$C1612C` as the
display-synchronization child of the structural loop. The API argument and its
live 320×200 field prefix establish `$C1822A` as the graphics ViewPort; see
[`run060_display_viewport_structure.md`](../data/run060_display_viewport_structure.md).
The remaining pointer-operation semantics remain unassigned.

Its byte-exact observed setup prefix is
`source_amiga/observed/prepare_outer_loop_child.asm`
(`$C1612C-$C1617D`, 82 bytes). It selects entries from pointer tables at
`$C182BA` and `$C182C2` before the later activity test.

The tail is byte-exact at
`source_amiga/observed/run_outer_loop_child_tail.asm`
(`$C1617E-$C16283`, 262 bytes). Together the two slices cover the complete
static `$C1612C-$C16283` child (344 bytes), matching the bounded packet's
entry and return edge.
