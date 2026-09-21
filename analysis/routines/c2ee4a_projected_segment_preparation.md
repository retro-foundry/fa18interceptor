# `$C2EE4A`: observed projected-segment preparation

## Evidence

Within the complete frame-602 `$C212B0` packet, `$C2EE4A` is invoked ten
times. Each invocation returns to `$C2130A` after 232–234 instructions. Its
canonical P-code is part of `pcode/raw/no_key_c212b0_display/`.

## Observed contract

The observed path loads two record triples from `$C4C592`, performs signed
multiply/divide projection arithmetic using the constants `$A0`, `$5A`,
`$140`, and `$B4`, and writes transformed words to `$C4B390`. It then invokes
`$C2FA7E` once. All ten invocations reach the `$DFF000` blitter-programming
path beneath that call and return `D0 = 1` to the caller.

This supports the name **projected-segment preparation**: it transforms one
record pair into a bounded display-space segment and submits the result to the
existing blitter-line path. The meanings of input coordinates and every
unobserved clipping branch remain unassigned.
