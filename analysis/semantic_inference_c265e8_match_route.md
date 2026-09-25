# `$C265E8` flagged-slot match route

This pass resolves the highest-priority missing path of the 20-slot scan.

## Result

The scan's bit-0 flag nominates a slot, then `$C26606` performs a second,
two-scalar threshold decision. It returns one only if the nominated slot clears
its companion-state gate and the first scalar is not greater than the second.
The parent consumes one by writing stage marker `$A4` and invoking `$C1518C`;
the static continuation then writes `$A8` and invokes `$C1CCBC`.

This is enough to rename the contract to a flagged-slot threshold decision. It
does not identify the slots or scalars as objects, targets, collision distance,
or AI.

## Evidence boundary

- Byte-exact match code `$C26606-$C266AB` is reconstructed in
  `source_amiga/observed/evaluate_c26606_flagged_slot.asm`.
- `build/run060_c265e8_forced_match/trace.jsonl` forces only slot 19 bit 0 and
  companion bit 5 at the breakpoint. It takes the match path, calls `$C1D974`
  twice, compares `$20BC <= $4000`, and returns one.
- `build/run060_c265e8_parent_match_route/trace.jsonl` starts at the parent
  call, observes `TST.L D0` / non-taken `BEQ`, marker `$A4`, and entry into
  `$C1518C`.

The writes deliberately exercise an uncovered branch. They are a bounded
control-flow oracle, not natural qualification behavior in run060.
