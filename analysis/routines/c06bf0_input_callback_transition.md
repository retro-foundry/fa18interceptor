# `$C06BF0` input callback transition

Classification: **static-only structural/dataflow**. `$C1C2B8` transfers here
from selected keyboard-dispatch routes, but no available P-code export executes
the 20-byte target.

`source_amiga/observed/run_input_callback_transition.asm` is byte-exact for
`$C06BF0-$C06C03`. It calls `$C1748C`, calls the local `$C06C02` return stub,
calls `$C17456`, and returns. `$C17456` is the separately reconstructed
joystick callback registration entry; the role of `$C1748C` and the visible
meaning of this transition are not established.

The native `F` key was not dispatched from the available free-flight state, so
the static raw-key route is not promoted to a behavioral control claim.
