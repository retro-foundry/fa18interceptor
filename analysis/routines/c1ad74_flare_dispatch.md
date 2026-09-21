# Raw-key `$23` flare command at `$C1AD74`

Classification: **behavioural command dispatch**, bound to the documented `F`
control and sealed `run002` recording.

- State: replay through frame 5,603 before `K 102 102 16 1` (`F`).
- The isolated press reaches `$C1AD74` at replay frame 3 with raw byte `$23`
  and returns through `$C0F45C` in 105 instructions.
- No future input occurs while stepping. P-code:
  `pcode/raw/run002_flare_key_dispatch/` (105 starts / 443 operations).

Raw `$23` selects `$C1C0E0`. The observed non-exhausted route sets bit 1 in
`$C4599B`, decrements `$C4584D`, writes `$1E` at `$C4584F`, latches `$02` at
`$C4588B`, and calls `$C25704` with `D0 = $4026`. The contiguous handler is
guarded by `D6 = 0`; its byte-exact command slice is
`source_amiga/observed/dispatch_flare_command.asm` (`$C1C0E4-$C1C121`).
