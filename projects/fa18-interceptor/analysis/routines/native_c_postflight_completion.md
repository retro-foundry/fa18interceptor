# Complete postflight/reset/restart owners (2026-10-03)

C11788/C11830/C11872/C118A0/C118E6/C118FC/C11934/C11958/C119D4/
C1104C/C0F946/C0F974/C091E6 now have complete readable C, normal CPU
adapters and original source timing. Nine older fixed-cycle adapters are
completed and four original source-only callable entries are added.
Coverage remains 474/624 translated entries and extends to 40 source-only
entries: 514 rows and 327 timed entries (287 translated plus 40 source-only).
The seeded translation does not describe the complete original game graph.

## Source behavior and ownership

`python tools/recomp/audit_postflight_completion_source.py` seals all 205
unique / zero shared boundaries within this batch. C118FC/C11934 and C1104C
are original installed callbacks; C09192 actually calls the independent
C091E6 root-transform entry. Source callability evidence is retained in
`analysis/figures/native_postflight_completion_checkpoint.json`.

`postflight_completion.c` owns the twelve callback domains. C11788 preserves
its context/player gates, bit-ten test, two successive cockpit flag masks,
actual scene reset, status mask and wrapping byte decrement. Its positive
remaining count installs C11830; the alternative consumes actual buffer
clear before publishing the failure callback. C11830 retains the root flag,
context-dependent view child, actual scene child and countdown. The adapters
consume original children and preserve their changed registers, CCR and RAM.
They replace the older approach that independently repeated native child
effects and then guessed the children's register tails.

C118A0 retains its PEA table argument, actual C11ACC child, reloaded failure
input and the two original message codes. C11872/C118E6/C118FC/C11934 retain
their countdown/message gates and callback publication. C11958 preserves its
finished-message route, phase-FF reset and phase-one immediate restart; its
MOVE.B/WORD high halves and SUBQ flags remain explicit. C119D4 preserves the
conditional request flag, three-pass countdown and viewport chain.
C1104C/C0F946/C0F974 retain their independent message/viewport gates.
LEA-published callbacks change A0; immediate-long publications preserve it.
Existing typed post-input and scene-reset APIs delegate to these domains,
retaining their actual native child behavior.

C091E6 reuses `matrix.c`'s existing `local_to_world` domain and its established
product outputs. It retains the original A1/A2 MOVEM stack writes and restores,
the root record/inverse matrix, all scratch products, final ADD.L flags and
returned triple. Its 34 boundaries include 32 already timed transform
instructions and two root-entry instructions. The shared domain now adds
signed products as unsigned longs before the signed shift and position add:
three valid MULS.W products can overflow their signed sum, so defined C
wrapping is necessary to preserve the original ADD.L result. The edge fixtures
include all-three `8000 * 8000` products and overflowing position additions.
Root real/controlled fixtures and actual dispatch also cover MOVEM stack writes
overlapping the root position. The final flag input is read after those writes,
in its original order; the focused root run replaces the initial root evidence
without adding duplicate cases to the 319,488 total.

## Independent proof

`python tools/recomp/check_postflight_completion.py` passes 319,488 complete
CPU/RAM cases: 212,992 with actual children and 106,496 with controlled children.
Each layer independently covers every owner boundary. All sixteen registers,
high halves, PC, full SR and all Chip/Slow RAM, including saved/argument stacks,
match without exclusions. Real fixtures permit actual CIA reads using write-log
mode 2. Controlled children compare complete entry CPU/RAM before changing
registers, CCR, remaining count, status and failure input; parent code reloads
the changed fields at their original points. OS/hardware timing parity remains
a separate requirement.

`python tools/recomp/check_postflight_completion_dispatch.py` passes 9,984
actual ON/shadow/sandbox fixtures. Every mode covers every owner boundary;
ON must start its native continuation and both reference modes must report
a completed comparison. All 3,328 source calls per mode are hardware-free.
Non-call, OFF, selection and source-write invalidation guards pass, with full
CPU/SR/RAM comparison and no register/RAM exclusions.

Step-disabled normal C recording proof completes 1,890 shadow / 1,891 sandbox
comparisons. Nine entries are active: C11788 2/3, C11830 2/2, C11872 12/12,
C118A0 6/6, C118E6 375/375, C11958 1,019/1,019, C119D4 101/101,
C0F946 367/367 and C0F974 6/6. One incomplete C11788 shadow call remains
retained; hardware and mismatches are zero. The four new source-only peers
are cold. Their raw zero rows and generic C118FC rejection remain retained;
no zero-call result is counted as a normal C recording comparison. Production
liveness, ownership/ranges and busy/tail contracts remain unchanged.

## Timing, integration and remaining work

Local DMA timing matches 205 instructions / 6,560 cases, including the two
root-entry instructions. The fresh combined DMA run matches 16,384 unique
instructions / 524,288 cases for complete registers, SR, PC, cycles and RAM.
The batch overlaps the preceding union at 32 transform PCs and adds 173.
Shared runtime CPU/bus/math and instruction-oracle fixtures are unchanged;
the combined run nevertheless follows the shared domain arithmetic correction.
All ten preceding generator families produce exactly their preceding-HEAD
output; their stored bridges were not rewritten.

The full 514-row gate matches 554,063 shadow / 413,303 sandbox calls, with zero
mismatches, exact sealed final RAM and identical poison frames. All 36,236
isolated live frames and seals match source OFF. The group probe is exact
through frame 600; ALL remains frame 416 / 361 pixels. Copper fade remains
deferred. GNU and MSVC Release builds pass; build/ is 0.800 GiB. Evidence hashes
are in `analysis/figures/native_postflight_completion_checkpoint.json`.

`python tools/recomp/audit_postflight_messages.py` seals sixteen next owners,
513 unique / zero shared boundaries, including complete C110A4 and all four
original C111E8 table arms. It implements none of them. Include the C0F4D8/
C0F812 text publisher and the actual C11350/OS children when reconstructing
the complete message and restart chain. The legacy typed `queue_mode_messages`
correctly names C10678 and delegates to `advance_menu_mode_messages`;
C110A4 needs its own distinct domain. The earlier delegation correction
proposal was mistaken.
Preserve the preceding OS/file-load limitation and do not substitute returning
children. Continue the original graph, then the plain native backend and only
necessary OS services. Stage D and the whole C port remain open.
