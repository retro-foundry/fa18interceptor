# Record matrix-update tail at `$C2D94E`

Classification: **behavioural transform handoff**.  This observed tail follows
the `$C2D408` post-matrix guards in no-key, attract, run001, and run003 update
packets.

The byte-exact code clears bit 2 of record byte `$03`, writes `D4-D6` to record
words `$66/$68/$6A`, then invokes `$C2E47A` with that triple and `A1 + $80`.
After restoring the parent record pointer, it advances to `$92`, forms three
values initialized to zero or `$7080 - D5/D6/D7`, and calls `$C2E514`.

This proves a record triple publication followed by two distinct matrix/transform
consumers.  Their coordinate convention and field ownership remain unassigned.

## run060 root-attitude instance

The sealed run060 frame-948 pre-turn trace observes this exact `MOVEM.W` at
`$C2D954` with `A1=$C46184`: it updates root `+$66/+68/+6A` before the
following `$C2E514` call writes root `+$92..+$A2`. On this bounded route the
first word advances `$7070 -> $7048 -> $7018 -> $6FE0 -> $6F88`; the other two
are zero. This establishes a live orientation-transform instance for the root
record without claiming a general record-owner meaning.
