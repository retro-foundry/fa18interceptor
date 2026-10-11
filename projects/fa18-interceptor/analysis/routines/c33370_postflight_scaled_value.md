# `$C33370` postflight scaled-value setup

Classification: **static-only dataflow**. This local postflight stage is not
covered by current P-code exports.

`source_amiga/observed/prepare_postflight_scaled_value.asm` reproduces
`$C33370-$C333B1` (66 bytes). It selects a `$C46184`-relative record, chooses
either a capped long from `$C45658` or a record-relative long shifted right by
10 and multiplied by five, and leaves the result in `d0`.

The value's physical or display meaning remains unproven.
