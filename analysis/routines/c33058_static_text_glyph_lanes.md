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

The nested run060 `$C330FE` packet now identifies one submitted lane as plane
4 offset `$04BA`, with seven source-byte rows and the set-form mode. See
`c330fe_strided_long_mask_update.md`.

A 520-instruction trace of the enclosing `$C33058` entry at the same
checkpoint proves all four slots use that `$04BA` offset in descending plane
order: plane 4 `$018E3A`, plane 3 `$016EFA`, plane 2 `$014FBA`, plane 1
`$01307A`. With `D6=$0009`, lanes 4 and 1 take `$0BFA|$F000` while lanes 3 and
2 take `$0B0A|$F000`. The complete four-lane before/after words are exercised
by `fa18_submit_static_glyph`, using `FA18StaticGlyphSubmission` and the
native `FA18PlanarPage` rather than the original pointer table.

This ports `$C33058` after its caller has selected a glyph stream and supplied
the proved native lane inputs. Glyph-table selection, layout cursor updates,
other destination offsets, and the record's display geometry remain open.
