# Complete menu/context completion owners (2026-10-03)

C10A24/C10C08/C10C68/C10AB2/C10AE6/C10B1E/C10CFE/C10D8A/C10DAE/
C11A26/C11A50/C09192/C16D04/C25070 have complete readable C, normal adapters
and source timing. Three translated and seven original source-only entries
are added; four existing adapters replace fixed-cycle implementations.
Coverage is 474/624 translated entries plus 36 source-only entries: 510 rows
and 314 timed entries (278 translated plus 36 source-only). This remains a
whole-game reconstruction in progress, rather than a recording-only target.

## Original behavior

`python tools/recomp/audit_menu_context_finish_source.py` seals 441 unique /
zero shared original boundaries. Every new entry has an original child call
or installed callback. C25070 also owns its negative-return arm at C2506C;
that arm is included in its registry range and ownership, not counted as
another recreated routine.

C10A24/C10AB2/C10AE6/C10B1E retain the smoothing/key gates, actual cancellation,
message reset, cockpit refresh, position preset and mode-dependent sound children.
C10C08 publishes context or viewport readiness. C10C68 preserves LINK -6,
its saved mode and local cursor, actual C25070 heading child and message
terminator. It reloads child-changed local state before subsequent queue writes.
C10CFE/C10D8A/C11A26/C11A50 retain message/countdown gates, word masks, timer
children and elapsed-sample arithmetic, including optional accumulation.

C10DAE is the complete 137-instruction stage, not an observed prefix. It saves
the original phase in its LINK -2 frame, preserves the control-record bit-nine
route, table word increment and masks, and consumes actual scene, sound,
command, timer, reset and finish children. Its alternate paths retain signed
phase bounds, independent message gates, target/view reset, source viewport
parameters and final callback choices. Values used after children are reloaded
from the original globals or saved frame, without restoring child-changed state.

C09192 keeps the original root-transform child and three returned position
longs. C16D04 prepares the actual timer request, calls C53C78 with its original
PEA argument, and copies both response longs. It neither substitutes a clock
nor recreates the OS child. C25070 preserves the original record scan,
signed word offset, world/heading/BCD children, unsigned division overflow,
two decimal additions and three heading characters. Decimal rounding includes
the source's non-BCD byte `0A`, sticky Z and the original N/V outputs.
Existing typed context, expiry, queue and heading APIs delegate to these domains.

## CPU/RAM and actual dispatch proof

`python tools/recomp/check_menu_context_finish.py` passes 344,064 complete
original calls: 229,376 real-child cases and 114,688 controlled-child cases.
Each layer independently covers every owner boundary, including all 137
C10DAE boundaries and all 75 C25070 boundaries. All registers/high halves,
PC, full SR and all Chip/Slow RAM, including frames and argument stacks,
match without exclusions. Controlled children compare full entry CPU/RAM
before changing registers, CCR, saved mode/cursor, timer responses, target
state, selected context and heading/BCD values.

The real-child fixtures permit actual CIA reads (`fa18_write_log_active=2`)
while suppressing nested game ports. The earlier sampled fixture used the
legacy blocking mode (`1`), which returns `FF` on CIA reads. A runtime trace
found the first divergence at the timer child's FE9136 read. That blocked
fixture is retained as a diagnostic, not evidence of real timer input parity.
The complete real-child layer was rerun with permitted hardware reads.
CPU/RAM snapshots do not establish independent OS or hardware timing parity.

`python tools/recomp/check_menu_context_finish_dispatch.py` passes 10,752
actual ON/shadow/sandbox fixtures, every owned boundary in every mode, full
CPU/RAM and entry/mode/selection/write guards. Each mode has 3,040 hardware-free
source calls and 544 hardware-bearing source calls. ON must start a native
continuation and match original outputs for both categories. Reference modes
must report a completed match for each hardware-free call; a hardware-bearing
call must report exactly one hardware classification, zero matches, zero
incomplete and zero mismatches. Their outputs retain the original execution.
Those 544 calls per reference mode are not completed C comparisons.
The guard also completes an original OS child that returns through runtime
dispatch, requiring no native continuation and zero port calls throughout.

Normal C recorded batch proof matches 4,996 shadow / 5,018 sandbox calls.
Four C10CFE calls per mode remain hardware-classified. Shadow retains four
C10D8A and eighteen C10DAE incomplete calls. C16D04 is called with zero
completed comparisons: 8,114 hardware and 112 incomplete shadow calls,
8,578 hardware sandbox calls. Its raw batch and independent isolation reports
remain separate: isolation retains 8,118 hardware / 112 incomplete shadow
calls and 8,579 hardware sandbox calls, with zero completed comparisons.
No zero-match result is promoted to a recorded C proof.
Seven source-only peers are cold; the generic C10A24 rejection remains retained.
C25070 is absorbed in the batch, then independently matches two calls in each
reference mode, giving 4,998 shadow / 5,020 sandbox completed normal comparisons.
Production liveness, source ownership/ranges and busy/tail contracts are retained.

## Timing, integration and remaining scope

Local DMA timing passes 441 instructions / 14,112 cases. The first run caught
the generator treating C11AA0's memory-destination ADD as a register-destination
ADD. Its family-specific direction handling is corrected. The nine older
families produce exactly the same generator output as preceding HEAD; their
stored bridges were not rewritten. The family-local shift/decimal helpers
leave shared runtime CPU/bus/math and instruction-oracle fixtures unchanged.
All 441 PCs are new to the independently tested union: 16,211 / 518,752.
There is no fresh combined run this batch; last fresh remains 15,370 / 491,840.

The full 510-row gate passes 554,063 shadow / 413,303 sandbox matches, zero
mismatches, exact RAM seals and identical poison frames. All 36,236 isolated
live frames and seals match fresh source OFF. The group probe is exact through
frame 600; ALL remains frame 416 / 361 pixels. Copper fade remains deferred.
GNU and MSVC Release builds pass. Counts, hardware classifications, diagnostic
hashes and build size are in `analysis/figures/native_menu_context_finish_checkpoint.json`.

`python tools/recomp/audit_postflight_completion.py` seals thirteen next owners:
C11788/C11830/C11872/C118A0/C118E6/C118FC/C11934/C11958/C119D4/C1104C/
C0F946/C0F974/C091E6, 205 unique / zero shared boundaries. It implements none
of them. Reconstruct complete postflight/reset/restart callbacks and the
independently called root transform; preserve existing children and shared
transform spans. Continue the remaining original graph, then the plain native
backend and only necessary OS services. The seeded 624 denominator does not
close cold or indirect original owners; Stage D and the full C port remain open.
