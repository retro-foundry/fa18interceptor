# `cycle_radar_range` at `$C1B1C8` (Hunk 5 +`$6570`)

Classification: **behavioural**. The first run003 `R` press becomes raw Amiga
key `$13`. Its `$C33186` child returns to `$C1B1CE` in 1,924 instructions, and
an independent post-helper trace completes to `$C0F45C` in 26 instructions.
Canonical P-code is `pcode/raw/run003_r_command_child/` and
`pcode/raw/run003_r_post_helper/`.

The handler sets `$C4599A` bit 6, selects `$C46184 + $C458DE`, and cycles the
low nibble of byte `$63` through 9, 11, and 13. It then writes display-update
mode 3 at `$C4583B`. The radar range name follows the recorded `R` control and
GAME.md; the record's wider ownership remains unresolved.
