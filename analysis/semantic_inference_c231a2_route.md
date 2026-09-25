# `$C231A2` paired-record route result

This pass resolves the previous high-fan-out gap in the paired-record guard:
the meaning of its continuation after all ordinary eligibility checks pass.

## Contract

`$C231A2` has a zero ordinary route and a nonzero special route. The ordinary
route includes rejection and a fully passed record whose terminal class is not
`$20` or `$30`; it returns `D0=0,Z=1`. The special route is selected by either
a nonzero one-shot byte `$C457BC`, or after all checks by a terminal class
`$20/$30` in `A2+$63`. It clears `$C457BC` and returns `D0=1,Z=0`.

At the three paired-bank callers, zero takes the `BEQ` skip. Nonzero falls
through to `$C2377E`, then reaches their shared `$C23A7E` record-update stage.
This proves a route-selection contract, not a game-world entity type or an AI,
weapon, collision, or targeting role.

## Evidence

- Static bytes `$C231F0-$C23227`, reconstructed in
  `source_amiga/observed/return_c231a2_route_result.asm`.
- Existing no-key runtime trace proves a linked-record rejection and zero
  result without writes.
- `build/run060_c231a2_nonzero_return/trace.jsonl` uses a documented one-byte
  breakpoint write (`$C457BC=1`) to exercise the otherwise uncovered special
  return: it clears that byte and returns `D0=1,Z=0` in ten instructions.
- `build/run060_c231a2_caller_nonzero_continuation/trace.jsonl` proves the
  caller's `BEQ` is not taken and that it enters `$C2377E` before `$C23A7E`.

The breakpoint mutation is a control-flow oracle, not evidence that run060
naturally reaches this special route.
