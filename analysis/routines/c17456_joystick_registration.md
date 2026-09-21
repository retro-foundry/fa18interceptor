# Joystick callback registration at `$C17456` (Hunk 0 +`$95A6`)

Classification: **structural static evidence**. This 54-byte function is a
byte-exact reconstruction in
`source_amiga/observed/joystick_callback_registration.asm`.

It initializes descriptor fields at `$C1ABF0`, writes callback entry
`$C1718E` to `$C1AC02`, and calls `$C53B00` with descriptor kind 5 and the
descriptor pointer. The `$C1718E` target is independently live-validated by
the sealed `run001` JOY0DAT callback trace.

No runtime invocation of `$C17456` has been captured in the current scenarios,
so this establishes its direct static registration behavior only. The service
at `$C53B00` and descriptor kind meaning remain unassigned.
