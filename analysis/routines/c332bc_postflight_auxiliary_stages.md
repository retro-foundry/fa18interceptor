# `$C332BC` postflight auxiliary stages

Classification: **static-only structural/dataflow**. The parent postflight
stage directly calls this entry, but it is not covered by current P-code.

`source_amiga/observed/run_postflight_auxiliary_stages.asm` reproduces
`$C332BC-$C332FB` (64 bytes). It writes `$000FFFFF` to `$C456E6`, applies two
byte guards, and invokes a fixed chain of three local, two external, and two
additional local stages before returning.

The stages' gameplay meaning remains unproven; names record only call order.
