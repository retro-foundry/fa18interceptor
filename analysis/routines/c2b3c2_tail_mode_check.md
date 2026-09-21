# Tail-mode check at `$C2B3C2`

Classification: **behavioural early-return prefix**. The deterministic
training frame-600 parent calls `$C2B3C2` from `$C0F380` and returns to
`$C0F386` in three instructions. P-code is
`pcode/raw/training_600_c2b3c2/`.

The entry compares `$C458A6` with one. The observed value differs, so it
branches to the adjacent `$C2B3C0` `RTS`. The later mode-one continuation is
outside this packet. The complete observed prefix `$C2B3C0-$C2B3CB` is
byte-exact source in `source_amiga/observed/check_tail_mode_one.asm`.
