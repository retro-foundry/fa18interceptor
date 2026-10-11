# `$C3201A`: selected-record `+$18` formatter with optional `FT`

Classification: **runtime-backed formatter dataflow**. The unit literal is
proven; the source record field and fixed screen label are not.

`source_amiga/observed/format_record_offset18_with_optional_ft.asm` is byte
exact for `$C3201A-$C32129`.

In its normal route, the stage reads selected-record long `+$18`, arithmetic
shifts it right ten bits, then multiplies the result by five. It filters this
against a sign-marked previous-value cache at `$C45900`, converts the retained
value via `$C25A08`, and draws six packed-decimal characters through `$C3271A`.
The normal geometry uses `$C31928`, `$18CE`, `$1E`, `D6=$FCA`, and `D5=4`.

When `$C45785` is set, the alternate submission writes literal bytes `F` and
`T` immediately after the six-character scratch region and redraws through the
same font renderer with parameters `$1CDA/$1A`, then `$F3A/$0C`. This proves
that one rendered variant carries an `FT` unit suffix. It does not by itself
prove that record `+$18` is the visible altitude field: the stage has mode and
override routes, and its final bitplane geometry still needs direct placement
evidence.
