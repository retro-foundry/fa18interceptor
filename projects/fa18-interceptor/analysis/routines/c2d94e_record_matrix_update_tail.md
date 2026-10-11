# Record matrix-update tail at `$C2D94E`

Classification: **behavioural transform handoff**.  This observed tail follows
the `$C2D408` post-matrix guards in no-key, attract, run001, and run003 update
packets.

The byte-exact code clears bit 2 of record byte `$03`, writes `D4-D6` to record
words `$66/$68/$6A`, then invokes `$C2E47A` with that triple and `A1 + $80`.
After restoring the parent record pointer, it advances to `$92`, forms three
values initialized to zero or `$7080 - D5/D6/D7`, and calls `$C2E514`.
The save/restore lists differ: saved `D4-D6` restore into `D5-D7`.
These inverse values therefore use the original `D4/D5/D6`, not the
incoming `D5/D6/D7`. Incoming `D7` does not influence either matrix.

This proves a record triple publication followed by two distinct matrix/transform
consumers.  Their coordinate convention and field ownership remain unassigned.

## Native contract

`port/record_matrix_update.c` ports the direct `$C2D94E-$C2D99A` publication
and ordering as `fa18_update_record_matrix`. Its caller owns the distinct
`$C2E47A` build matrix and `$C2E514` alternate attitude matrix computations;
the contract supplies those as required callbacks. It preserves the bit-2
clear, D4--D6 publication, and zero-or-`$7080 - original D4/D5/D6` angle derivation before the
second callback. `record_matrix_update_contract_test` covers the run075
`(D4,D5,D6,D7)=(0,$6FB8,0,0)` handoff and call order.
The earlier packet and its test misread this restore ordering; the
2026-10-05 correction is independently proved by both complete source entries,
including all actual matrix/lookup children. `$C2D954` preserves the control
bit; only `$C2D94E` clears it. New code uses the live native record owner in
[`native_record_orientation.md`](native_record_orientation.md).

## run003 second-lane publication

The sealed run003 comma hold reaches the publisher's `$C2D954` `MOVEM.W` at
absolute frame 5,360 with `A1=$C46184` and
`D4/D5/D6=$6D40/$60E8/$0000`. Thus it writes that triple to root
`+$66/+68/+6A`. The immediately preceding `$C2DEE0` return has the same three
words and its input was the matrix-side leaf's `$0000/$FFFC/$0000` working
triple. This is scenario-backed publication dataflow; it does not identify
the physical meaning of the second lane or each coupled output word.

## run060 root-attitude instance

The sealed run060 frame-948 pre-turn trace observes this exact `MOVEM.W` at
`$C2D954` with `A1=$C46184`: it updates root `+$66/+68/+6A` before the
following `$C2E514` call writes root `+$92..+$A2`. On this bounded route the
first word advances `$7070 -> $7048 -> $7018 -> $6FE0 -> $6F88`; the other two
are zero. This establishes a live orientation-transform instance for the root
record without claiming a general record-owner meaning.

Later in the same sealed replay, the frame-1754 publisher entry writes
`$7038/$0000/$0010` to the same root triple. This occurs in the update window
where the independent `$C1B410` packet changes third control lane `+$2A` from
zero to −3. The non-zero third angle is direct publication evidence; the
intermediate transform that would establish `+$2A` as its sole cause remains
to be bounded.
