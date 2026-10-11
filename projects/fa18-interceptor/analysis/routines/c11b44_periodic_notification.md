# `update_periodic_notification_code` at `$C11B44` (Hunk 0 +`$3C94`)

Classification: **behavioural**. The no-future-input training frame-600 trace
enters this routine from `$C0EFF0` and returns there in nine instructions. Its
canonical P-code is `pcode/raw/training_600_c11b44/`.

The observed expiry path decrements `$C45890`, reloads it to 8 when it reaches
zero, and writes `$86` to `$C4588E`. The byte-exact 108-byte static routine also has
the countdown-dependent `$06`, `$04`, and clear-code paths. The semantic use
of those notification values remains unassigned.
