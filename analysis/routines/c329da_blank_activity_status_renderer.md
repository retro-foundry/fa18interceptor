# `$C329DA-$C32A43`: blank activity-status renderer

Classification: **static formatter/render-submission dataflow**.

`source_amiga/observed/render_blank_activity_status.asm` is byte exact for
`$C329DA-$C32A43`.  The earlier `$C328A8` gate branches here when its masked
record byte `+$63` is zero.  This block writes three spaces to the scratch
buffer at `$C457FA`, makes a `$C32794` glyph-renderer call, reloads the same
scratch-buffer pointer and makes a second glyph-renderer submission through a
tail branch.  The following address `$C32A44` begins a separate transform
entry and is excluded from this slice.

The block's two renderer configurations differ in the fixed `A4` value
(`$1CC6` then `$1B86`); their display position, style, and gameplay ownership
are not assigned.  In particular, the blank bytes do not prove that this is a
weapon, target, or any other named game-state display.
