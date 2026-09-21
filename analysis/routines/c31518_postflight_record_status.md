# `$C31518` postflight record-status classification

Classification: **partially runtime-observed dataflow**. The run024
frame-23000 continuation reaches a suffix path in this block; other branches
remain unobserved.

`source_amiga/observed/classify_postflight_record_status.asm` reproduces
`$C31518-$C315BF` (168 bytes). It evaluates record bits and bytes relative to
`a1`, sets bits 1-5 in `$C4586D` for selected cases, optionally clears/sets
record bit 6, and restores `d0/d1` from the earlier long values for the next
normalization phase.

The status-bit meanings are unproven; only the decoded predicates and writes
are named.
