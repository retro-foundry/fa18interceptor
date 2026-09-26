# run075 frame 559 `$C2FF48` transition packet

The canonical run075 transition reaches `$C2FF48` at engine frame 560. Entry
registers are `D0=$013F`, `D1=$FFFF8A13`, `D2=$0000`, `D3=$013F`, `D4=$0000`,
`D5=$00B3`, `D6=$020E`, `D7=$FFFF`, `A0=$C4590A`, `A1=$C45F24`,
`A2=$C47E9A`, `A3=$C47E90`, `A4=$C47E04`, and `A5=$C466A2`.

The leaf disables the blitter with `BLTCON1=$8400`, calls `$C301F6`, then
loads a 20 word record table at `$C4B390`. Each record contributes a pair of
coordinates. The loop compares the pair against the current bounds and keeps
the extrema while decrementing a 20 record counter. The traced packet starts
with records `(0,89)`, `(71,89)`, `(70,89)`, `(90,89)`, and continues through
the bounded pass.

This is a transition geometry preparation step. It does not directly submit a
static glyph. The following display operations are `$C2F688` at frame 561 and
`$C2F8B4` at frame 564.

Authority: `build/run075_c2ff48_558x_v2/trace.jsonl`, captured from the
canonical run075 restore and playback with a breakpoint at `$C2FF48`, armed at
frame 558. The breakpoint reached frame 560.
