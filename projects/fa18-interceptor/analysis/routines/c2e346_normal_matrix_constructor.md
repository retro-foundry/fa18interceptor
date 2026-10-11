# Normal-route matrix constructor at `$C2E346`

Classification: **behavioural packet with byte-exact source**. In sealed
run003 frame 6,000, the capped normal matrix path reaches
`$C2DB2E -> $C2E346 -> $C2DB32`. The child returns in 38 instructions with
empty future playback. Canonical P-code is
`pcode/raw/run003_6000_c2e346_matrix_child/` (38 RAM instruction starts, 338
operations, one observed call target).

The complete static constructor is byte-exact source in
`source_amiga/observed/build_single_angle_matrix.asm`. The enclosing `$C2D99C`
and `$C2DB18` packets remain capped at the fixed 3,000-instruction limit.
