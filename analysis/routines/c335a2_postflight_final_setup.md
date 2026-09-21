# `$C335A2` postflight final setup

Classification: **static structural/dataflow**. This 20-byte range is not
covered by the current P-code exports.

It invokes local `$C33FB4` with `d0 = $D4` and `d1 = 1`, then invokes local
`$C33AD6` with `d0 = $CE`. Execution continues at `$C335B6`, the entry of the
already reconstructed `$C333B2` table-pass tail.

The effects of the two local helpers are not established here.
