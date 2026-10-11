# `apply_function_key_throttle_level` at `$C1BD04` (Hunk 5 +`$5F5C`)

Classification: **behavioural**. The sealed run003 tuple `K 291 0 16 1`
replays as raw Amiga `$59`, enters the `$50-$59` function-key range, and
reaches this tail with level ten in `D4`. The exact restored-state dispatcher
trace returns in 388 instructions; P-code is
`pcode/raw/run003_f10_exact_keyboard_poll/`.

The generic path converts level `n` to `12n`, with a one-unit adjustment at
the top threshold, stores the byte at `$C45870`, and, when `$C457D8` is active,
publishes the value shifted left three to `$C45778` and `$C4577C`, capped at
`$03C0`. For observed level ten it stores `$79`. The level-one special route
is static in this slice and was not taken by the recorded state.

## run060 turn-window exclusion

A direct breakpoint on `$C1BD04`, armed before replay frame 900 and replayed
through frame 970 of sealed `run060`, is not reached. This covers the
frame-939 recorded joystick event and the first root angle publication at
frame 949. Therefore the generic function-key publisher is not the observed
route that produces the run060 turn-window control state, despite using the
same `$C45778/$C4577C` pair and `$03C0` cap.
