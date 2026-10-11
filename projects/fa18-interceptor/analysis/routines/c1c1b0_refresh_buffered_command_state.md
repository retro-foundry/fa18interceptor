# `$C1C1B0` buffered command-state fragment

Classification: **structural with byte-exact source**. `$C1C1B0` is a direct
branch to the common queue routine. The following entry at `$C1C1B4` tests the
longword `$C4FDBC`; when nonzero, it copies `$C4FDA4` to `$C4FDBC` and
`$C4FDB0` to `$C4FDB8`, advances those destinations by 1 and 8, writes `$03`
to `$C4584B` and `$01` to `$C4582A`, then queues the command.

The source is `source_amiga/observed/refresh_buffered_command_state.asm`.
The `$C4FDxx` fields and this fragment's game-control meaning require a
bounded producer trace before they receive semantic names.
