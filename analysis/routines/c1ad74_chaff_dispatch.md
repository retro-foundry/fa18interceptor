# Raw-key `$33` chaff command at `$C1AD74`

Classification: **behavioural command dispatch**, bound to the documented `C`
control and sealed `run002` recording.

## Runtime packet

- State: `run002` replay through frame 5,526 before `K 99 99 16 1` (`C`).
- The isolated press reaches `$C1AD74` at replay frame 6 with raw byte `$33`
  and returns through `$C0F45C` in 105 instructions.
- No future input occurs while stepping.
- P-code: `pcode/raw/run002_chaff_key_dispatch/`, 105 starts / 445 operations.

## Observed command effects

Raw `$33` selects `$C1C172`. Its observed non-exhausted route sets bit 2 of
`$C4599B`, decrements `$C4584C`, writes `$1E` to `$C4584E`, writes 1 to
`$C4588B`, and calls `$C25704` with `D0 = $4028`. The static exhausted path
instead clears `$C4584C` and calls the same target with `$4029`.

`source_amiga/observed/dispatch_chaff_command.asm` is the byte-exact
`$C1C172-$C1C1AF` handler slice. The counter's initialisation and the command
effect target's subsystem ownership remain unassigned.
