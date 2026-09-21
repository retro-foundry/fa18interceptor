# Raw-key `$24` gear command at `$C1AD74`

Classification: **behavioural command dispatch**. The control identity comes
from the documented `G` mapping and sealed `run002`; the memory effects are
runtime evidence.

## Runtime packet

- State: `run002` through frame 4,302, immediately before its recorded
  `K 103 103 16 1` (`G`) press at frame 4,303.
- The isolated press reaches `$C1AD74` with raw key byte `$24` and returns
  through `$C0F45C` after 85 instructions.
- No future input occurs while stepping.
- P-code: `pcode/raw/run002_gear_key_dispatch/`, 85 starts / 341 operations.

## Observed command effects

Raw `$24` selects `$C1BC12`. The complete observed route sets bit 0 of
`$C4599A`, tests bit 7 of `$C46187` without taking that branch, and toggles
bit 7 of `$C46200` with `BCHG`. It then reaches `$C1C23C`, which sets
`$C457A3` to 1 and clears `$C45878-$C4587A` before returning.

`$C46200` bit 7 is the measured toggle state for the documented gear command
in this scenario. Its broader record ownership and the `$C4599A` request bit
need independent consumer evidence. The byte-exact local handler is
`source_amiga/observed/dispatch_gear_command.asm` (`$C1BC12-$C1BC4F`).
