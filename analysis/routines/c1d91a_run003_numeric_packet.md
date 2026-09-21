# Run003 numeric packet at `$C1D91A`

Classification: **bounded structural packet**. In sealed run003 frame 6,000,
the capped `$C1D974` route reaches `$C1CF2C -> $C1D91A`; this packet returns to
`$C1CF32` after 1,141 instructions with empty future playback. Canonical
P-code is `pcode/raw/run003_6000_c1d91a/` (520 RAM instruction starts, 3,920
operations, six observed call targets).

It joins the existing byte-exact static source tail
`source_amiga/observed/fixed_point_stage_tail.asm`. The complete dynamic packet
is retained separately because its observed call tree is wider than that static
tail; arithmetic semantics remain unassigned.
