# Raw-key `$12` eject command at `$C1AD74`

Classification: **behavioural command dispatch**. The control identity is
anchored by the sealed `run002` Shift+E recording and the documented eject
mapping. This packet establishes command state, not every later animation or
flight-state consequence.

## Runtime packet

- State: `run002` replay through frame 5,874, after its Shift press and before
  its `E` press.
- The isolated `K 101 69 17 1` event reaches `$C1AD74` at replay frame 2 with
  raw key byte `$12`, then returns through `$C0F45C` after 92 instructions.
- No future input is present while stepping.
- P-code: `pcode/raw/run002_eject_key_dispatch/`, 86 starts / 337 operations.

## Observed command effects

Raw `$12` selects `$C1B126`. The observed route tests `$C457AE`, writes
`$08` to `$C45842`, and calls `$C1C214`, which sets `$C457AB` to 1 on this
path. On return it sets bit 5 of `$C4599A`, ORs `$0A` into `$C46200`, and
reaches the shared command fallback at `$C1C23C`.

`source_amiga/observed/dispatch_eject_command.asm` is the byte-exact
`$C1B126-$C1B15D` command slice. The field names describe the observed command
relationship; the ultimate consumer of the request flags remains unassigned.
