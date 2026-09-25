# `$C265E8`: 20-slot flagged-record decision scan

Classification: **runtime-backed behavioral scan on the no-match path**.
The helper is a parent flight-update decision input; the meaning of the flag
and the match continuation are still unknown.

The byte-exact `$C265E8-$C26605` source scans 20 records in reverse slot
order, `19,18,...,0`. For slot `i` it tests bit 0 of byte
`$C45C72 + 64*i + $27`. A set bit branches to `$C26606` before the no-match
return; that target is outside this captured path. If all 20 bits are clear,
the helper sets `D0=0` and returns. The loop uses `DBF`, so slot zero is
tested before termination. There are no persistent-memory writes on the
observed path. `$C151CE` independently uses the same base and 64-byte slot
stride to select a current record pointer.

The deterministic run060 frame-9545 packet
`build/run060_qualification_message_9545_trace/trace.jsonl` tests all 20
slots and returns `D0=0` at trace row 6761. At `$C0F0E6`, the parent tests
that longword, takes its zero branch to `$C0F108`, and calls `$C1CCBC`.
This is a concrete producer-to-consumer contract, not a claim that the flag
means an enemy, landing event, or qualification status. The ten distinct
P-code instruction starts for the same no-match scan appear in
`pcode/raw/attract_1800/`; repeated trace iterations establish the full
20-slot traversal. Source: `source_amiga/observed/scan_c265e8_flagged_records.asm`
and `run_parent_flight_update.asm`.
