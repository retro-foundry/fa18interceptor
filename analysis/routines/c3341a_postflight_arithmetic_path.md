# `$C3341A` postflight arithmetic path

Classification: **static structural/dataflow**. This continuation is not
covered by current P-code exports.

The entry divides incoming `d0` by 10, stores the quotient at `$C45B1E`, then
uses `$C45B22` and calls `$C25A08`/`$C259C2` to transform a packed long. It
combines that result with `$C45988/$C458D8`, conditionally calls `$C2F5D4`,
and forms a second scaled value passed to `$C33F8A`.

This is a navigation report only: the complete branch-heavy routine remains
unconverted, and no physical or display meaning is inferred from the math.
