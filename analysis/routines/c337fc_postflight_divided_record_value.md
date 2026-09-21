# `$C337FC` postflight divided record value

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It reads `$68` from the record selected by `$C46184 + $C458DE`, arithmetic
shifts it right three bits, and processes the resulting long through
`$C25A08`, `$C259C2`, and packed storage `$C45B22`. Values at or above `$360`
are replaced with zero after a four-bit shift.

The resulting value is adjusted by a conditional `-$64`, divided by 5,
negated, offset by `$9F`, written to `$C4598C`, then used to index from
`$C33A16` before invoking `$C33F54`. `$C338AA` is the next boundary. The
record fields, packed representation, and helper effects remain unproven.
