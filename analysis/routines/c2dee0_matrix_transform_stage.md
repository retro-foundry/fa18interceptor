# `$C2DEE0` matrix-transform stage (Hunk 32 +`$AD8`)

Classification: **structural**. This bounded routine is an immediate user of
the native-angle matrix-builder; it is not yet assigned to a player, camera, or
world-object subsystem.

## Evidence packet

- No-input `start_demo` replay from `captures/baseline_menu/state.bin`.
- Breakpoint `$C2DEE0`, frame 607; observed return `$C2D704`.
- 288 instructions, no input after breakpoint.
- Ghidra P-code: `pcode/raw/no_key_c2dee0/`, 288 observed starts / 2,302
  operations, all mapped to verified Hunk 32.

The routine saves `D1/A0/A1`, checks the signs of its three angle-word inputs,
sets `A1` to `$C45B90`, then calls `build_rotation_matrix` at `$C2DF02`.
For the observed zero-angle input, that produces the identity `$4000`/zero
matrix documented in [`c2e47a_rotation_matrix.md`](c2e47a_rotation_matrix.md).

After the matrix call, this packet reads nine word positions at `$C46204`
with offsets `0, 2, 4, 6, 8, $A, $C, $E, $10`, combines them with fixed-point
multiplications, and reads a word table at `$C3DD92`. It returns three signed
values in `D4-D6` after left-shifting them by three. The meanings of both
memory regions and the output vector are unknown.

The enclosing Hunk-32 sequence reaches this call site at `$C2D700` after
forming `A4 = A1 + $80` at `$C2D6FC`. Immediately before it, the same sequence
updates words at offsets `$56-$5A` from `A1` and branches on nearby flag/angle
fields. It reaches `$C2D700` twice in the bounded `$C1C63E` packet. These are
object-layout observations only; the object type and field meanings are
unknown.
