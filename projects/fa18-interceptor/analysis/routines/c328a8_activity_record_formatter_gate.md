# `$C328A8-$C328D9`: activity-gated selected-record formatter prefix

Classification: **static dataflow in a runtime-observed parent activity path**.

`source_amiga/observed/gate_c328a8_activity_record_formatter.asm` is byte
exact for `$C328A8-$C328D9`. The parent activity stage invokes `$C328A8`; this
prefix requires a positive `$C45844`, decrements it, selects
`$C46184 + $C458DE`, loads bytes `+$5F` and `+$63`, masks the latter to its
high nibble, and dispatches zero to `$C329DA`.

The field ownership, later literal labels, and screen effect require a bounded
execution trace before a stronger claim.
