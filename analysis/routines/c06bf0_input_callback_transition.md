# `$C06BF0` input callback transition

Classification: **bounded behavioral callback transition**. `$C1C2B8`
transfers here from selected keyboard-dispatch routes. A controlled native
Delete event reaches the target and returns to the keyboard poll in 104
instructions.

`source_amiga/observed/run_input_callback_transition.asm` is byte-exact for
`$C06BF0-$C06C03`. It calls `$C1748C`, calls the local `$C06C02` return stub,
calls `$C17456`, and returns. `$C17456` is the separately reconstructed
joystick callback registration entry; the role of `$C1748C` and the visible
meaning of this transition are not established.

## Native Delete bounded trace

`build/run003_delete_key_variant.e9k` changes the six recorded frontend `H`
events in sealed run003 to native Delete (key/character 127), without changing
other replay rows. With `$C06BF0` armed at frame 1,900, the first replacement
press reaches it at frame 1,965 with `D0 = $00000046`. The retained bounded
trace (`build/run003_delete_c06bf0_trace/`) executes the two calls and returns
to `$C0F45C` after 104 instructions. Later recorded inputs were deliberately
not delivered while single-stepping, as stated in its trace summary.

This proves the native-Delete-to-raw-`$46` route in this free-flight state and
the transition's bounded call/return behavior. It does not identify a visible
Delete control effect or the semantic purpose of either child routine.

## Rejected flight-state frontend-F probe

`build/run003_f_key_variant.e9k` replaces all six frontend `H` events in the
sealed run003 recording with frontend key/character 70 (`F`) while preserving
every other event.  A `$C06BF0` breakpoint armed at frame 1,900 did not hit
through frame 2,200 (`build/run003_f_key_c06bf0_probe/`).  This independently
rejects frontend `F` as a direct route to the raw `$46` dispatcher case during
that free-flight replay; it does not identify the actual raw input producer.

## Rejected top-level frontend-F probe

`build/run029_keyf_variant.e9k` changes only the frame-266/269 digit-5 events
in sealed run029 to frontend key/character 70 (`F`), then
`build/run029_keyf_prefix.e9k` retains no later input. Breakpoints armed from
frame 250 at both `$C1AD74` and `$C06BF0` did not hit through frame 400.
Consequently this frontend event does not establish that the dispatcher's raw
`$46` comparison is a normal top-level `F` binding, and it supplies no live
contract for this transition. The entry remains static-only.
