# `select_training_control_record` prefix at `$C12950` (Hunk 0 +`$4AA0`)

Classification: **behavioural prefix and early-return path**. The deterministic
training frame-600 state reaches `$C12950` from the parent’s `$C0F370` branch
and returns to `$C0F376` in 27 instructions. P-code is
`pcode/raw/training_600_c12950/`.

The routine publishes `$C46184 + ($C458DC << 9)` at `$C18210`, snapshots two
input bytes into locals, then returns early through `$C1316E` because
`$C45795` is zero. The static prefix through that branch is byte-exact source;
the intervening `$C129AE-$C1316D` paths remain outside this observed contract.
