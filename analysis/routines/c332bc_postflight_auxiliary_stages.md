# `$C332BC` postflight auxiliary stages

Classification: **partially runtime-observed structural/dataflow**. The
run024 frame-23000 continuation executes `$C332BC-$C332EC` and takes the
guarded re-entry; its normal multi-stage chain is not covered by that trace.

`source_amiga/observed/run_postflight_auxiliary_stages.asm` reproduces
`$C332BC-$C332FB` (64 bytes). It writes `$000FFFFF` to `$C456E6`, applies two
byte guards, and invokes a fixed chain of three local, two external, and two
additional local stages before returning.

The stages' gameplay meaning remains unproven; names record only call order.
