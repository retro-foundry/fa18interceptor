# `$C33058-$C330FD`: static-text glyph lane renderer

Classification: **renderer dataflow**, with a run060 text-onset path entry.

`source_amiga/observed/render_static_text_glyph_lanes.asm` is byte exact for
`$C33058-$C330FD`. It converts the glyph-table address in `A3` to a source
pointer, then performs four `$C330FE` mask-update calls. Each call selects
`$0B0A` or `$0BFA` from one bit of `D6`, ORs in `D3`, and applies the result to
one of four `A5`-relative destinations plus `D5`.

After the four lanes, clear bit 0 of `$C457DC` advances `(A1,A2)` by `(4,1)`,
saves `(A1,A2,A4)` to `$C456FE`, and re-enters `$C32EF6`. With that bit set,
a positive `$C457DE` saves the layout unchanged; otherwise it advances the
pair before saving and returns.

The run060 frame-9285 native-checkpoint trace reaches `$C33058` after the
observed `$C32EF6 -> $C32EE4 -> $C32F54 -> $C32FCE -> $C33002` path. The
trace validates this is a live glyph-render route during success-text onset;
it does not establish a landing result rule, pixel equivalence under stepping,
or the semantic identity of the four destination lanes.
