# `$C305D6` unequal-pair blitter continuation

Classification: **runtime-backed dataflow/control flow**. Multiple existing
P-code exports enter this continuation from the `$C305AA` unequal-pair range,
including `attract_cockpit_c2fede` and `run001_c1f6f8_record_walk_stage`.

`source_amiga/observed/continue_unequal_pair_blitter_setup.asm` is byte-exact
for `$C305D6-$C30667` (146 bytes). It derives the unequal pair delta, selects
one of the sign/order adjustment paths, computes a bounded line parameter,
sets a control bit when needed, and joins the existing `$C30668` blitter
submission slice.

This is register and hardware-setup dataflow only; no graphics/object meaning
is assigned to its inputs or resulting blit.
