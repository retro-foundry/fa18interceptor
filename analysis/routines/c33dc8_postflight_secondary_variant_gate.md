# `$C33DC8` postflight secondary variant gate

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It returns to `$C33DA4` when `$C459C0` is negative, when the high nibble of
selected-record byte `$63` equals `$10`, or when the first word of
`$C45942:$C45946` is negative. Values at most `$60` branch to `$C33ED8`.

`$C33DFE` begins the remaining path. State field meanings are unproven.
