# `$C1BD78` indexed command-mode selector

Classification: **structural with one scenario-backed menu mapping and
byte-exact source**. In zero context mode, this routine maps `D4` through
direct modes, special values, an indexed record byte, and an offset-validation
table before storing `$C458A6` and calling `$C3318E`.

It uses `$C45792` as an observed state marker on selected routes.  In the
frame-584 run024 menu scenario, frontend key `5` reaches this routine with
`D4=4`; `$C1BE46` changes it to 9 and `$C1BDEC` stores 9 at `$C458A6`.
The later menu callback appends qualification selector 105.  See
`analysis/run024_qualification_selection_chain.md`.  This proves that one
scenario mapping, not a general control interpretation of every `D4` value.

A controlled run029 F1 differential probe reaches this entry with `D4` low
byte 10. It takes the zero-word route at the live `$C1AB74` pointer-selected
state block and calls `$C3318E` without writing `$C458A6`; see
`analysis/routines/c3318e_indexed_command_side_effect.md`.  Therefore F1 must
not be represented as a known direct selector code or a numeric menu mapping.

The controlled selectable-label state also accepts a native F2 event at this
entry with raw `D0=$51` and `D4` low byte 11. Its `$C1AB74` offset-zero word
is likewise zero, so it takes the same `$C1BDF2 -> $C3318E` route without
writing `$C458A6`. A matching normal replay keeps the visible submenu image
unchanged while changing Slow RAM. Consequently the visible F2 label is not
evidence of an accepted Emergency Defense selection in that forced state; see
`analysis/run029_key6_all_conditionals_display_probe.md`.
