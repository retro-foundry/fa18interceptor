# `$C31722` postflight status resolution

Classification: **partially runtime-observed dataflow**. The run024
frame-23000 continuation reaches both an entry path and a late path in this
slice; the intervening alternatives remain unobserved.

`source_amiga/observed/resolve_postflight_status_bits.asm` reproduces
`$C31722-$C3180B` (234 bytes). It terminates the selected table, compares
status bits in `$C4586D` against `$C4586E`, conditionally ORs event bits into
`$C45B54`, updates `$C45887`, handles one `$C45AE0` word case, and publishes
the status byte for the following scan.

The semantic role of these flags/events is unproven. Labels intentionally
describe only the observed bitwise state flow.
