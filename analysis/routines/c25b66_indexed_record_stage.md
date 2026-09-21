# Indexed record stage at `$C25B66`

Classification: **bounded structural stage**. In the sealed run003 frame-6,000
normal update, `$C22D88` calls `$C25B66` and execution returns to `$C22D8E`
after 2,603 instructions. Future playback is empty. Canonical P-code is
`pcode/raw/run003_6000_c25b66/` (1,831 RAM instruction starts, 12,079
operations, 29 observed call targets).

The packet enters from the capped `$C22C80` indexed-record stage and reaches
the established current-record selector `$C13D84`, matrix helper `$C2D408`,
and multiple nested record/update helpers. Its record ownership and aggregate
semantics are not yet assigned. This is a complete caller-return packet, not a
claim that `$C25B66` is a standalone source-level function.
