# `$C2F826` primary renderer lane-mask targets

Classification: **partially runtime-backed dataflow**. The `$C2F786` table
loaded by the runtime-backed `$C2F5F4` entry resolves to 16 entries within this
block. `pcode/raw/attract_1800/` executes the complete `$C2F84E-$C2F857`
two-lane-set entry; all other entries remain static-only.

`source_amiga/observed/apply_primary_renderer_lane_masks.asm` is byte-exact
for `$C2F826-$C2F8CF` (170 bytes). `$C2F830` is an adjacent four-lane XOR
entry. The table-selected entries encode all sixteen combinations of four
lanes: a clear lane ANDs `D0-D3` through `A3-A0`, and a set lane ORs
`D4-D7` through the same pointer order.

This confirms the target table's masked four-lane memory-combine contract. It
does not assign a pixel, bitplane, or object interpretation to the lanes.
