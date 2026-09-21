# Current-record class dispatch at `$C2D408`

Classification: **behavioural update dispatcher**.  `$C25D9E` calls this stage
with `A1` holding the current record; it is repeatedly observed in attract and
record-update replay packets.

The exact observed entry masks byte `$62(a1)` with `$F0` and branches on class
`$30`.  The observed non-`$30` path initializes `D0/D2/D4` to zero and, when
the active selector is zero and `$C457AE` is zero, enters the default path at
`$C2D5FC`.

That default path recognizes class `$10`; it optionally calls `$C1342C` when
bit `$40` of `$C458CC` is set, then loads record words `$56/$58/$5A` into
`D0/D2/D4`.  Its postload continuation combines `$C457AF/$C457AE`, retests
class `$10`, and conditionally loads `$6A(a1)` into `D7` before calling
`$C2DD4E`.  The other class routes remain raw.

All observed routes then use the record substructure at `$80(a1)` as `A4` for
the signed matrix-product stage `$C2DEE0`.  On return, the dispatcher clears
bit 4 of byte `$03(a1)` and branches on its prior state.  This is a direct
record-triple-to-matrix handoff, not a claim about the substructure's object
type.
