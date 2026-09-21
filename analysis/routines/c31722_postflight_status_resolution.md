# `$C31722` postflight status resolution

Classification: **static-only dataflow**. This slice follows the larger
postflight loop and falls through to the terminal record scan.

`source_amiga/observed/resolve_postflight_status_bits.asm` reproduces
`$C31722-$C3180B` (234 bytes). It terminates the selected table, compares
status bits in `$C4586D` against `$C4586E`, conditionally ORs event bits into
`$C45B54`, updates `$C45887`, handles one `$C45AE0` word case, and publishes
the status byte for the following scan.

The semantic role of these flags/events is unproven. Labels intentionally
describe only the observed bitwise state flow.
