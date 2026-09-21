# `$C32AA4` postflight transform-submit entry

Classification: **runtime-observed dataflow**. The run024 frame-23000
continuation executes this 16-byte entry from the postflight transform-helper
chain.

`source_amiga/observed/initialize_postflight_transform_submit.asm` is
byte-exact for `$C32AA4-$C32AB3`. It clears `D4`, loads lane offset `$C45986`
into `D6` and long offset `$C45918` into `D7`, then joins `$C32AD0`.

The joined body formats `$C45B22` into bytes at `A0`, applies the loaded offset
to `A4`, and consumes word pairs through `A1`. Thus the `$C33258` table is a
transform-submit input table, though its units and rendered object remain
unproven.
