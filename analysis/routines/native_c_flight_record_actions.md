# Complete flight-record action and control-stream owners

Updated 2026-10-03. Ten complete original callable owners are implemented and
registered. The registry is now 500/624 translated entries plus 66 source-only
callable entries: 566 rows and 386 timed entries (320 translated, 66 source-only).
Game-function porting remains incomplete. Stop once all original game functions,
including cold and indirect owners, are recreated and only Kickstart services
and timing remain. This batch replaces no OS service.

`analysis/data/flight_record_actions_source_scope.json` seals 575 unique / 245
shared boundaries against the immutable demo01 state. The preceding scope
inventory retains byte-backed actual JSR/BSR incoming evidence for every entry.
There are 30 per-owner child sites, representing 20 distinct entry/return pairs.
All remain original consumers. Readable behavior is in
`port/game/flight_record_actions.c`; its normal adapter owns CPU outputs, saves,
stack arguments and actual child calls. The generated bridge owns instruction,
bus and event boundaries. No opcode handler is used in either handwritten body.

| Owner | Complete behavior | Owned PCs | Controlled owner PCs | Real owner PCs |
| --- | --- | ---: | ---: | ---: |
| C230E8 | Select release, sound or manoeuvre from the root flag nibble; original global writes, saved source cursor and full return value. | 32 | 32 | 22 |
| C23116 | Alternate action selector with the shared sound/manoeuvre exits. | 25 | 25 | 19 |
| C23186 | Nine signed-word sound arguments, exact long stack publication and original message child. | 8 | 8 | 8 |
| C23228 | Magnitude alert, class-preserving forward record clone, projection publication and shared control-stream playback. | 199 | 177 | 110 |
| C233AA | Recording-mode initialization, stream/reset selection, marker commands, pending messages, limits and playback termination. | 105 | 105 | 59 |
| C23578 | Signed-byte selection advancement and wrapping, stream/view publication and original next-message selection. | 43 | 43 | 43 |
| C236AA | Manoeuvre clone, rotation/matrix children, saved record/source cursors and complete shared action-motion tail. | 155 | 155 | 122 |
| C23716 | Release clone, class 49 publication and complete shared action-motion tail. | 142 | 115 | 115 |
| C2377E | Scene/availability guards, action clone, root statistics and complete shared action-motion tail. | 188 | 164 | 164 |
| C257EC | Stack direction arguments, signed lower-word magnitudes, actual length child, power-of-four scaling, division, products and upper-word sign publication. | 53 | 50 | 50 |

327,680 completed calls match all sixteen registers, PC, full SR and all Chip/
Slow RAM: 16,384 controlled-child and 16,384 real-child calls per entry. The
controlled layer also compares complete child-entry CPU/SR/RAM, changes outputs
and cursors, and proves restoration after rotation/matrix consumers. Real-child
coverage is partial as shown above. C23228's raw 150-PC observation includes
nested C23578 instructions; only 110 belong to that parent.

Coverage categories remain distinct, and every original PC stays owned:

- The completed controlled owner union is 550/575. C23348 tests the inhibit
  byte; C2334E BNE and C23350 BEQ consume that same TST result. The 22-PC
  recording arm cannot be reached from the sealed entry without changing the
  flags between those branches. Its complete internal C23354-to-RTS segment
  separately matches 1,024 CPU/SR/RAM cases and all 23 segment PCs.
- Valid, disjoint-record release fixtures publish class 49 before the tail,
  so their 27-PC alternate-class arm is not observed in completed C23716 calls.
  The action fixtures publish class 0/1, leaving 24 class-48/49 PCs unobserved
  in C2377E. These paths are covered by completed C236AA child contracts and
  by 1,024 direct C2385A internal segments covering all 130 tail PCs. Neither
  internal segment is a new registered callable owner.
- Zero lower-word scale enters the original write-21/fault-child/repeating
  back edge. 1,024 observations match at the first actual fault-child entry,
  including the caller frame and all RAM. The test stops at that boundary;
  it does not return from the service or count a completed call. C257EA's
  nonreturning back edge has independent instruction/bus/cycle proof.
  Minimum-word scale retains the original nonreturning scaling behavior.

The normalizer uses the saved scale's upper word for final sign, retains DIVU
overflow and the actual zero-divisor exception backend, and has no added
zero-length shortcut. Instruction fixtures prove exception entry; complete
normalizer Kickstart-handler return remains unproven. Source WORD/BYTE halves,
MOVEM.W sign extension, ordered 41-long copies and child outputs are preserved.

Actual ON/shadow/sandbox dispatch passes 7,680 completed fixtures and all OFF,
selection, non-call and source-write invalidation guards. Every fixture is
hardware-free and requires exact completed classification. The C257EC guard
uses its actual translated label 2; label 0 belongs to the preceding fault loop.
The shared production runtime and classification assertions are unchanged.

Independent normal C, with steps disabled and ownership retained, passes
20,301 shadow / 20,394 sandbox recorded comparisons for C230E8/C23116/C23228.
C230E8 retains 86 shadow incompletes and C23228 seven; no sandbox incompletes,
hardware classifications or mismatches occur. The other seven owners have zero
calls in all three recordings even in the separate cold selection, with the
three parents omitted. They have complete fixture proof and original incoming
call evidence. Both generic batch zero-comparison rejections remain recorded;
the unchanged hot-entry checker passes. No synthetic recorded calls are claimed.

Local DMA proof passes 575 instructions / 18,400 cases. A fresh combined run
passes 20,259 / 648,288; independent union accounting starts with 19,855,
adds 575 and counts the 171 previous-overlap PCs once. All nineteen older
generator outputs and shared CPU/bus/memory/math/runtime/proof files are
unchanged. GNU headless and MSVC Release builds pass.

The full 566-row recording gate passes 567,984 shadow / 417,363 sandbox matches,
zero mismatches, exact seals and identical poison frames. All 36,236 isolated
live RGB444 frames and final seals match. The family is exact through frame 600;
ALL retains its frame-416 / 361-pixel difference. Copper-fade and OS/timing work
remain deferred. `build/` is 1.137 GiB at this checkpoint.

Reproduce with `audit_flight_record_actions_source.py`,
`check_flight_record_actions.py`, `check_flight_record_actions_dispatch.py`,
the unchanged hot-entry `check_whole_call_glue.py`, and
`check_active_planes_step.py --group flight_record_actions --bus` or `--group all`.
Raw logs, report hashes and per-owner counts are recorded in
`analysis/figures/native_flight_record_actions_checkpoint.json`.

Next complete C25B66/C26322/C26352/C266AE/C26C72/C26CC0/C26D8A/C28996/C28B16:
1,537 unique / zero shared boundaries, each with actual incoming JSR/BSR bytes,
sealed in `analysis/data/flight_dynamics_scope_inventory.json` and reproducible
with `tools/recomp/audit_flight_dynamics.py`. The related C2C392 owner is called
at C25C6A but its computed transfer at C2C46E must be reconciled before its
complete scope can be sealed. The nine-owner inventory is not implementation.
