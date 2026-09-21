# Stationary selected-fire state evidence

This note records only the bounded run004 command and immediate-consumer
evidence. It does not assign the two state values to weapon names internally.

| Reported phase | Raw input | Selected high nibble at `$C461E7` | Immediate command effect | Confirmed downstream result |
| --- | --- | ---: | --- | --- |
| First stationary firing phase | Space / `$40` | `$10` | `$C0833E` sets `$C4599A` bit 2 and `$C458C6` bit 3. | The bit remains set in a synthetic packet that contains the Space down event without its later key-up. In the full run it is clear immediately after the recorded Space release at frame 2,723; the clear implementation remains open. |
| Later stationary firing phase | Space / `$40` | `$30` | `$C0833E` sets `$C4599A` bit 2 and `$C457BA = 1`. | `$C22DB4 -> $C2374C -> $C22DC8` returns in 214 instructions, clears the flag, and initializes a record-like block. |

The player reports that the first phase fired guns and the later phase fired
missiles. That report aligns with the two bounded branches, but the binary
evidence currently establishes only the distinct selected-fire states and the
`$30` branch's immediate consumer.

Canonical P-code:

- `pcode/raw/run004_first_fire_dispatch/`
- `pcode/raw/run004_second_fire_dispatch/`
- `pcode/raw/run004_c2374c_second_fire_consumer/`

## `$10` held-input latch window

`build/run004_first_fire_latch_samples.json` samples a deliberately isolated
first-fire packet containing the Space down event only. `$C458C6` is zero at
replay frame 1, then `$0008` at every 30-frame sample from frame 31 through
frame 1,171. `$C4599A` and `$C457BA` are zero at those samples. This proves
only the behavior when the input remains held, because the full recording has
the corresponding Space up event at source frame 2,723.

The ten static CPU instruction references to `$C458C6` at `$C09F9A`,
`$C0A29A`, `$C120D4`, `$C120EA`, `$C121EA`, `$C121FA`, `$C1521A`, `$C1B6B2`,
`$C241F6`, and `$C2953C` were each breakpoint-tested against frames 1--1,200
of that same one-event replay; none was reached. The Engine9000 watchpoint
helper remains unsuitable for proving reads because it also missed the known
write. This is bounded reachability evidence for the held-input state, not
proof that no reader exists.
The machine-readable result is `analysis/run004_first_fire_c458c6_readers.json`,
produced by `scripts/check_replay_breakpoints.py`.

Full-run sampling in `analysis/run004_key_command_latches.json` shows
`$C458C6 = 8` after the long Space press at frame 2,596 and `$C458C6 = 0`
after its key-up at frame 2,723. The one-event release replay did not reach
`$C1AD74` or `$C083A0`. Its `$C0F3C4` input phase is bounded (1,221
instructions; P-code `pcode/raw/run004_space_release_input_phase/`), but the
word remains `$0008` through its `$C16EAE` child and through the following
parent update to `$C15DA2` (99,613 stepped instructions). The latter result is
`analysis/run004_space_release_parent_latch.json`; the later clear path is
still unbounded.

The input phase's direct `$C1715C` sampler is reconstructed as
`source_amiga/observed/sample_input_state.asm` (50 bytes). It forms a two-bit
state from `$C1839A` bit 6 and `$DFF016` bit 10; those hardware/state-bit roles
are deliberately not named beyond the observed operations.
