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
