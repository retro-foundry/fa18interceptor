# `$C1518C`: record-scan renderer pass

Classification: **runtime-backed bounded record scan with renderer setup**.
The helper is the true branch of the `$C265E8` flagged-slot threshold path.

It initializes three local bytes, calls `$C2F490`, points local scan state at
`$C46201` and `$C461BF`, decrements the latter byte, then iterates record
entries rooted at `$C45C72` through the `$C151CE` slot-selection path. The
static loop limit is 20 entries. In the continuous forced-slot run060 trace it
visits ten consecutive 64-byte entries, `$C45C72` through `$C460F2`, before
the loop reaches `$C153DC`.

The observed finish route sees its local `-$1(A6)` clear, clears `$C461BF`,
and rewrites `$C46201` as `(old & $0F) | $50` before returning to `$C0F0F8`.
This proves a renderer-associated record sweep and shared finish-state update;
it does not identify the records as targets, aircraft, terrain, or AI.

Evidence: `build/run060_c1518c_continuous_forced_route/trace.jsonl` enters
from the replay-valid parent breakpoint, executes 490 instructions from the
parent interval, and returns from `$C1518C` at trace row 489. The slot flag
was deliberately forced upstream to take this otherwise uncovered route, so
the trace proves control/dataflow rather than a natural run060 event. Byte
source is split across `initialize_c1518c_record_scan.asm`,
`select_c151ce_record_scan_slot.asm`, and `return_c153dc_record_scan.asm`.
