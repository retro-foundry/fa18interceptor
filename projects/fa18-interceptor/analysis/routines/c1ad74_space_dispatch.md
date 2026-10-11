# Raw-key `$40` Space command at `$C1AD74`

Classification: **behavioural command dispatch**, bound to the documented
Space weapon-fire control and sealed `run002` recording.

## Runtime packet

- State: `run002` replay through frame 5,297 before `K 32 32 16 1` (Space).
- The isolated press reaches `$C1AD74` at replay frame 1 with raw byte `$40`
  and returns through `$C0F45C` in 67 instructions.
- No future input occurs while stepping.
- P-code: `pcode/raw/run002_space_key_dispatch/`, 67 starts / 268 operations.

## Observed command effect

Raw `$40` selects `$C1B21C`. With the measured `D6 = 0`, it calls `$C0833E`
then joins the keyboard dispatcher fallback. The observed route sets bit 2 of
`$C4599A`, masks `$C461E7` to its high nibble, and writes 1 to `$C457BA` when
that nibble is neither zero nor `$10`.

`source_amiga/observed/dispatch_space_command_effect.asm` is the byte-exact
contiguous `$C0833E-$C08393` helper. Its other branches are static bytes in the
same helper; the semantics of its mode and latch fields remain unassigned.
