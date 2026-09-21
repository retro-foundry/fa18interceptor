# Raw-key `$0C` throttle-plus packet at `$C1AD74`

Classification: **behavioural command dispatch**, bound to the recorded
`K 61` (`=`) throttle adjustment in sealed `run002`.

- State: replay through frame 3,909 before `K 61 61 16 1`.
- The isolated press reaches `$C1AD74` at replay frame 2 with raw byte `$0C`
  and returns through `$C0F45C` in 76 instructions.
- No future input occurs while stepping. P-code:
  `pcode/raw/run002_throttle_plus_key_dispatch/` (76 starts / 307 operations).

Raw `$0C` selects `$C1B5B8`, loads `$01` into `D2`, then uses the same
byte-exact selector and reset helper as the raw `$0B` route. It replaces the
low two bits of `$C461E9` with `$01` and clears `$C45870`, `$C45778`, and
`$C4577C`. The fields have no broader assigned throttle semantics.
