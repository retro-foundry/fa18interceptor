# Run003 M-map flight-object marker candidate

The black map symbol is a three-segment vector emitted through `$C2B93E`:

| segment | endpoints |
| --- | --- |
| 1 | `(31,130) -> (31,130)` |
| 2 | `(25,131) -> (35,131)` |
| 3 | `(26,132) -> (34,132)` |

Each reaches `$C2FA7E` with `A5=$C4C598`.  Immediately before the `$C2B93E`
line-list loop, the run003 M-map trace returns from `$C2ED6A` with
`A1=$C45BEA`.  `$C45BEA` is the known mutable display context for the
F/A-18-like `$C34C` flight-object face family.  This promotes the symbol from
an unclassified mark to a **scenario-backed flight-object marker candidate**.

It does not prove player ownership.  The map may show another flight object,
and neither the symbol producer nor a controlled player/object identity trace
has yet established that distinction.
