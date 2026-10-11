# Matrix-side record arguments

The active path is runner -> machine frame -> recomp hook -> C25B66 step ->
C2D408 -> `update_record_nonclass_matrix()` -> `update_matrix_side_record()`.
That owner selects its record and still publishes the original CURRENT_RECORD
pointer. Six nested C helpers now take that selected record as a C argument:
`update_record_5a`, `update_record_56_from_66`, `steer_record_56`,
`steer_record_5a`, `ease_record_58` and `nudge_outside_dead_zone`.

This removes their dependence on repeated guest CURRENT_RECORD lookups. The
known C owners are their only retained callers. The existing atomic parent has
no intervening interrupt; its alert/audio chain does not write CURRENT_RECORD.
Math, record fields and original pointer publication are unchanged. Field
addresses remain `gaddr`; this is not native game-state ownership or memory
cutover. The parent's fixed timing and event-boundary debt remain open.

Validation uses 800-frame isolated C2D408 probes rather than full replays:

| Input | Parent calls | Direct side calls | Guest pointer reads removed |
| --- | ---: | ---: | ---: |
| demo01 | 70 | 56 | 56 |
| qual_carrier_success | 15 | 0 | 0 |
| qual_fail_crashes | 76 | 53 | 106 |

All RGB/index/RAM bytes and stdout/stderr match the prior runner. Instruction,
chipset, service, port and direct-call counts are identical. Only port reads
on guest page C18000 decrease; every other page/origin access is unchanged.
Source shadow: demo 65 matches/five incomplete, crashes 66 matches/ten
incomplete, no mismatches/hardware exclusions. Sandbox: 70/76 matches, no
incomplete comparisons/mismatches. GNU/MSVC build both active runners; all
twelve CTests and GNU profiling invisibility pass. Hashes and counters are in
`emulation_removal_record_arguments_batch.json`.

No full suite was repeated. Reuse its **38.4011% raw CPU minimum**, with **zero
observed instruction delta** in these probes. Accepted CPU share remains
unavailable because combined parity still fails. Memory/chipset/boot cutover
**0%**, deletion **0/4**. There are now 8,946 guest access sites; six pointer
lookup sites were removed, **zero** state sites converted.
