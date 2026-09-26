# Run003 indexed record selector at `$C13D84`

Classification: **behavioural packet with byte-exact selector source**. In
sealed run003 frame 6,000, `$C25D7E` calls `$C13D84` and it returns to
`$C25D84` after 370 instructions with empty future playback. Canonical P-code
is `pcode/raw/run003_6000_c13d84/` (370 RAM instruction starts, 2,216
operations, two observed call targets).

The packet extends the existing byte-exact selector prefix at
`source_amiga/observed/prepare_indexed_control_record_context.asm`. It is the
normal-route evidence for publishing the current indexed control-record
context; downstream record ownership remains unassigned.

Its only observed direct game call is `$C1484C -> $C26428`, the independently
bounded 62-instruction indexed-record updater documented in
`c26428_indexed_record_update.md`. This closes the direct selector-to-updater
edge for this route.

## Run060 scheduler-gate input

An aligned run060 global-frame-9,200 parent-update trace reaches the same tail
while `$C18210=$C46184`. At `$C1486A`, it writes `$0000` to `$C46184+$6E`,
changing the prior `$0007`. The live arithmetic is the byte-exact tail:
`word(C18210+$78) + word(pointer at -$20(A6))`, with the resulting `D0=0`.
The write is retained in `build/run060_frame09200_record_6e_writer_trace/`.
This supplies the final zero consumed by the later `$C0A2F0` scheduler gate;
it does not assign a physical unit or semantic name to either source word.
