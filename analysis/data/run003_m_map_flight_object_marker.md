# Run003 M-map object-marker candidate

The black map symbol is a three-segment vector emitted through `$C2B93E`:

| segment | endpoints |
| --- | --- |
| 1 | `(31,130) -> (31,130)` |
| 2 | `(25,131) -> (35,131)` |
| 3 | `(26,132) -> (34,132)` |

Its exact `$C2B91E` input is now byte-decoded as three
`(signed dy, signed dx, width-minus-one)` records followed by `$80`; see
[`c2b91e_marker_line_records.json`](c2b91e_marker_line_records.json).

Each reaches `$C2FA7E` with `A5=$C4C598`.  Immediately before the `$C2B93E`
line-list loop, the run003 M-map trace returns from `$C2ED6A` with
`A1=$C45BEA`.  `$C45BEA` is the known mutable display context for the
F/A-18-like `$C34C` flight-object face family. This ties the symbol to an
aircraft-associated mutable context, but does not identify the marked entity.
The cross-map pan comparison establishes a **map-attached object marker
candidate**, rather than a screen-fixed reticle.

It does not prove player ownership. The map may show another flight object, a
base, city, or selected object, and neither the symbol producer nor a
controlled identity trace has yet established that distinction.
