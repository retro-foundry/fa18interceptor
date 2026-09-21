# Run014 post-F10 function-key replay

`captures/run014_post_f10_functions/` is a native Engine9000 replay from
`build/run003_post_f10_first/state.bin` at frame 3,899. It presses and releases
F1--F10 with 90-frame gaps, then repeats F10 after a further 180-frame gap.
It completes at frame 5,519 with end-video SHA-256
`514cc721ea110e05a5c386d60eaa812f272c662f68ad804b91df717d8c800003`.

The first F1 event is separately rebased and traced with no future input. It
hits `$C0F43A` on replay frame one, calls `$C1AD74` with raw `$50`, reaches the
byte-exact `$C1BD04` level routine, stores `$0C` at `$C45870`, and returns to
`$C0F45C` after 390 instructions. P-code is
`pcode/raw/run014_f1_keyboard_poll/`.

The full F1--F10 output table is in `analysis/function_key_levels.md`.
