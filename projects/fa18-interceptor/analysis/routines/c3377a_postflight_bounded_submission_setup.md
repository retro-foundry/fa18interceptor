# `$C3377A` postflight bounded submission setup

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

The entry makes two local setup calls with fixed arguments (`$69, 0` to
`$C33FB4`, then `$6B` to `$C33B06`). It forms `d0 = $6E + $C45988` and only
continues when `5 < d0 < $13B`.

Within that interval it derives `d1 = $56 + $C458D8` and invokes external
helpers `$C2F60A`, `$C2F5C0`, `$C2F5D4`, then `$C2F60A` again using fixed
register constants. `$C337DC` is the next boundary. The helper effects and
user-visible role remain unproven.
