# `adjust_zoom_scale` at `$C1BAE2` (Hunk 5 +`$5C4A`)

Classification: **behavioural**. The run003 `]` press becomes raw `$1B` and
enters `$C1BAE2`; it returns in 181 instructions. `[` becomes raw `$1A` and
enters `$C1BB02`; it returns in 180 instructions. P-code packets are
`pcode/raw/run003_right_bracket_key_dispatch/` and
`pcode/raw/run003_left_bracket_key_dispatch/`.

In the observed context, `]` arithmetic-shifts `$C45A42` right while it is
above `$20`; `[` shifts it left while it is below `$80`. Both request display
update mode 3 and encode whether the scale equals `$80` in `$C457DD` bit 7.
The context-guard branches to `$C1B80A` and `$C1B890` are static and were not
taken in these packets.
