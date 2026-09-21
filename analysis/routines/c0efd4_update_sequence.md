# `$C0EFD4` observed update sequence (Hunk 0 +`$1124`)

Classification: **structural**. `$C0EFD4` is a verified Hunk-0 routine above
the bounded input phase, but no return-to-main-loop claim has been made.

## Capped no-input packet

The `local/start_demo.e9k` replay has no event after menu selection. A
breakpoint at `$C0EFD4`, armed at frame 600, hit at frame 607. The trace ran
10,000 instructions without a recorded input event after the breakpoint and
was deliberately capped before the requested return `$C15DA2`.

`build/no_key_parent_update_capped/trace_summary.json` explicitly records
`termination: "max_instructions"`. This packet is useful for its prefix only;
it is not imported as a complete function or P-code authority.

## Observed prefix

```text
$C0EFD4  create local word from $C458DA
$C0EFE0  bsr $C0F3C4       ; bounded pending-key phase
$C0EFE4  jsr $C0F5F8
$C0EFEA  jsr $C11B44
$C0EFF0  test $C45795
$C0EFFA  write $0008 to $C45AD4; jsr $C12098
$C0F008  write $0010 to $C45AD4; jsr $C25B1E
$C0F016  jsr $C1C63E
```

In this packet `$C25B1E` immediately returns. Execution then enters `$C1C63E`
(verified Hunk 8) at trace index 409 and has not returned by index 9,999. The marker
writes at `$C45AD4` are literal instruction effects whose meaning is unknown;
they only provide stable boundaries for future short traces. Earlier
in-flight P-code at frame 1800 independently observes later writes through
`$00D0` in this same static sequence.

The next dynamic packet should start at `$C1C63E` and use a cap, preserving its
termination reason rather than treating a long run as a completed routine.

The static prefix through that `$C1C63E` call is now byte-exact source at
`source_amiga/observed/run_parent_update_prefix.asm`
(`$C0EFD4-$C0F01B`, 72 bytes). The marker values are retained as measured
stage boundaries rather than assigned subsystem semantics.

The immediately following static middle slice is also byte-exact at
`source_amiga/observed/run_parent_update_middle.asm`
(`$C0F01C-$C0F08F`, 116 bytes). It orders the bounded matrix pipeline and
angle-octant call among adjacent update calls without assigning every helper a
subsystem role.

The following conditional flight-update slice is byte-exact at
`source_amiga/observed/run_parent_flight_update.asm`
(`$C0F090-$C0F123`, 148 bytes). Its `$C45AD4` marker values distinguish the
observed branch paths but remain boundaries rather than subsystem names.

The subsequent indexed-record and activity-gated setup range is byte-exact at
`source_amiga/observed/run_parent_postflight_setup.asm`
(`$C0F124-$C0F1E1`, 190 bytes). It selects `$C46184 + ($C458DC << 9)` and
preserves its local branch ordering without assigning record ownership.

The following activity-gated stage block is byte-exact at
`source_amiga/observed/run_parent_activity_stages.asm`
(`$C0F1E2-$C0F2A7`, 198 bytes). It retains the exact helper-call ordering and
the signed activity-byte decrement without a broader subsystem assignment.

The tail is byte-exact at
`source_amiga/observed/run_parent_update_tail.asm`
(`$C0F2A8-$C0F3C3`, 284 bytes). Together with the five preceding adjacent
source slices, it covers the full static `$C0EFD4-$C0F3C3` routine (1,008
bytes). The prior 10,000-instruction dynamic cap still applies to a complete
execution claim for this parent boundary.

Later in the same static sequence, marker `$0020` at `$C0F01C` precedes the
observed edge `$C0F02A -> $C2D99C -> $C0F030`. Its bounded matrix-pipeline
packet is documented in [`c2d9ba_matrix_pipeline.md`](c2d9ba_matrix_pipeline.md).
