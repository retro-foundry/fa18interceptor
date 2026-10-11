# `$C1BB66` fire-request action tail

Classification: **structural with byte-exact source**. This 20-byte fragment
precedes the documented raw-`$44` weapon-cycle handler at `$C1BB7A`. It sets
bit 6 in `$C4599D`, writes `$05` to `$C45891`, then transfers to the common
queued-command routine at `$C1C23C`.

The source slice is
`source_amiga/observed/queue_fire_request_action.asm`. The request/action
names describe only these observed writes; a future isolated raw-key trace is
needed before assigning this tail to a specific fire control.
