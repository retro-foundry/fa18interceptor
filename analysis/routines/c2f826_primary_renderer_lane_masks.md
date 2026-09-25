# `$C2F826` primary renderer lane-mask targets

Classification: **partially runtime-backed planar pixel behavior**. The `$C2F786` table
loaded by the runtime-backed `$C2F5F4` entry resolves to 16 entries within this
block. `pcode/raw/attract_1800/` executes the complete `$C2F84E-$C2F857`
two-lane-set entry; deterministic run060 executes the all-clear `$C2F826`
entry. Other table targets retain their byte-exact static contracts unless
separately traced.

`source_amiga/observed/apply_primary_renderer_lane_masks.asm` is byte-exact
for `$C2F826-$C2F8CF` (170 bytes). `$C2F830` is an adjacent four-lane XOR
entry. The table-selected entries encode all sixteen combinations of four
lanes: a clear lane ANDs `D0-D3` through `A3-A0`, and a set lane ORs
`D4-D7` through the same pointer order.

Together with the one-hot `$C2F766` table, `$C2F688`'s 40-byte row offset,
and run060's four Chip RAM writes through the all-clear `$C2F826` target,
these lanes are planar pixel words. See `c2f688_planar_pixel_pipeline.md`.
The current Copper display selection and depicted object remain unassigned.
