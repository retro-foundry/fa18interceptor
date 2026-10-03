# Complete postflight message and text owners (2026-10-03)

Sixteen complete owners reconstruct the original text publisher, outcome
messages and return/retry callbacks:
C0F4D8, C0F812, C11078, C110A4, C11350, C113E4, C1141E, C11446,
C11478, C114D2, C1159E, C115BA, C1169A, C116B0, C116CE and C11738.
The registry contains 476/624 translated entries plus 53 original source-only
callable entries: 529 rows and 343 timing entries (290 translated plus 53
source-only). C110A4 and C11350 are added translated owners; C11078's old
fixed-charge adapter is completed; thirteen source-only entries are added.
The 624 denominator remains recording-seeded translation coverage, not whole-game coverage.

`python tools/recomp/audit_postflight_messages_source.py` seals 513 unique /
zero shared boundaries within this batch and all four C111E8 branch-table
arms. The index is the original signed extended mode minus four, checked
against zero and four before ASL.L #1. The arms remain source boundaries
inside C110A4. They are not counted as independent routines. C15C36's actual
external JSR proves the independently callable C0F4D8 entry; its six original
bytes and return address are sealed separately. The remaining source-only
entries have original callback publications recorded in the checkpoint.

`postflight_messages.c` provides readable game behavior, with 37 actual child
sites in its CPU adapter. Calls retain their original return PCs and stack
arguments, including the table PEAs, file selection, indexed message, formatter,
delay and display bounds. The production adapter invokes actual children.
The native domain requires service hooks for children which depend on platform
services; it does not fabricate their results.

C110A4 preserves the saved unsigned local mode, reloaded signed global mode,
four branch arms, phase-specific message codes and the two-byte cursor gap.
It reloads the frame cursor after actual children before writing the terminator.
The controlled child deliberately changes both the saved local mode and cursor
to verify those reloads. C11350 saves its original row before the child,
reloads the current table and mode afterward, increments the table word, and
uses the original signed global-byte limit and unsigned row-byte limit.
Those global bytes are C458A7/C458A8, named SCENE_DISPATCH_LIMIT and
SCENE_DISPATCH_LIMIT_PREVIOUS, rather than ATTEMPTS_LEFT at C45898.
C0F812 copies all 32 words, preserves the original signature tests and byte
ORs, and invokes the original formatter or delay. C115BA copies at most 24
nonzero bytes and adds no terminator that the original did not write.
C1141E intentionally installs itself after its two negative gates.

The header and registry establish that the legacy `queue_mode_messages` API
correctly maps to C10678 and delegates `advance_menu_mode_messages`.
The earlier handoff's proposed correction was mistaken. That API remains
correct; `prepare_postflight_messages` separately implements complete C110A4.

`python tools/recomp/check_postflight_messages.py` passes 344,064 complete
CPU/full-SR/all-RAM calls, without exclusions. The controlled layer passes
131,072 cases across all sixteen owners and covers every one of the 513
boundaries. The real layer passes 212,992 cases across thirteen owners and
covers all 411 of their boundaries. Real children use write-log mode 2, which
permits actual CIA reads and hardware side effects while suppressing nested
native port comparisons. The controlled layer checks complete child-entry
CPU and RAM before changing registers and selected inputs.

Three original service-dependent owners do not complete the real-child fixture:
C0F4D8 stops at FC1FC4 in case 0; C11478 stops at FC0FF0 in case 32;
C114D2 stops at FC0FF0 in case 0. Their nonzero exits and raw logs remain
retained, and no complete real-child proof is counted for them. C110A4's
real fixture uses the original nonzero MENU_TABLE_STATUS guard for the file
child, separate from MODE_TABLE[0]. This does not prove the child's actual
file-loading path. Independent OS/file/hardware timing parity remains open.

`python tools/recomp/check_postflight_messages_dispatch.py` passes 9,984
actual ON/shadow/sandbox fixtures across the thirteen real-completing owners.
Every mode covers every boundary in those owners, compares full CPU/SR/RAM,
and checks non-call, OFF, selection and source-write invalidation guards.
ON requires a native continuation. Reference modes require the existing
completed-match classification for hardware-free source calls and the exact
hardware classification for hardware-bearing calls. The three service-dependent
owners are explicitly excluded from this real dispatch proof, retaining their
full controlled-child proof separately.

The initial structural and dispatch coverage checks correctly rejected eight
unvisited C110A4 boundaries. Their fixture correlated mode parity with the
phase-word bit. Varying that bit independently covers both outcomes; the
initial failure logs remain diagnostics, and the final tests replace their
case counts rather than add duplicate proof.

The step-disabled normal-C recording proof matches four shadow / four sandbox
calls: C11078 once and C110A4 three times. All fourteen other owners have zero
recorded calls. Six raw reports retain those cold rows, and the generic tool
correctly rejects C0F4D8's zero completed comparisons. No recording call is
invented or counted from RAM-only evidence.

Local DMA passes all 513 instructions / 16,416 cases. Shared runtime CPU/bus/
math and instruction fixtures are unchanged; all eleven older generator
outputs are byte-for-byte unchanged. The combined instruction union is
16,897 unique instructions / 540,704 DMA cases, with 0 overlapping PCs,
documented in `analysis/figures/native_postflight_messages_checkpoint.json`;
it is a union of independent checks, not a fresh combined run. The last fresh
combined run remains 16,384 / 524,288 from the preceding batch.

The full 529-row gate passes 554,063 shadow / 413,303 sandbox matches, zero
mismatches, exact RAM seals and identical poison frames. All 36,236 isolated
live ON frames and seals match source OFF. The group probe is exact through
frame 600; ALL remains frame 416 / 361 pixels. Copper fade remains deferred.
GNU and MSVC Release builds pass; build/ is 0.833 GiB. Evidence paths, hashes and classifications
are recorded in `analysis/figures/native_postflight_messages_checkpoint.json`.

`python tools/recomp/audit_postflight_file_callers.py` seals the next five
related game-side owners: C0F56A, C0EF08, C162E4, C1631C and C16386,
174 unique / zero shared boundaries. This inventory implements none of them
and replaces no OS service. Continue the original game graph, then Stage F
and only necessary Stage E services. Stage D and the whole C port remain open.
