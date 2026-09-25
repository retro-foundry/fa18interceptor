# Candidate geometry handoff inference

This focused semantic pass prioritizes an existing high-fan-out uncertainty:
whether the local plane-side helper at `$C27456` belongs to the `$C26EBE`
candidate-record scan or is an unrelated geometric utility.

## Result

It belongs to the scan's second stage. In the deterministic no-input attract
trace, `$C26EBE` receives the caller's relative `D2/D3/D4` triple, selects the
record at `$C46184 + $1C00` (`$C47D84`), and passes its broad three-component
bound test. The later `A3` value at `$C27410` is that same `$C47D84` record;
`$C27456` then consumes geometry-pointer streams selected by the follow-on
stage in `A4`.

This supplies semantic context for the candidate scan, the plane-side helper,
the `$C46184` record region, and callers that branch on the terminal condition.
It supports the neutral name **two-stage candidate geometry filter**. It does
not justify calling it collision, targeting, visibility, terrain, or a
specific game entity.

## Evidence boundary

- Focused replay trace: `build/no_key_c26ebe_candidate_scan/trace.jsonl`.
  It hits `$C26EBE` at frame 607, executes 3386 instructions, and ends at the
  requested caller return PC `$C26014`.
- The selected offset is `$1C00`; `A3` becomes `$C47D84` before the
  `$C27410 -> $C27456` calls.
- `$C4589F` changes `00 -> 01` and selected-record byte `$03` changes `00 ->
  `01` during the broad phase. The terminal return is nevertheless `D0=0,Z=1`,
  so broad acceptance and final caller condition are distinct state facts.
- The trace was replayed without future input after the breakpoint. It is a
  deterministic control-flow/dataflow oracle, not a player-action outcome.

The highest-value remaining semantic links are the success continuation of
`$C231A2` and the match route after `$C265E8`: each can explain multiple
record-bank update callers without inventing a game-world role.
