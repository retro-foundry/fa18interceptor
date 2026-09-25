# `$C265E8`: 20-slot flagged-record decision scan

Classification: **runtime-backed flagged-slot threshold decision**. The helper
is a parent flight-update decision input. A set slot flag nominates a candidate;
it is not by itself a true result.

The byte-exact `$C265E8-$C26605` source scans 20 records in reverse slot
order, `19,18,...,0`. For slot `i` it tests bit 0 of byte
`$C45C72 + 64*i + $27`. If all 20 bits are clear, the helper sets `D0=0` and
returns. The loop uses `DBF`, so slot zero is tested before termination. There
are no persistent-memory writes on the no-match path. `$C151CE` independently
uses the same base and 64-byte slot stride to select a current record pointer.

For a flagged slot, `$C26606` first requires bit 5 of slot byte `$26`, unless
`$C45785` is nonzero. It derives two three-component absolute-difference
triples: first from the linked `$C46184 + 2*256*word[slot+$2E]` record and
then from the slot's own `+$00/+04/+08` triple after its signed `+$30/+32`
offset terms. Each triple is shifted right eight and passed to `$C1D974`.
The helper saves the first returned word and compares it with the second:
`D0=1` only when `first <= second` (signed); otherwise it returns zero. This
is a bounded candidate/threshold contract, not yet a proved distance,
collision, target, or visibility test.

The deterministic run060 frame-9545 packet
`build/run060_qualification_message_9545_trace/trace.jsonl` tests all 20
slots and returns `D0=0` at trace row 6761. At `$C0F0E6`, the parent tests
that longword, takes its zero branch to `$C0F108`, and calls `$C1CCBC`.
This is a concrete producer-to-consumer contract, not a claim that the flag
means an enemy, landing event, or qualification status. The ten distinct
P-code instruction starts for the same no-match scan appear in
`pcode/raw/attract_1800/`; repeated trace iterations establish the full
20-slot traversal. Source is split across
`source_amiga/observed/scan_c265e8_flagged_records.asm`,
`source_amiga/observed/evaluate_c26606_flagged_slot.asm`, and
`run_parent_flight_update.asm`. Match evidence:
`build/run060_c265e8_forced_match/trace.jsonl` and
`build/run060_c265e8_parent_match_route/trace.jsonl`; both use documented
breakpoint writes, so they prove control/dataflow rather than a natural run060
event.
