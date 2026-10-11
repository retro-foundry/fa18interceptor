# `$C33C18` postflight shared variant path

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

The entry restores `$C4593E:$C45940` into `d0:d1` and branches to `$C33CC4`
when `d0 <= 0`. It runs `$C345A0` with selector `$A`, then requires nonnegative
`$C459C0`. A second stage selects the record at `$C46184 + $C458DE`, scales
its `$4A` word by `($4C * value) / $8CA0`, sets selector `$D`, and calls
`$C347F2`.

If that record word is at most `$7F00`, it conditionally clears bit 0 at
`$4(a0)` and updates `$C45B44/$C45B46` before calling `$C31D16`. The next
boundary is `$C33CC4`. Field and helper semantics remain unproven.
