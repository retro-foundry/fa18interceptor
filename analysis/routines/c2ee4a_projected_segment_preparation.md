# Projected-segment preparation at `$C2EE4A`

Classification: **renderer helper, partially reconstructed**.  The exact
observed portions are `$C2EE4A-$C2EE67` and `$C2EE94-$C2EEB3`, in
`source_amiga/observed/prepare_projected_segment_entry.asm` and
`source_amiga/observed/prepare_projected_segment_negative_d3.asm`.

The prefix initializes a one-word local counter, reads three words from the
six-word input workspace at `$C4C592`, and sets `$C4B390` as the projected
output workspace.  It performs signed comparisons of the input values and
branches to raw clipping/intersection continuations.  The observed fall-through
calls `$C2F0F4`; a zero result branches to the existing raw rejection path at
`$C2F02E`.

This is the shared preparation entry invoked by the observed attract, external
view, and Golden Gate record-submission paths.  The surviving routine body is
intentionally not named as a particular object or coordinate convention: the
prefix only proves workspace ownership and signed clipping/intersection control
flow.

## Observed complete-path evidence

Within the complete frame-602 `$C212B0` packet, `$C2EE4A` is invoked ten
times. Each invocation returns to `$C2130A` after 232–234 instructions; its
canonical P-code is part of `pcode/raw/no_key_c212b0_display/`.

Those paths load two record triples from `$C4C592`, perform signed
multiply/divide projection arithmetic with `$A0`, `$5A`, `$140`, and `$B4`,
write transformed words to `$C4B390`, then invoke `$C2FA7E` once. All ten
reach the `$DFF000` blitter-programming path and return `D0 = 1`. This supports
the bounded description “project projected segment and submit it”; it does not
assign meanings to the source coordinates, object ownership, or every raw
clipping branch.
