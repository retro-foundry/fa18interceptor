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

The C port is `record_matrix_update.c` with `glue_record_matrix_update.c`.
Its focused proof over demo01, qual_carrier_success and qual_fail_crashes
matched 3,975 completed shadow calls and 4,877 sandbox calls, with zero
mismatches. The shadow pass leaves interrupted calls incomplete; sandbox
compared every recorded call.

The post-matrix path then proves a guard sequence: byte `$05(a1)` equals `$0A`,
bit 6 of `$03(a1)` is set, `$C45784` is nonzero, bit 7 of `$03(a1)` is clear,
and the record remains class `$10`.  It halves word `$66`, rejects values below
`$1C20` into raw code, and otherwise computes a threshold from `$66` and `$22`.
When that threshold is below `$6E(a1)`, it clears word `$26(a1)`; the following
path clears bit 2 of `$20(a1)`.  These are field/branch contracts only.
