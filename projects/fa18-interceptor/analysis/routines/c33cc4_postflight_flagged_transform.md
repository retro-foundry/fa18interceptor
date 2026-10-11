# `$C33CC4` postflight flagged transform wrapper

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It calls `$C33CD2` only when byte `$C457AE` is zero, then returns. The
transform's internal state/matrix behavior is separately bounded at
`$C33CD2–$C33DA3`; the flag's user-visible meaning is unproven.
