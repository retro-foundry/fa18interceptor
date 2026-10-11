# Raw-key `$0B` throttle-minus packet at `$C1AD74`

Classification: **behavioural command dispatch**, bound to the recorded
`K 45` throttle adjustment in sealed `run002`.

- State: replay through frame 3,843 before `K 45 45 16 1`.
- The isolated press reaches `$C1AD74` at replay frame 3 with raw byte `$0B`
  and returns through `$C0F45C` in 78 instructions.
- No future input occurs while stepping. P-code:
  `pcode/raw/run002_throttle_minus_key_dispatch/` (78 starts / 315 operations).

Raw `$0B` selects `$C1B5BC`, loads `$02` into `D2`, and joins `$C1B5C2`.
The observed route replaces the low two bits of `$C461E9` with that value,
then calls `$C1B602`, which clears `$C45870`, `$C45778`, and `$C4577C` before
the keyboard fallback. The byte-exact shared slices are
`source_amiga/observed/select_throttle_mode.asm` (`$C1B5B8-$C1B5DB`) and
`source_amiga/observed/reset_throttle_input_state.asm` (`$C1B602-$C1B615`).
The fields are not yet assigned a broader throttle semantics.
