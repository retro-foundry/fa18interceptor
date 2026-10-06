# Joystick callback registration at `$C17456` (Hunk 0 +`$95A6`)

Classification: **vertical-blank server registration**. This 54-byte
function is a byte-exact reconstruction in
`source_amiga/observed/joystick_callback_registration.asm`.

It initializes descriptor fields at `$C1ABF0`, writes callback entry
`$C1718E` to `$C1AC02`, and calls `$C53B00` with descriptor kind 5 and the
descriptor pointer. The `$C1718E` target is independently live-validated by
the sealed `run001` JOY0DAT callback trace.

The controlled native-Delete trace reaches this routine from `$C06BF0` after
calling adjacent `$C1748C` with the same descriptor. It returns to `$C0F45C`
as part of the trace's 104-instruction bounded interval.

Cadence follow-up: C53B00 loads ExecBase and calls its -$A8 AddIntServer
vector with the caller's number 5 and descriptor pointer. This installs a
vertical-blank server, rather than a per-game-update callback. The reference
machine raises interrupt bit 5 at each frame start (`port/machine/machine.c`).
The [AmigaOS interrupt documentation](https://wiki.amigaos.net/wiki/Exec_Interrupts)
describes AddIntServer's vertical-blank server chain. Native runtime evidence
and the exact source-tail comparisons are recorded in
`../native_viewport_cadence_milestone.md`.
