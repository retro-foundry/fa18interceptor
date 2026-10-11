# `$C212B0`: observed display primitive-submission loop

## Evidence

`build/no_key_c212b0_display_trace/` is a no-future-input packet that hits
`$C212B0` at frame 602 and returns to `$C1F944` after 2,552 instructions.
Canonical P-code is `pcode/raw/no_key_c212b0_display/`; the packet observes
236 RAM instruction starts and two direct call targets.

## Observed path

The routine takes pairs of words from the `A2` stream, copies two selected
records from `$C48390` into `$C4C592`, and calls `$C2EE4A` for each valid
pair. `$C2EE4A` performs signed projection/clipping arithmetic, then calls
`$C2FA7E`. Its observed child route reaches `$C2FB4E` and writes Amiga custom
chip blitter registers at `$DFF000` before submitting up to four active planes.

The packet repeats this cycle and returns an OR-accumulated word in `D0`.
It proves a display primitive-submission role for the entry, while leaving the
meaning of individual input records, scene objects, and the returned status
bits unassigned.

## Limits

This is a complete bounded invocation, not a reconstruction of the full static
body. Source is only promoted when its branches can be retained byte-exactly
and reviewed as a coherent routine.
