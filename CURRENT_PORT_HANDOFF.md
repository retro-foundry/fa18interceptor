# C port handoff

Updated 2026-10-04. This is the current work state. Older notes remain in git
history (the preceding handoff is in commit 51fb22b2); ignored gate logs may
also remain under build/recomp/.
PORT.md describes the architecture and source conventions.

## Active objective: ROM independence (2026-10-04)

The user has authorized implementing the ROM-independence plan. This supersedes
the earlier stopping point and deferral of OS work recorded below. Deliver a
separate `fa18_romfree` runner that starts from the original ADF without a
Kickstart image or UAE savestate. Retain the CPU/translation and chipset model
for this milestone, and retain the ROM-backed runner as a validation oracle.
Original behavior and the existing machine timeline remain the authority.

The Amiga SDK is reference material only, not a build or runtime dependency.
Reusable Amiga loading and compatibility facilities must be separated from
Interceptor's addresses, resources, and startup configuration. Track unproved
services explicitly; unsupported calls and ROM reads must fail with context.
Completion requires launch, menus, flight, postflight, persistence and exit,
with zero ROM reads/fetches and zero unsupported services. A loader or service
inventory alone does not satisfy that objective.

## Historical objective and order (superseded above)

Recreate readable C for the whole game, proven against the original source and
sealed native recordings. Work in related batches. The current stopping point
is complete original game-function porting: stop when the only remaining work
is Kickstart services and timing parity. This user instruction supersedes the
earlier open-ended full-port objective. Reconcile
the complete original call/callback graph, including cold and indirect owners,
before declaring game-function porting complete. The seeded 624-entry count
alone is not a completion criterion. Keep remaining service and timing debt
explicit in the final handoff; do not continue into Kickstart replacement or
standalone timing work after game-function porting is complete. Any remaining
game implementation or behavior gap prevents claiming this stopping point.
The user explicitly deferred OS work and asked for larger routine batches.
Selector, placement, post-input, context-refresh and bootstrap milestones
are complete at 432/624; the complete record-update and enclosing update-stage
parents raised coverage to 434/624; the complete C29042 selector-origin owner
raised it to 435/624; complete update/input/display owners raised it to 438/624;
four complete input-event owners raised it to 442/624; complete C1AC28/C1AD74
command dispatch owners raised it to 444/624; the complete postflight mode
scheduler family raised it to 454/624; complete context publishers and
selected-record helpers raised it to 459/624; complete menu-transition
callbacks and their sound/summary helpers raised it to 465/624; complete
top-level menu setup and input/message helpers raised it to 469/624; complete
menu follow-ups and the table-file owner raised it to 470/624; complete
delayed-menu/outcome callbacks raised it to 471/624; fourteen complete
menu/context return owners retain that translated count; fourteen complete
menu/context completion owners now raise it to 474/624; thirteen complete
postflight/reset/restart owners retain that translated count; sixteen complete
postflight-message/text owners raise it to 476/624; the complete formatter
and four game-side mode-file callers raise it to 477/624; eight complete
input-device callback/setup owners raise it to 480/624. Four gameport/text
setup owners retain that count and extend source-only coverage to sixty-five.
Complete main-loop timer/readout and setup-bounds owners now raise coverage
to 481/624 and source-only coverage to sixty-six. Complete main-loop control
records and message sequences raise translated coverage to 483/624. Complete
control-record action and alert owners raise it to 488/624. Complete control/
flight parents and four existing helper upgrades raised it to 490/624. Ten
complete flight-record action/control-stream owners raised it to 500/624.
Five complete motion-projection, publication and collision helpers raised
it to 505/624. Four complete flight-dynamics parents and the shared C28B34
upgrade raised it to 509/624. Four complete history, zone-exit and candidate-
geometry upgrades retain that translated count and raise source-timed entries
to 399. Five complete grid, scene-label and record-marker owners now raise
translated coverage to 514/624 and source-timed entries to 404.
Six complete projection/readout/sweep owners and four projection upgrades now
raise translated coverage to 520/624 and source-timed entries to 410.
Six complete stream/numeric/marker owners and two store upgrades now raise
translated coverage to 526/624 and source-timed entries to 418.
Eight complete display-parent upgrades retain that translated count and raise
source-timed entries to 426.
Thirteen complete readout, cue and status upgrades retain that translated count
and raise source-timed entries to 439.
Eight complete HUD cache/text helper upgrades retain that translated count
and raise source-timed entries to 447.
Six complete projection, cue and conditional-text parent upgrades retain
that translated count and raise source-timed entries to 451.
Five complete history/display and indirect stream upgrades retain that
translated count and raise source-timed entries to 456.
Ten complete older render-parent upgrades retain that translated count
and raise source-timed entries to 465.
Seventeen complete face/segment/grid/block stream-parent upgrades retain
that translated count and raise source-timed entries to 481.
Fourteen complete tested/list/grid/lattice face and edge upgrades retain
that translated count and raise source-timed entries to 494.
Fifteen complete ground/HUD mark/panel/blit parent upgrades retain that
translated count and raise source-timed entries to 509.
Twelve complete renderer-helper upgrades retain that translated count and
raise source-timed entries to 510. Thirty-seven complete renderer entries and
writer callbacks now raise translated coverage to 527/624, source-only
coverage to 73 and source-timed entries to 518. Twelve complete selected-
segment, projection and crossing upgrades retain registration and raise
source-timed entries to 521. Twelve complete corner/view owners raise
translated coverage to 537/624 and source-timed entries to 531; C200F6
retains aggregate tail timing, with its resumable boundaries in C20100.
Six complete control/readout owners raise translated coverage to 539/624
and source-timed entries to 537.
Six complete steering owners retain translated coverage, raise source-only
coverage to seventy-five and source-timed entries to 543.
Seventy-five original source-only callable entries are additionally recreated,
proven and activated; they do not increase the seeded 624-entry denominator.
The latest user instruction defers the minor Copper-fade difference: keep its frame-313
HUD/countdown evidence for later and continue complete readable game batches.
Do not make fade timing the next work item or weaken the normal parity gates.

## Display-selection reference frozen before ROM-free work

The display-selection implementation in c3ecdcf8 has completed its pending
integration gates against the reference runtime, before shared machine changes.
Registration remains 539 translated +75 source-only =614 total,
543 source-timed. Do not infer full game completion from those counts.

Seven owners C0D74A/C0D752/C0DAA0/C0DAD0/C0DAD4/C0DADC/C0DAE6 now use
readable domain C, actual original child results and source continuations;
the older enclosing register replay and five pair adapters are removed.
Source scope: 259 unique /229 shared instructions, 57 child contracts and
three sealed oracle arrays. All 229,376 whole calls pass registers, PC, full
SR, all RAM, ordered Custom writes and terminal hardware state. Controlled
union is 259/259; original union is 256/259, missing only three rounding
increments covered by controlled calls and local DMA. Local DMA passes
259/8,288; dispatch smoke passes 1,344 calls (64 per owner per mode).
MSVC Release/GNU oracle builds and compatibility for 41 older generators pass.

Dispatch passes 21,504 calls (1,024 per owner per mode). Normal-C recording
comparisons pass 12,396 shadow /7,437 sandbox. Combined DMA passes 30,239
instructions /967,648 fixtures; the 259 display PCs already belong to the
previous union. The full 614-row gate passes 571,427 shadow /458,087 sandbox,
all seals and poison frames. All 36,236 isolated live frames and seals match.
Family timing is exact through 600 frames; ALL retains the inherited first
difference at frame 424 /34,144 pixels. Exact commands and changed files
are in analysis/routines/native_c_display_record_selection.md; sealed local
proof and implementation hashes are in
analysis/figures/native_display_record_selection_checkpoint.json.
The recordings verifier passes. Ignored build/recomp install/prepare scripts are one-shot; do not
rerun them. Use the committed checkers to reproduce proofs instead.

C2C392 producer-domain evidence and the other game work listed below remain
open. Read-only local notes additionally cover C0E78A/C2F49C/C500D8 in
build/recomp/remaining_game_services_read.{json,log} and
build/recomp/audio_registration_read.log; none is newly implemented.
Do not treat data following C500C8 as decoded game code. C500D8 needs original
callback installation/callability evidence. These remain separately tracked
game-port debts while the authorized ROM-independence work proceeds.

## ROM-free implementation progress

`port/amiga` builds independently with GNU and MSVC and requires no Amiga SDK,
ROM, emulator, or game assets. It provides guest memory, Hunk relocation,
read-only OFS files and metadata, ROM-dependency audit, a strict runtime guard,
and a resumable service registry. The legacy disk and Hunk APIs remain
compatible with the typed native port. SDK definitions are reference only.
All four portable contract tests pass GNU and MSVC Release. The OFS reader
matches all 22 game-owned resources against their original extraction hashes;
the loader places all 185 hunks directly from the original ADF.

The DOS segment-list inspector resolves all 185 hunks and verifies all 8,441
relocations. A true Engine9000 power-on launch, using the sealed desktop inputs
after 3,000 boot frames, stops at C0DEB0 on host frame 7,070. Its complete Hunk
placement matches the warm recordings, with zero changed nonrelocated bytes.
Only hashes, addresses, and register metadata are committed in
`analysis/data/romfree_hunk_layout.json` and `romfree_cold_entry.json`; captured
RAM stays under ignored `build/amiga` as oracle evidence.

The seven existing service bridges, Exec FindTask/FindName, all seven list
operations, message/signal/task protection, Supervisor, memory and scheduling now execute 817 C phases
without reading ROM instruction or operand bytes.
Nested calls preserve the original stack and vectors. The structural oracle
passes 418,304 CPU/DMA fixtures with cleared
ROM buffers, a strict access guard, full registers/SR/RAM, ordered memory and
hardware accesses, and exact cycles. It corrected potgo's original low-word-
first stack write. Blitter ownership's deeper helpers still execute ROM in the
reference runner and need complete replacements.

FindTask/FindName additionally pass 9,216 complete original-versus-C calls,
including null, absent, prefix and case-sensitive queries, empty/nonempty task
lists, interrupt depth boundaries, full CCR and display DMA. Original Engine9000
contracts include the true game's FindTask caller returning to C0DEE2 and its
nested FindName path. The shared dispatcher runs before interpreter opcode
fetching; strict ROM access enforcement is available but cannot yet be enabled
for a whole-game run because other service dependencies remain.

The reusable `port/amiga/exec_lists.c` implements Insert, AddHead, AddTail,
Remove, RemHead, RemTail and Enqueue; ABI/timing remains in its machine adapter.
All 63 list phases are covered by 21,504 complete CPU/DMA calls with exact full
register/SR/RAM/access/cycle parity, independent topology assertions and stable
signed priority ordering. Original Engine9000 contracts cover Insert, RemHead
and Enqueue; the four unobserved services have static/controlled proofs.
See `analysis/routines/fc15e8_fc1694_exec_lists.md`.

Reusable `exec_task_services.c` adds PutMsg, ReplyMsg, WaitPort, SetSignal,
SetExcept, Signal, Wait, AllocSignal/FreeSignal, AllocTrap/FreeTrap, Forbid,
Permit and Reschedule semantics. The pinned adapter preserves nested Signal,
Enqueue, Wait, Cause and Supervisor calls on the original machine timeline.
43,008 complete nonblocking CPU/DMA calls match all registers, full SR, RAM,
ordered accesses and cycles with ROM/rtarea cleared and the guard active.
Fixtures cover empty/nonempty message queues, all port actions, controlled
guest callbacks, task wakeups, deferred rescheduling, pending Wait, bit
exhaustion and nesting. Blocking switches are now proved in the scheduler batch
below; real Cause remains pending. Controlled callbacks do not replace missing services.
Original Engine9000 contracts cover PutMsg, ReplyMsg, Signal, SetSignal,
Permit and nonempty WaitPort. A blocking Wait observation enters idle STOP
and did not return within its instruction bound. It is recorded as incomplete.
See `analysis/routines/fc1b76_fc2046_exec_task_services.md` and
`analysis/data/romfree_exec_task_contracts.json`. The reference runner enables
the new signatures by default; `--no-os-task-services` retains the ROM oracle.

Supervisor FC08E6, its privilege-frame handler FC090E–FC092A and the Permit
callback FC1FBE–FC1FC6 now execute without ROM reads. Reusable frame operations
remain in `amiga_compat`; the retained CPU owns privilege exceptions, stack-bank
switches and RTE. 3,072 complete CPU/DMA calls prove user/supervisor paths,
nested callbacks, both stack banks, full SR and supervisor-mode Permit/
Reschedule callbacks. The true cold-entry checkpoint captures four original
instructions /47 OCS colour clocks through privilege-frame rewriting, before
callback transfer. The real callback proceeds into Switch; its original
captured complete return remains unobserved. Scheduler services and controlled complete returns are now
proved in the batch below.
The reference runner enables the verified signatures by default; use
`--no-os-supervisor` for the ROM oracle. Both GNU and MSVC Release full service
replay comparisons pass all 36,236 frames/seals. See
`analysis/routines/fc08e6_fc092a_exec_supervisor.md` and
`analysis/data/romfree_exec_supervisor_entry.json`.

Reusable `exec_memory.c` adds Allocate/Deallocate, core AllocMem/FreeMem,
AllocAbs, TypeOfMem and AvailMem. All 237 memory phases pass CPU/DMA proof;
21,568 complete calls and 2,048 original Alert-vector paths match registers,
full SR, RAM, ordered accesses and cycles with ROM/rtarea cleared. Independent
contracts check free-list topology/accounting, fragmentation, both coalescing
directions, attribute filtering, zero/wrapping sizes, clearing and failures.
The large clear fixture writes 65,537 longs and exercises the high-word loop.
Original zero-size AllocAbs at a free-chunk start self-links that chunk; this
is explicitly asserted, not replaced with invented behavior. AllocEntry,
FreeEntry, complete Alert handling and the installed AllocMem retry wrapper
remain pending. The reference runner enables the pinned memory signatures by
default; `--no-os-memory` retains the ROM oracle. GNU/MSVC Release full replay
comparisons still match all 36,236 frames/seals.

The cold public AllocMem vector C001B0 points to RAM wrapper C06550, which
uses common stack wrapper C06598 and private vector base C06522 to call ROM
retry/scavenging code FE491E, then core AllocMem FC17D0. Engine9000 entry/exit
contracts now cover Allocate, Deallocate, core AllocMem, FreeMem, AvailMem
and the installed wrapper. TypeOfMem was not reached in 300 cold-start frames;
TypeOfMem/AllocAbs have static source and controlled proofs. Captured wrapper
RAM is evidence only, never runtime initialization input. See
`analysis/routines/fc16d8_fc1958_exec_memory.md` and
`analysis/data/romfree_exec_memory_contracts.json`. Soft interrupts and generic exception paths are still required.

The 68000 scheduler is now implemented in reusable `exec_scheduler.c` and
`exec_context.c`, with pinned ABI/timing in `exec_scheduler_adapter.c`.
All 148 scheduler phases and 8,192 complete CPU/DMA calls prove task selection,
contexts, callbacks, interrupt exit and blocking Wait/WaitPort. Source/C
registers, full SR, both stack banks, all RAM, ordered accesses and cycles match
with ROM/rtarea cleared and forbidden-access counters zero. Memory services
share the context-transfer implementation and retain their complete call proof.

Original warm-demo Switch reaches FC0FF0 in 45 instructions /425 OCS clocks;
interrupt exit reaches FC0EC0 in eight /82 clocks. Both real captured entries
now replay with exact checkpoint registers/full SR/all RAM and exact native
ROM/C bus accesses and cycles. Switch matches Engine9000 timing; interrupt exit
has the inherited native-reference 73 versus Engine9000 82 clocks with bus timing
disabled. The replacement introduces no difference. Captures stop before RTE;
complete returns are covered by the controlled fixtures. Metadata and hashes
remain in `analysis/data/romfree_exec_scheduler_contracts.json`; captured RAM
is ignored oracle evidence and never runtime initialization input.

Service STOP now yields through the real interpreter hook, preserving machine
ownership of idle time. Another 1,024 complete CPU/DMA tests prove empty queues
and IRQs accepted immediately during STOP, including no continuation/IRQ opcode
fetch. `--no-os-scheduler` retains the ROM oracle. GNU/MSVC Release full replays
match all 36,236 sealed frames, final RAM and CPU cycles. The fresh full 614-row
gate retains 571,427 shadow /458,087 sandbox comparisons, all RAM seals and
identical poison frames (`build/recomp/exec_scheduler_full_gate.log`). Routine
resumes yield at context returns so the outer dispatcher preserves these original
comparison totals. See
`analysis/routines/fc0e9c_fc10c4_exec_scheduler.md` and the reports under
`build/amiga/exec-scheduler-recordings{,-msvc}/full.json`. Soft interrupts/Cause,
IRQ registration/handlers and generic traps/exceptions remain pending. The cold
Exec vectors identify Cause FC135C, SetIntVector FC11CA, AddIntServer FC1210
and RemIntServer FC1250 for the next foundation batch; capture original contracts
and prove their complete nested dependencies before replacing them.

`analysis/data/romfree_startup_checkpoint.json` records original execution from
C0DEB0 to C0E27E: 1,632 instructions /11,677 OCS colour clocks, 1,065 ROM/rtarea
instruction PCs and 74 ROM-boundary transitions. This includes IRQs and OS task
execution, not just game-origin calls. It is one Workbench startup path and
does not close the complete inventory or authorize captured-RAM initialization.

The audit records nested ROM flow, CPU entry state, accesses, low-memory vector
activity, and machine cycles. All three existing recording audits retain their
final RAM seals; they are warm coverage, not proof of every game mode or clean
startup. A separate 300-frame audit/non-audit comparison is byte-identical.

`fa18_romfree` is not delivered yet. Remaining Exec foundations, interrupt/exception paths,
graphics LoadView and helpers, devices, DOS persistence, evidence-backed OS
initialization and shutdown remain. Whole-game zero-ROM acceptance is pending.

Explicit machine construction is implemented in `port/machine/startup.h`:
`fa18_machine_init` clears all banks and resets CPU/timeline/blitter state under
the ROM guard; `fa18_machine_prepare_run` seeds DMA after separately installing
assets and OS structures. The caller supplies verified CPU/device state. This
does not initialize the game's OS or use captured RAM. GNU and MSVC Release
constructor fixtures pass without reading ROMs, states or game assets, including
repeat initialization, live frames, failure atomicity and blitter-state clearing.
CMake now shares `fa18_runtime` between the oracle executable and constructor
test. The post-extraction MSVC full C-service/ROM-service replay comparison
matches all 36,236 frames and seals. See `analysis/routines/romfree_machine_startup.md`.

The converted phases are integrated into the reference runner. GNU and MSVC
Release builds pass. The fresh 614-row full gate passes 571,427 shadow /458,087
sandbox comparisons, exact final seals and poison frames. C services versus
original ROM services match all three sealed recordings over 36,236 frames,
including every RGB444 byte, complete final RAM, CPU cycles and final PC.
The reusable checker is `tools/amiga/check_service_recordings.py`; local results
are `build/amiga/service-recordings/full.json`. These tests prove the replaced
services under the existing runtime, not a clean ROM-free launch. Ordered
full-recording interrupt/audio timelines and remaining game modes are pending.

## Verified baseline

- Six complete steering owners retain 539 translated entries and add two
  source-only peers: 75 source-only, 614 rows, 543 source-timed (468 translated
  plus 75 source-only). Four older adapters are replaced. Scope is 72 unique
  /34 shared instructions, zero child sites, original BSR callers and two
  sealed oracle arrays. All 98,304 whole calls pass; union coverage is 72/72.
  Original zero/minus-one D1 prefixes prove which mask arm each pitch entry
  skips for all 256 control bytes; the counterpart covers each skipped arm.
  Dispatch passes 18,432 calls; each mode has 6,144 hardware-free source
  calls and zero Custom packets. C2CBBC's initial ON range failure is retained;
  registration now includes its backward shared C2CB86 body. Shared runtime
  is unchanged. Four existing owners pass normal C: 162 shadow /162 sandbox.
  Two source-only peers are uncalled with independent whole proofs; the
  unchanged generic rejection C2CB82: no completed comparisons is retained.
  Full 614-row gate passes 571,427/458,087, zero mismatches, exact seals and
  poison frames. All 36,236 isolated live frames/seals match. Local DMA passes
  72/2,304; fresh combined DMA passes 30,239/967,648, independently previous
  30,167 plus 72 with zero overlap. All 40 older generator outputs, shared
  core/observers, protected files, previous control/readout code and unrelated
  batch48 exports are unchanged. GNU/MSVC Release pass; build/ is 2.041 GiB.
  Family exact through
  600; ALL remains 424/34,144. See analysis/routines/native_c_record_steering.md
  and analysis/figures/native_record_steering_checkpoint.json.
  Next complete C0D74A/C0D752 enclosing display-record owners and five pair
  writers C0DAA0/C0DAD0/C0DAD4/C0DADC/C0DAE6. All 259 unique /229 shared
  source instructions are read; each parent has 57 original child sites.
  Use actual C2E758 child results and preserve LINK/dead stack bytes/shared
  frame exits; remove the old corner register replay once these owners pass.
  The display implementation now exists; its pending integration is recorded
  in the current handoff above. C2C392 producer reconciliation
  remains open: eight original stream descriptors publish actions 35,11,39,
  14,17,20,23,26; bounded nonnegative C458A7 parameter selections publish
  2,5,9. Record copies and saved-state inputs still need tracing. Forty table
  pointers do not independently establish all producer bounds.
  Other larger parents, older partial adapters, C1612C graphics-wait
  integration and full original cold/indirect/callback reconciliation remain
  game work. Goal is not complete. Kickstart services and timing are deferred.

- Six complete control/readout owners raise registration to 539 translated
  plus 73 source-only entries: 612 rows, 537 source-timed (464 translated
  plus 73 source-only). C12950/C131BE are newly registered; C13176/C133B2/
  C13396/C52EC8 replace older adapters. Scope is 867 unique /0 shared PCs,
  43 child contracts/sites, three sealed oracle arrays and both original
  guarded computed target sets. Their eight branch stubs retain C131BE's
  frame and are not standalone registrations. All 196,608 whole calls pass;
  controlled and original unions each cover 866/867. C133FA is unreachable
  for all 65,536 word inputs; its source body and DMA boundary remain.
  Dispatch passes 18,432 calls; each mode has 6,144 hardware-free source
  calls and 5,376 ordered Custom writes. Four owners pass normal C:
  15,961 shadow /16,016 sandbox, with 124 incomplete shadow calls explicit.
  C13176 has one completed isolated sandbox comparison. C133B2/C13396 are
  uncalled with independent whole proofs; the unchanged generic checker
  rejection remains. Initial C12950 CPU comparisons passed but branch
  coverage failed; independent valid input witnesses complete final coverage.
  Full 612-row gate passes 571,427/458,087, zero mismatches, exact seals
  and poison frames. All 36,236 isolated live frames/seals match. Local DMA
  passes 867/27,744; fresh combined DMA passes 30,167/965,344, independently
  previous 29,300 plus 867 with zero overlap. All 39 older generator outputs,
  shared core/observers, protected files and previous corner native code are
  unchanged. GNU/MSVC Release pass; build/ is 2.015 GiB. Family exact through
  600; ALL remains 424/34,144. See analysis/routines/native_c_control_readouts.md
  and analysis/figures/native_control_readouts_checkpoint.json.
  Those four steering upgrades and two source-only pitch peers are now
  complete above. C2C392 remains open. Forty contiguous original
  pointer slots are read; all action-byte producers and the computed target
  domain remain unresolved. The instruction decrements before sign-extending
  the byte; preserve that order when auditing selector wrap. C241EE selects
  packed parameter data at C2BB98 and C242D0 writes the action byte; follow
  its table and C458A7 writers. Display-selection integration is pending above.
  Other larger parents, older partial adapters, C1612C graphics-wait
  integration and full original cold/indirect/callback reconciliation remain
  game work. Goal is not complete. Kickstart services and timing are deferred.

- Twelve complete corner/view owners raise registration to 537 translated
  plus 73 source-only entries: 610 rows, 531 source-timed (458 translated
  plus 73 source-only). Two existing owners are upgraded and ten seeded
  owners newly registered. Scope is 515 unique /106 shared instructions,
  25 distinct child contracts /31 per-owner sites, four sealed oracle arrays,
  original static incoming transfers and the C2CE5E pair template.
  All 393,216 whole CPU/PC/full-SR/all-RAM calls pass. Controlled union is
  515/515; C2CD28 covers 102/139 and the missing shared 37 PCs are covered
  by C2D082. Original-child union is 433/515; all missing PCs remain explicit.
  Original C2EA02 non-returning behavior passes 16,384 observations of 64
  branches; no game return or cap is supplied. Actual dispatch passes 34,816
  calls: eleven owners in all three modes and C200F6 synchronous ON only.
  Custom packets are 110,086 ON /107,526 shadow /107,526 sandbox;
  source classifications are hardware-free and distinct from fixture packets.
  C200F6 restores saved pointers above its RTS return. Its whole C tail is
  proven with aggregate 40+16 cycles; C20100 already owns all three resumable
  boundaries. The initial standalone pending-frame failure is retained.
  Normal C passes C2E758/C2CE82/C2D082: 7,083 shadow /7,962 sandbox. Nine
  owners are uncalled and have independent whole fixtures; the unchanged
  checker rejection C2CCA0: no completed comparisons is retained.
  Full 610-row gate passes 568,446/443,870, zero mismatches, exact seals and
  identical poison frames. All 36,236 isolated live frames/seals match.
  Local DMA passes 515/16,480; fresh combined DMA passes 29,300/937,600,
  independently reconciled as previous 29,042 plus 515 minus overlap 257.
  All 38 older generator recipes produce unchanged outputs; shared core,
  observers and protected files remain unchanged. Historically stale committed
  command-dispatch output is retained exactly. Older corner_edges_registers
  replay remains unchanged for its enclosing display owner. GNU/MSVC Release
  pass; build/ is 1.974 GiB. Family exact through 600; ALL remains 424/34,144.
  See analysis/routines/native_c_corner_view.md and
  analysis/figures/native_corner_view_checkpoint.json.
  Those C12950/C131BE control-record selectors and magnitude owners are
  now complete above. C2C392 has forty contiguous original pointer slots;
  its computed target producer domain remains unresolved.
  Other larger parents, older partial adapters, C1612C graphics-wait integration
  and full original cold/indirect/callback reconciliation remain game work.
  Goal is not complete. Kickstart services and timing remain deferred.

- Twelve complete selected-segment/projection/crossing upgrades retain
  527 translated plus 73 source-only entries: 600 rows, now 521 source-timed
  (448 translated plus 73 source-only). The scope is 663 unique /60 shared
  PCs and twenty distinct child sites. Both selected A4 targets and their
  original table slots/callers are sealed. All 393,216 whole CPU/PC/full-SR/
  all-RAM calls pass; controlled coverage is 663/663, original-child union
  610/663 with every missing PC explicit. C2EE4A covers all 211 boundaries
  with independently varied child Z returns. All four original unrounded
  parallel loops retain non-returning behavior: 65,536 independent observations
  of 64 branches match full CPU/PC/SR/RAM. The retained initial returning
  fixture entered an original loop and failed its budget; bounded returning
  fixtures and non-returning proofs remain separate. Dispatch passes 36,864
  calls; each mode has zero hardware-bearing /12,288 hardware-free classifications
  and 21,852 actual Custom packets. All twelve owners pass normal C:
  106,838 shadow /126,128 sandbox. Full 600-row gate passes
  568,446/443,870, zero mismatches, exact seals and identical poison frames.
  All 36,236 isolated live frames/seals match. Local DMA passes 663/21,216;
  fresh combined DMA passes 29,042/929,344, previous 28,931 plus 663,
  overlap 552. All thirty-seven older generators and shared core/observers
  are unchanged. GNU/MSVC Release pass; build/ is 1.921 GiB.
  Family matches through 600; ALL remains 424/34,144. See
  analysis/routines/native_c_segment_projection.md and
  analysis/figures/native_segment_projection_checkpoint.json.
  Those twelve corner/view owners are now complete above. Larger parents,
  older adapters, C2C392, C1612C graphics-wait integration and original
  cold/indirect/callback reconciliation remain game work. Goal is not complete.

- Thirty-seven complete renderer entries/writer callbacks raise registration
  to 527 translated plus 73 source-only entries: 600 rows, 518 source-timed
  (445 translated plus 73 source-only). Twenty-nine existing owners are
  upgraded, C2F688 is newly seeded and seven original table callbacks are
  source-only. The scope is 550 unique /235 shared PCs, seven distinct /
  eight per-owner child sites. All 1,212,416 whole CPU/PC/full-SR/RAM calls
  pass; controlled coverage is 546/550, original-child union
  546/550. Four cold NEG PCs have 65,536 separate
  production-segment calls. Dispatch passes 113,664 calls; each mode has
  zero hardware-bearing /37,888 hardware-free classifications and 6,912
  actual Custom packets. Generated children resume hardware-event yields;
  the retained initial harness failure confused that yield with failure.
  Thirty seeded owners pass normal C: 537,534 shadow /
  571,393 sandbox. Seven cold source-only callbacks have
  zero recording calls and retain the unchanged generic rejection; their
  complete-call/dispatch fixture proofs are separate. All twelve older
  helpers pass normal-C regression at 423,745/530,198.
  Full 600-row gate passes 568,446/443,870,
  zero mismatches, exact seals and identical poison frames. All 36,236
  isolated live frames/seals match. Local DMA passes 550/17,600; fresh
  combined DMA passes 28,931/925,792, with all 550 PCs in the previous
  union. All thirty-six older generators and shared core/observers are
  unchanged. GNU/MSVC Release pass; build/ is 1.87 GiB.
  Family frames match through 600; ALL remains 424/34,144. See
  analysis/routines/native_c_render_entry_helpers.md and
  analysis/figures/native_render_entry_helpers_checkpoint.json.
  Those selected-segment/projection/crossing owners are now complete above. Larger render parents, older partial adapters,
  C2C392, C1612C graphics-wait integration and the full original cold/
  indirect/callback graph remain game-function work. Goal is not complete.

- Twelve complete renderer-helper upgrades retain 526 translated plus
  sixty-six source-only entries: 592 rows, now 510 source-timed (444
  translated plus sixty-six source-only). The batch owns 971 unique / 392
  shared boundaries and thirteen distinct / sixteen per-owner child sites.
  All 393,216 complete CPU/PC/full-SR/all-RAM calls pass. Controlled whole
  calls cover 964/971; four NEG arms and three first-plane busy-loop PCs
  have 81,920 independent production-segment calls. Original-child union
  is 947/971, with every missing PC explicit. Ordered Custom writes and
  terminal hardware/latches match using real device clocks and initial
  blits; fixture packet totals remain explicit. Original children execute
  actual source bytes in whole-C proofs; actual dispatch independently uses
  production generated-child continuations. All 36,864 dispatch calls pass;
  each mode has zero hardware-bearing / 12,288 hardware-free classifications
  and 90,368 actual Custom writes. Sandbox plane baseline uses the original
  suppressed-write policy and commits original logged packets at return.
  Guard checks prove admission/counting, not completion of a generated cold
  source continuation. Normal C passes every owner: 423,745 shadow /
  530,198 sandbox; the generic checker is unchanged. All 36,236 isolated
  live frames and seals match. Full 592-row gate passes 568,446 shadow /
  443,870 sandbox, exact seals and poison frames. Original busy-read opt-ins
  are restored; retained failed gates had a one-byte C45927 counter drift.
  Local DMA passes 971 /31,072; fresh combined DMA and independent union
  pass 28,931 /925,792 (previous 28,849 plus 971, overlap 889). All thirty-five
  older generators and shared core/observers remain unchanged. GNU/MSVC
  Release pass; build/ is 1.816 GiB. Family exact through 600; ALL remains
  424 /34,144 pixels. Remaining exports are unchanged; unused
  glue_active_planes.c callbacks are removed. See
  analysis/routines/native_c_render_leaf_helpers.md and
  analysis/figures/native_render_leaf_helpers_checkpoint.json.
  Next thirty-seven renderer entry helpers own 550 unique /235 shared
  boundaries, sealed in analysis/data/render_entry_helpers_scope_inventory.json:
  C2F688, square entries, C301F6, C330FE and all thirty-two original writer
  callbacks. Twenty-nine are registered upgrades, C2F688 is an unregistered
  seeded entry and seven callbacks have source-only table/jump evidence.
  Selected-segment indirect calls, older adapters, C2C392, C1612C graphics-wait
  integration and the full original cold/indirect/callback graph remain
  game-function work. Goal is not complete.

- 526 translated plus sixty-six source-only entries remain registered:
  592 rows, 509 source-timed (443 translated plus sixty-six source-only).
  Fifteen complete ground/HUD rendering upgrades own 1,352 unique / zero
  shared boundaries and sixty original child sites. All 491,520 whole CPU/
  PC/full-SR/all-RAM calls pass. Controlled whole-entry coverage is 1,350/
  1,352; actual production segments independently cover cold C3429E/C32380
  in 32,768 additional calls. Original-child and dispatch missing PCs remain
  explicit in the checkpoint. Ordered Custom packets and terminal hardware
  match; controlled direct-blit waits use actual initial hardware blits,
  including five fixture packets per odd scenario, not canned busy reads.
  Actual dispatch passes 46,080 calls and all guards; each mode has zero
  hardware-bearing / 15,360 hardware-free classifications and 22,848 writes.
  Normal C passes every owner: 30,293 shadow / 42,601 sandbox; generic
  contracts and positive-owner rejection remain unchanged. All 36,236 isolated
  live frames and RAM seals match. Shared core/observer and earlier domains
  are unchanged; all thirty-four older generators are byte-identical. Local
  DMA passes 1,352 / 43,264; fresh combined DMA and independent union pass
  28,849 / 923,168 (previous 27,543 plus 1,352, overlap 46). GNU/MSVC Release
  pass; build/ is 1.74 GiB. Full 592-row gate passes 568,446 shadow /
  443,870 sandbox, zero mismatches, exact seals and poison frames. Family
  exact through 600; ALL now differs at 424 / 34,144 pixels, versus previous
  416/361; retained as deferred timing evidence. Remaining older export bodies
  are unchanged. See analysis/routines/native_c_hud_render_parents.md and
  analysis/figures/native_hud_render_parents_checkpoint.json. Those twelve
  renderer helpers are now complete as described above, with original callers
  and both sixteen-entry pixel-writer tables sealed in
  analysis/data/render_leaf_helpers_scope_inventory.json. Selected-segment
  indirect calls, older adapters, C2C392 computed transfer, C1612C graphics-wait
  integration and the full original cold/indirect/callback graph remain
  game-function work. Goal is not complete.

- 526 translated plus sixty-six source-only entries remain registered:
  592 rows, 494 source-timed (428 translated plus sixty-six source-only).
  Fourteen complete face-list/edge upgrades own 533 unique / 294 shared
  boundaries and twelve distinct / nineteen per-owner child sites, with
  original direct/indirect callers sealed. All 458,752 whole CPU/PC/full-SR/
  all-RAM calls pass; both controlled and original-child calls cover every
  owned boundary for every owner. Frozen-clock geometry makes no active
  fill/outline/DMA claim; zero Custom writes and exact terminal hardware.
  Actual dispatch passes 43,008 calls, all guards and strict classification:
  six hardware-bearing / 14,330 hardware-free per mode. Normal C passes
  every owner: 70,334 shadow / 92,495 sandbox. The initial generic selection's
  C21C4C zero-call rejection remains sealed; independent selection passes
  165/166 comparisons. Generic rejection is unchanged. All 36,236 isolated
  live frames and RAM seals match. Shared core and earlier completed domains
  are unchanged; all 33 older generator outputs are byte-identical. Local
  DMA passes 533 / 17,056; fresh combined DMA and independent union pass
  27,543 / 881,376 (previous 27,064 plus 533, overlap 54). GNU/MSVC Release
  pass; build/ is 1.676 GiB. Full 592-row gate passes 568,446 shadow /
  443,868 sandbox, zero mismatches, exact seals and poison frames. Family
  exact through 600; ALL remains 416/361. Old batches 57/58/66 are removed;
  C21060 is now complete; remaining exports in batches 25/47/48/49 are
  byte-identical. See analysis/routines/native_c_face_list_parents.md and
  analysis/figures/native_face_list_parents_checkpoint.json for exact seals.
  Those fifteen ground/HUD parents are now complete as described above.
  Selected-segment
  indirect calls, older adapters, C2C392 computed transfer, C1612C graphics-
  wait integration and the full original cold/indirect/callback graph remain
  game-function work. Goal is not complete.

- 526 translated of the seeded 624 plus sixty-six source-only entries remain
  registered: 592 rows, 481 source-timed (415 translated plus sixty-six
  source-only). Seventeen complete face-stream upgrades own 773 unique /
  99 shared boundaries and thirteen distinct / fifteen per-owner child
  sites, with original direct/indirect callers sealed. All 557,056 whole
  CPU/PC/full-SR/all-RAM calls pass; both controlled and original-child
  calls cover every boundary for every owner. Frozen-clock fixtures use
  degenerate geometry or original behind-view rejection and make no active
  fill/outline/DMA claim. Ordered packets and terminal hardware match with
  zero fixture writes. Actual dispatch passes 52,224 calls, exact
  classification, complete boundary coverage and all guards. Normal C
  passes every owner: 22,333 shadow / 26,010 sandbox. All 36,236 isolated
  live frames and RAM seals match. Shared runtime/CPU/bus/arithmetic/
  classification/observer and earlier completed domains are unchanged;
  all 32 older generator families are byte-identical. Local DMA passes
  773 / 24,736; fresh combined DMA and independent union pass 27,064 /
  866,048 (previous 26,324 plus 773, overlap 33). GNU/MSVC Release pass;
  build/ is 1.637 GiB. Full 592-row gate passes 568,446 shadow /
  443,868 sandbox, zero mismatches, exact seals and poison frames.
  Family exact through 600; ALL remains 416/361. Old batches 53/54 are
  removed; C21060 in batch57 remains byte-identical and still needs complete
  integration. See analysis/routines/native_c_face_stream_parents.md and
  analysis/figures/native_face_stream_parents_checkpoint.json for exact
  seals and per-recording classifications. Next fourteen tested/list/grid/
  lattice face and edge owners have 533 unique / 294 shared boundaries and
  original callers sealed in analysis/data/face_list_parents_scope_inventory.json.
  Selected-segment indirect calls, older adapters, C2C392 computed transfer,
  C1612C graphics-wait integration and the complete original cold/indirect/
  callback graph remain game-function work. Goal is not complete.

- 526 translated of the seeded 624 plus sixty-six source-only entries remain
  registered: 592 rows, 465 source-timed (399 translated plus sixty-six
  source-only). Ten complete older rendering-parent upgrades own 1,101
  unique / zero shared boundaries and sixteen original child sites, with
  direct and six indirect table/caller edges sealed. All 327,680 whole
  CPU/PC/full-SR/all-RAM calls pass; controlled contracts cover every owned
  boundary. Face-side, record-shadow and all vertex paths have complete
  original-child coverage; other exact missing PCs and original zero-view,
  degenerate-geometry and last-row bounds remain explicit. Frozen-clock
  fixtures make no active shape/mask/fill/outline/DMA claim. Ordered packets
  and terminal hardware match with zero fixture writes. Actual dispatch
  passes 30,720 bounded calls, exact classification and all guards. Normal
  C passes every owner: 57,634 shadow / 62,893 sandbox. All 36,236 isolated
  live frames and RAM seals match. Shared runtime/CPU/bus/arithmetic/
  classification/observer and earlier completed HUD domains are unchanged;
  all 31 older generator families are byte-identical. Local DMA passes
  1,101 / 35,232; fresh combined DMA and independent union pass 26,324 /
  842,368 (previous 25,346 plus 1,101, overlap 123 from C1FB82). GNU/MSVC
  Release pass; build/ is 1.599 GiB. Full 592-row gate passes 568,446
  shadow / 443,868 sandbox, zero mismatches, exact seals and poison
  frames. Family exact through 600; ALL remains 416/361. See analysis/
  routines/native_c_render_parents.md and analysis/figures/
  native_render_parents_checkpoint.json for exact hashes, classifications,
  per-owner coverage limits and retained initial failures. Next seventeen
  segment/face/grid/block stream parents have 773 unique / 99 shared
  boundaries and original callers sealed in analysis/data/
  face_stream_parents_scope_inventory.json. Other older adapters, C2C392
  computed transfer, C1612C graphics-wait integration and full original
  cold/indirect/callback coverage remain open. No service or timing-only
  work is selected.

- 526 of 624 translated entries plus sixty-six source-only entries remain
  registered: 592 rows, 456 source-timed (390 translated plus sixty-six
  source-only). Complete C0D04C/C33370/C1FE24/C1FE46/C0CF98 upgrades own
  643 unique / zero shared boundaries and 48 original child sites. Original
  direct callers and three exact indirect C1F942 table/observed return edges
  are sealed. All 163,840 whole CPU/PC/full-SR/all-RAM calls pass; controlled
  contracts cover every owned boundary. Original-child coverage is 232/237,
  165/382 and 8/8 for each stream owner, with original behind-view, numeric
  record-class and text-column bounds. Analog paths are controlled-contract
  proofs; frozen-clock fixtures make no active glyph/fault/DMA claim.
  Ordered packets and terminal hardware match with zero fixture writes.
  Actual dispatch passes 15,360 bounded calls, exact classification and all
  guards, using the original indirect caller for stream entries. Independent
  normal C passes all five: 5,901 shadow / 8,073 sandbox. All 36,236 isolated
  live frames and RAM seals match. Shared runtime/CPU/bus/arithmetic/
  classification/observer and earlier completed HUD domains are unchanged;
  all 30 older generator families are byte-identical. Local DMA passes 643 /
  20,576; fresh combined DMA and independent union pass 25,346 / 811,072
  (previous 24,703 plus 643, no overlap). GNU/MSVC Release pass; build/
  is 1.562 GiB. Full 592-row gate passes 568,446 shadow / 443,868 sandbox,
  zero mismatches, exact seals and poison frames. Family exact through 600;
  ALL remains 416/361. See analysis/routines/native_c_hud_history_stream.md
  and analysis/figures/native_hud_history_stream_checkpoint.json for exact
  hashes, classifications, coverage limits and retained fixture build failure.
  Next ten older render parents have 1,101 unique / zero shared boundaries
  and original direct/six indirect callers sealed in analysis/data/
  legacy_render_parents_scope_inventory.json. C2C392 computed transfer,
  C1612C graphics-wait integration and full original cold/indirect/callback
  coverage remain open. No service or timing-only work is selected.

- 526 of 624 translated entries plus sixty-six source-only callable entries
  remain registered: 592 rows and 451 source-timed entries (385 translated,
  sixty-six source-only). Complete upgrades C0DAEE/C0CFFA/C33CD2/C33B38/
  C332BC/C32662 own 279 unique / zero shared boundaries, all original calls
  and twenty distinct child sites. All 196,608 whole CPU/PC/full-SR/all-RAM
  calls pass. Controlled contracts cover every owner and all 279 boundaries.
  Original-child coverage is 32/32, 26/29, 67/67, 24/90, 5/16 and 22/45,
  using original behind-view, text-column and context bounds with clock held.
  Active drawing/fault/DMA is not claimed by those bounded fixtures.
  Ordered packets and terminal hardware match; these fixtures have zero writes.
  Actual ON/shadow/sandbox passes 18,432 bounded fixtures with exact
  classification and all guards. Normal C passes all six: 8,780 shadow /
  13,276 sandbox comparisons. All 36,236 isolated live frames and RAM seals
  match. Shared runtime/CPU/bus/arithmetic/classification/observer are unchanged;
  all 29 older generator families are byte-identical. Local DMA passes 279 /
  8,928; fresh combined DMA and independent union pass 24,703 / 790,496
  (previous 24,511 plus 279, overlap 87). GNU/MSVC Release pass; build/
  is 1.524 GiB. Full 592-row gate passes 568,446 shadow / 443,868 sandbox
  matches, zero mismatches, exact seals and poison frames. Family exact through
  frame 600; ALL remains 416/361. See analysis/routines/
  native_c_hud_projection_parents.md and analysis/figures/
  native_hud_projection_parents_checkpoint.json for exact hashes, classifications,
  per-entry proof limits and retained initial build failure.
  Next complete C0D04C history projection and C33370 postflight display:
  619 unique / zero shared boundaries and original calls in
  analysis/data/hud_history_display_scope_inventory.json. Older rendering
  parents, C1FE24/C1FE46/C0CF98 indirect/table callability, C2C392 computed
  transfer, C1612C graphics-wait integration and full original game-call/
  callback coverage remain open. No service or timing-only work is selected.

- 526 of 624 translated entries plus sixty-six source-only callable entries
  remain registered: 592 rows and 447 source-timed entries (381 translated,
  sixty-six source-only). Complete upgrades C31C20/C3271A/C32726/C32736/
  C32794/C32AA4/C32AA6/C32AB4 own 182 unique / 141 shared boundaries,
  with original calls and six distinct / twenty per-owner child sites sealed.
  All 262,144 whole calls pass full CPU/PC/SR/all-RAM: 16,384 controlled
  and 16,384 original-child calls per owner. Controlled coverage is 21/21,
  64/64, 56/63, 65/65, 39/39, 83/83, 82/82 and 66/66; shared union
  182/182. Fixed C32726 zero mode excludes seven static leading-blank PCs,
  covered by sibling formatters. Original-child fixtures use original clipped
  columns/layouts and even destinations: 21/21, 41/64, 33/63, 42/65, 16/39,
  39/83, 38/82 and 22/66. Active glyph, fault and DMA paths are not claimed
  by these bounded fixtures. Ordered Custom packets and terminal hardware
  match; these fixtures have zero writes. Actual ON/shadow/sandbox passes
  24,576 completed bounded fixtures, exact hardware-free classification and
  all guards. Independent normal C passes all eight: 30,815 shadow / 32,589
  sandbox matches. All 36,236 isolated live frames and RAM seals match.
  Drawing source-first DMACONR contracts remain. Shared CPU/bus/arithmetic,
  classification, runtime and hardware observer are unchanged; all 28 older
  generator outputs are byte-identical. Local DMA passes 182 / 5,824;
  fresh combined DMA and independent union pass 24,511 / 784,352 (previous
  24,478 plus 182, overlap 149). GNU/MSVC Release pass; build/ is 1.487 GiB.
  Full 592-row gate passes 568,440 shadow / 443,868 sandbox matches, zero
  mismatches, exact seals and poison frames. Family exact through frame 600;
  ALL remains 416/361. See analysis/routines/native_c_hud_text_helpers.md
  and analysis/figures/native_hud_text_helpers_checkpoint.json for hashes,
  classifications, per-entry coverage and limits.
  Next complete C0DAEE/C0CFFA/C0D04C/C33CD2/C33B38/C33370/C332BC/C32662:
  898 unique / zero shared boundaries and direct original calls are sealed in
  analysis/data/legacy_hud_projection_scope_inventory.json. Older C1FE24/
  C1FE46/C0CF98 dispatch owners need separate indirect/table callability
  seals. Other older rendering/HUD parents, C2C392's computed transfer,
  C1612C graphics-wait integration and full original game-call/callback
  coverage remain open. No service replacement or timing-only work is selected.

- 526 of 624 translated entries plus sixty-six source-only callable entries
  remain registered: 592 rows and 439 source-timed entries (373 translated,
  sixty-six source-only). Complete upgrades C31F4C/C3201A/C3212A/C32178/
  C321D2/C32260/C31EB6/C31C60/C31D16/C31E6C/C31D64/C33F54/C328A8 own
  606 unique / 70 shared boundaries, with original incoming calls and 31
  distinct / 43 per-owner child sites sealed. All 425,984 whole calls pass
  full CPU/PC/SR/all-RAM: 16,384 controlled and 16,384 original-child calls
  per owner. Controlled coverage is 120/120, 135/135, 84/84, 88/88, 98/98,
  98/98, 36/36, 48/48, 23/23, 18/18, 64/64, 14/14 and 143/143; union
  606/606. Original-child fixtures use original clipped bounds and context
  select zero. Coverage is 71/120, 88/135, 60/84, 64/88, 75/98, 75/98,
  3/36, 48/48, 23/23, 18/18, 64/64, 14/14 and 120/143. They do not prove
  context, active DMA, glyph or fault paths. Ordered Custom packets and
  terminal hardware state match; these bounded fixtures have zero writes.
  Actual ON/shadow/sandbox passes 39,936 completed bounded fixtures with
  exact hardware-free classification and all guards. Independent normal C
  passes every owner: 32,575 shadow / 33,952 sandbox matches. Active drawing
  is independently checked on all 36,236 isolated live frames and RAM seals.
  Existing source-first DMACONR contracts remain. Shared CPU/bus/arithmetic,
  classification, runtime and test observer are unchanged; all 27 older
  generator outputs are byte-identical. Local DMA passes 606 / 19,392;
  fresh combined DMA and independent union pass 24,478 / 783,296 (previous
  23,942 plus 606, overlap 70). GNU/MSVC Release pass; build/ is 1.45 GiB.
  Full 592-row gate passes 568,440 shadow / 443,868 sandbox matches, zero
  mismatches, exact seals and poison frames. Family exact through frame 600;
  ALL remains 416/361. See analysis/routines/native_c_hud_readout_parents.md
  and analysis/figures/native_hud_readout_parents_checkpoint.json for exact
  hashes, per-owner coverage, retained build failures and proof limits.
  Next complete eight cache/text helpers C31C20/C3271A/C32726/C32736/
  C32794/C32AA4/C32AA6/C32AB4: 182 unique / 141 shared boundaries and
  original incoming calls in analysis/data/hud_text_helper_scope_inventory.json.
  Older projection parents, remaining HUD helpers, C2C392's computed transfer,
  C1612C graphics-wait integration and full original game-call/callback coverage
  remain open. No service replacement or timing-only work is selected.

- 526 of 624 translated entries plus sixty-six source-only callable entries
  remain registered: 592 rows and 426 source-timed entries (360 translated,
  sixty-six source-only). Complete upgrades C30764/C309B6/C30B5C/C30D34/
  C30F78/C3112A/C31A64/C31ACC own 595 unique / 63 shared boundaries,
  with original incoming calls and 43 distinct / 45 per-owner child sites sealed.
  Final 262,144 whole CPU/PC/full-SR/all-RAM calls pass: 16,384 controlled
  and 16,384 original-child calls per owner. Controlled coverage is 108/108,
  12/12, 110/110, 94/94, 82/82, 59/59, 91/91 and 102/102; union 595/595.
  The first strict union rejection at 592/595 remains evidence. Independent
  status flag inputs cover the missing three PCs without changing production.
  Frozen-clock original-child fixtures use explicit no-draw bounds and cover
  3/108, 12/12, 9/110, 11/94, 28/82, 12/59, 3/91 and 3/102. Ordered Custom
  packets and terminal hardware state match; all these bounded fixtures have
  zero writes. Active drawing is independently verified on the recordings.
  Actual ON/shadow/sandbox passes 6,144 bounded completed fixtures with exact
  hardware-free classification and all guards. Independent normal C passes
  all eight owners: 27,001 shadow / 41,163 sandbox comparisons. Existing
  source-first DMACONR contracts remain. Shared runtime, CPU/bus/arithmetic,
  classification and test hardware observer are unchanged; all 26 older
  generator outputs are byte-identical. Local DMA passes 595 / 19,040;
  fresh combined DMA and independent union pass 23,942 / 766,144 (previous
  23,347 plus 595, no overlap). GNU/MSVC Release pass; build/ is 1.413 GiB.
  Full 592-row gate passes 568,440 shadow / 443,868 sandbox matches, zero
  mismatches, exact seals and poison frames. All 36,236 isolated live frames
  and RAM seals match. Family exact through frame 600; ALL remains 416/361.
  See analysis/routines/native_c_hud_parents.md and
  analysis/figures/native_hud_parents_checkpoint.json for exact hashes,
  retained failures, per-owner coverage and proof limits.
  Next complete thirteen readout, cue and status parents:
  C31F4C/C3201A/C3212A/C32178/C321D2/C32260/C31EB6/C31C60/C31D16/C31E6C/
  C31D64/C33F54/C328A8. Their 606 unique / 70 shared boundaries and original
  incoming calls are sealed in analysis/data/hud_readout_parent_scope_inventory.json.
  Older projection parents and HUD helpers, C2C392's computed transfer,
  C1612C graphics-wait integration and complete original game-call/callback
  coverage remain open. No service replacement or timing-only work is selected.

- 526 of 624 translated entries plus sixty-six source-only callable entries
  are registered: 592 rows and 418 source-timed entries (352 translated,
  sixty-six source-only). Complete new C308D8/C308F4/C30F46/C31B76/C33AD6/
  C33B06 and upgrades C308E2/C30904 own 100 unique / 10 shared boundaries,
  with original incoming calls and all 11 child sites sealed. All 262,144
  whole calls pass full CPU/PC/SR/all-RAM, ordered Custom-write packets and
  terminal hardware/latch state: 16,384 controlled and 16,384 original-child
  calls per owner. Both kinds cover every owner's 5/5, 5/5, 8/8, 11/11,
  13/13, 42/42, 13/13 and 13/13 PCs in upgrade-then-new order. Shared union
  is 100/100. Actual ON/shadow/sandbox passes 393,216 completed fixtures,
  all owner PCs, exact classification, ordered hardware writes and guards.
  The original-child markers use bounded vertical offsets and zero or one
  enabled plane; controlled contracts retain all signed-word edges. Original
  ROM exceptions and frozen-clock multi-plane waits remain explicit limits.
  A test-only copy renames only the original Custom-write definition for
  observation; original private baseline state is restored before each side.
  Production machine/bus/blitter, shared arithmetic and classification code
  are unchanged. Store upgrades preserve the caller's A0 base and full CPU
  effects. Six new rows retain the existing source-first DMACONR contract.
  Independent normal C passes 9,146 shadow / 9,278 sandbox matches for five
  store owners. The three numeric/marker owners are cold in all recordings,
  including with store owners omitted; both strict zero-comparison rejections
  remain explicit. Local DMA passes 100 / 3,200; fresh combined DMA and the
  independent union pass 23,347 / 747,104 (previous 23,247 plus 100, no
  overlap). All 25 older generator outputs and shared proofs are unchanged.
  GNU/MSVC Release pass; build/ is 1.377 GiB. The full 592-row gate passes 568,440 shadow /
  439,147 sandbox matches, zero mismatches, exact seals and poison frames.
  All 36,236 isolated live frames and RAM seals match. Family exact through
  frame 600; ALL remains 416/361. See analysis/routines/native_c_hud_stream.md
  and analysis/figures/native_hud_stream_checkpoint.json for build size,
  hashes, hardware counts, all retained failures and bounded proof limits.
  Next reconstruct C30764/C309B6/C30B5C/C30D34/C30F78/C3112A/C31A64/C31ACC:
  595 unique / 63 shared boundaries and actual original incoming calls are
  sealed in analysis/data/hud_parent_scope_inventory.json. Shared digit and
  fault tails belong to those complete parents. Older projection parents,
  C2C392's computed transfer, C1612C graphics-wait integration and complete
  original game-call/callback coverage remain open. No service replacement
  or timing-only work is selected.

- 520 of 624 translated entries plus sixty-six source-only callable entries
  are registered: 586 rows and 410 source-timed entries (344 translated,
  sixty-six source-only). Complete new C2ECA8/C32A44/C32AC8/C33F70/C33F8A/
  C33FB4 and upgrades C2EC90/C2EC94/C2EC9C/C2ECA4 own 272 unique / 163 shared
  boundaries, with original incoming calls and 16 distinct / 41 per-owner
  child sites sealed. All 327,680 whole calls pass full CPU/PC/SR/all-RAM:
  16,384 controlled and 16,384 original-child calls per owner. Per-owner
  coverage is 59/80, 76/80, 60/80, 57/80, 54/79, 104/110, 83/83, 14/14,
  13/13 and 50/50 in that upgrade-then-new order, for both proof kinds.
  The shared whole-call union is 268/272. Original entry bounds exclude four
  upper-clamp PCs; 65,536 internal segment comparisons from C2ECD2/C2ECE4
  prove the actual production helpers and complete 272/272 shared coverage.
  These segments are not new functions or whole-entry coverage claims; the
  initial strict union rejection remains evidence. Fixed modes exclude
  sibling mode paths; the label's six leading-blank PCs are covered by the
  shared packed-readout owner. Actual ON/shadow/sandbox passes 491,520
  hardware-free completed fixtures, exact classification and all guards.
  Normal C passes 10,312 shadow / 10,360 sandbox matches for the four
  upgrades. All six new owners are cold in all three recordings, including
  with projectors omitted; both strict zero-comparison rejections remain
  explicit. Drawing rows retain the source-first DMACONR proof contract.
  Local DMA passes 272 / 8,704; fresh combined DMA and independent union
  pass 23,247 / 743,904 (previous 23,061 plus 272, overlap 86). All 24 older
  generator outputs and shared CPU/bus/math/classification proofs are
  unchanged. GNU/MSVC Release pass; build/ is 1.339 GiB. The full 586-row
  gate passes 568,155 shadow / 439,147 sandbox matches, zero mismatches,
  exact seals and poison frames. All 36,236 isolated live frames and RAM
  seals match. Family exact through frame 600; ALL remains 416/361. See
  analysis/routines/native_c_projection_readouts.md and
  analysis/figures/native_projection_readouts_checkpoint.json.
  Next reconstruct C308D8/C308F4/C30F46/C31B76/C33AD6/C33B06, 100 unique /
  zero shared boundaries, sealed with actual incoming calls in
  analysis/data/hud_stream_scope_inventory.json. Related existing C308E2/
  C30904 stores need complete CPU/A0-base upgrades in that batch. Older
  drawing parents still use legacy projection replay and require complete
  original-child/CPU upgrades. Complete original game-call/callback coverage,
  C2C392's computed transfer and C1612C graphics-wait integration remain
  open. No service replacement or timing-only work is selected.

- 514 of 624 translated entries plus sixty-six source-only callable entries
  are registered: 580 rows and 404 source-timed entries (338 translated,
  sixty-six source-only). Complete C2AFFA/C2B3C2/C2B564/C2B928/C2B952 own
  468 unique / 32 shared boundaries; actual incoming calls and 17 distinct /
  18 per-owner child sites are sealed. All 163,840 completed calls pass full
  CPU/PC/SR/all-RAM: 16,384 controlled and 16,384 original-child calls per
  owner. Controlled coverage is 34/34, 124/124, 167/167, 40/40 and 135/135;
  original-child coverage is 34/34, 120/124, 132/167, 40/40 and 129/135.
  Complete child-entry contracts preserve CPU/SR/RAM and return changed
  values, flags, cursors and RAM. Raw grid/record traces include nested point
  and marker PCs; coverage intersects each sealed owner scope. Actual ON/
  shadow/sandbox dispatch passes 30,720 hardware-free completed fixtures and
  all guards; bounded owner coverage is 34/34, 120/124, 132/167, 40/40 and
  121/135. Independent normal C passes 20,090 shadow / 20,093 sandbox matches
  for the two recorded parents. The other three owners have zero calls in
  all three recordings, including with parents omitted; strict generic
  zero-comparison rejections remain explicit. Drawing registrations retain
  the existing source-first DMACONR proof contract. Omitting it failed the
  carrier RAM seal; the final registrations pass it exactly, without changing
  shared runtime/bus/classification code. Local DMA passes 468 / 14,976.
  Fresh combined DMA and independently derived union pass 23,061 / 737,952:
  previous 22,593 plus all 468, no overlap. All twenty-three older generator
  outputs and shared CPU/bus/math/classification proofs remain unchanged.
  GNU/MSVC Release pass; build/ is 1.301 GiB. The full 580-row gate passes 568,155 shadow /
  439,147 sandbox matches, zero mismatches, exact seals and poison frames.
  All 36,236 isolated live frames and RAM seals match. Family exact through
  frame 600; ALL remains 416/361. See
  analysis/routines/native_c_flight_markers.md and
  analysis/figures/native_flight_markers_checkpoint.json.
  Next reconstruct C2ECA8/C32A44/C32AC8/C33F70/C33F8A/C33FB4, 264 unique /
  85 shared boundaries, sealed with actual incoming calls in
  analysis/data/projection_readout_scope_inventory.json. Complete original
  game-call/callback coverage remains open; C2C392's computed transfer and
  C1612C graphics-wait integration remain separate. No service or timing-only
  work is selected.

- 509 of 624 translated entries plus sixty-six source-only callable entries
  remain registered: 575 rows and 399 source-timed entries (333 translated,
  sixty-six source-only). Complete upgrades C2651E/C28E28/C26EBE/C27456 own
  1,015 unique / zero shared boundaries; actual incoming calls and four child
  sites are sealed. All 131,072 completed calls pass full CPU/PC/SR/all-RAM:
  16,384 controlled and 16,384 actual-child calls per owner. Controlled owner
  coverage is 50/50, 79/79, 819/819 and 67/67; actual-child coverage is 50/50,
  79/79, 816/819 and 67/67. The candidate raw trace includes nested face PCs.
  Complete child-entry contracts preserve CPU/SR/RAM and return changed values,
  flags, cursors and RAM. An explicit controlled RAM return changes the saved
  pass index and polygon terminator to exercise the parent's velocity-clear
  arm; the original C27456 is not claimed to publish those writes. Actual
  ON/shadow/sandbox dispatch passes 3,072 hardware-free complete fixtures and
  all guards; bounded owner coverage is 47/50, 77/79, 702/819 and 67/67.
  Independent normal C passes 16,826 shadow / 17,899 sandbox comparisons for
  the four-owner selection. A separate C27456 selection, with its parent
  omitted, passes 1,505 / 1,564. Hardware/incomplete classifications remain
  explicit. Local DMA passes 1,015 / 32,480. Fresh combined and independently
  derived union pass 22,593 / 722,976: previous 21,578 plus all 1,015, with
  no overlap. All twenty-two older generator outputs and shared CPU/bus/math/
  classification proofs remain unchanged. GNU/MSVC Release pass. The full
  575-row gate passes 559,969 shadow / 417,343 sandbox matches, zero mismatches,
  exact seals and poison frames. All 36,236 isolated live frames/seals match.
  Family exact through frame 600; ALL remains 416/361. build/ is 1.260 GiB. See
  analysis/routines/native_c_flight_geometry.md and
  analysis/figures/native_flight_geometry_checkpoint.json.
  Next reconstruct C2AFFA/C2B3C2/C2B564/C2B928/C2B952, 468 unique / 32 shared
  boundaries, sealed with actual incoming calls in
  analysis/data/flight_action_setup_scope_inventory.json. These five are
  unregistered translated entries. Related C2C392 still requires its signed
  action-byte computed transfer at C2C46E reconciled. Complete original
  game-call/callback coverage remains open; no service or timing-only work
  is selected.

- 509 of 624 translated entries plus sixty-six original source-only callable
  entries are registered: 575 rows and 395 source-timed entries (329 translated,
  sixty-six source-only). Complete C25B66/C266AE/C28996/C28B16 and upgraded
  C28B34 own 1,308 unique / 218 shared boundaries; all actual incoming calls
  and 25 distinct / 27 per-owner child sites are sealed. All 163,840 complete
  calls pass full CPU/PC/SR/all-RAM: 16,384 controlled and 16,384 real-child
  calls per owner. Controlled coverage is every owner's 554/554, 416/416,
  107/107, 231/231 and 218/218 PCs. Real coverage is 415/554, 416/416,
  107/107, 231/231 and 218/218; C28996's raw trace includes nested body PCs.
  Complete child-entry CPU/SR/RAM contracts return changed values, cursors
  and flags. The duplicate collision-class read arm uses an explicit test-only
  ordered publication contract; its original interrupt publisher is not
  claimed reconstructed. Frames, saved RAM values, mixed-width halves,
  stream order and signed ADD/SUB overflow branches are retained. Actual
  ON/shadow/sandbox dispatch passes 3,840 hardware-free complete fixtures
  and all guards. Independent normal C for C25B66/C28996/C28B34 passes
  5,711 shadow / 14,168 sandbox matches. Hardware/incomplete rows remain
  recorded. C266AE and C28B16 are cold in all six independent reports each,
  with parents omitted; both strict generic zero-comparison rejections remain.
  Local DMA passes 1,308 / 41,856. Fresh combined and independently derived
  union pass 21,578 / 690,496: 20,488 + 1,308, overlapping 218 old C28B34 PCs.
  Twenty-one older generator outputs and shared production/proof files remain
  unchanged. GNU/MSVC Release pass. The full 575-row gate passes 559,969
  shadow / 417,343 sandbox matches, zero mismatches, exact seals and poison
  frames. All 36,236 isolated live frames/seals match. Family exact through
  frame 600; ALL remains 416/361. build/ is 1.211 GiB. See
  analysis/routines/native_c_flight_dynamics.md and
  analysis/figures/native_flight_dynamics_checkpoint.json.
  Its next inventory was complete C2651E/C28E28/C26EBE/C27456: 1,015 unique / zero
  shared boundaries and actual incoming calls, sealed in
  analysis/data/flight_geometry_remaining_scope_inventory.json and reproduced
  by tools/recomp/audit_flight_geometry_remaining.py. These four are already
  registered; their completed upgrades above retain the translated count. Related C2C392 still
  requires its signed action-byte computed transfer at C2C46E to be reconciled.
  Complete original game-call/callback coverage remains open. No service or
  timing-only work is selected.

- The preceding motion-helper checkpoint registered 505 of 624 translated
  entries plus sixty-six original source-only callable
  entries are registered: 571 rows and 391 source-timed entries (325 translated,
  sixty-six source-only). Complete C26322/C26352/C26C72/C26CC0/C26D8A own
  229 unique / zero shared boundaries and retain sealed original incoming
  JSR/BSR evidence. All 163,840 completed calls pass full CPU/PC/SR/all-RAM:
  16,384 controlled and 16,384 real-child calls per owner. Both layers cover
  every owner's 20/20, 20/20, 29/29, 61/61 and 99/99 PCs. The two original
  C27456 child sites retain complete input CPU/SR/RAM contracts and changed
  return flags, working values and cursors. Signed divide overflow, the
  minimum-long/-1 source result, partial writes, caller-frame locals, saved
  cursors and signed ADD overflow branches are preserved. Divide-zero still
  uses the actual exception backend; full Kickstart-handler return is unproven.
  Actual ON/shadow/sandbox dispatch passes 3,840 hardware-free completed
  fixtures and all OFF/selection/non-call/source-write guards. All five owners
  have zero calls in all six normal-C recording reports with their parents
  omitted. The unchanged generic checker rejects C26322 for zero completed
  comparisons; no recorded normal-C comparison is claimed for these cold
  owners. Local DMA passes 229 / 7,328. Fresh combined and independently
  derived union pass 20,488 / 655,616, adding all 229 PCs with no previous
  overlap. Twenty older generator outputs and shared production/proof files
  remain unchanged. GNU/MSVC Release pass. The full 571-row gate passes
  567,984 shadow / 417,363 sandbox matches, zero mismatches, exact seals and
  poison frames. All 36,236 isolated live frames/seals match. Family exact
  through frame 600; ALL remains 416/361. build/ is 1.172 GiB. See
  analysis/routines/native_c_flight_motion_helpers.md and
  analysis/figures/native_flight_motion_helpers_checkpoint.json.
  Its four enclosing owners and the shared stream are now complete in the
  latest checkpoint above; the earlier remaining inventory is historical
  source evidence. Complete original call/callback coverage remains open.

- The preceding flight-record checkpoint registered 500 translated entries
  plus sixty-six original source-only callable
  entries are registered: 566 rows and 386 source-timed entries (320 translated
  plus sixty-six source-only). Complete C230E8/C23116/C23186/C23228/C233AA/
  C23578/C236AA/C23716/C2377E/C257EC cover 575 unique / 245 shared boundaries
  and 30 per-owner / 20 distinct actual child sites. Every entry has sealed
  original incoming JSR/BSR evidence. 327,680 completed all-CPU/PC/full-SR/
  all-RAM calls pass: 163,840 controlled-child and 163,840 real-child.
  Controlled owner coverage is 32/32, 25/25, 8/8, 177/199, 105/105, 43/43,
  155/155, 115/142, 164/188 and 50/53. Real owner coverage is 22/32, 19/25,
  8/8, 110/199, 59/105, 43/43, 122/155, 115/142, 164/188 and 50/53.
  C23228's raw real 150 PCs include nested source; report its 110-PC owner
  intersection. The controlled completed-owner union is 550/575.
  No PC is removed from production ownership. The same-TST BNE/BEQ pair
  makes the 22-PC recording arm unreachable from the sealed entry. Its
  C23354 internal segment passes 1,024 cases / all 23 segment PCs.
  Valid disjoint-record release fixtures publish class 49 and action fixtures
  class 0/1: their 27/24 unobserved shared-tail PCs are separately covered by
  completed manoeuvre contracts and 1,024 C2385A internal segments / all 130
  tail PCs. Neither segment is a new callable owner. Zero scale retains the
  write-21/fault-child/repeating loop; 1,024 complete CPU/SR/RAM first-child
  boundary observations are not completed calls or replacement returns.
  The nonreturning C257EA back edge has independent instruction proof.
  Minimum-word scaling behavior and the actual divide-zero backend remain;
  full normalizer Kickstart-handler return is unproven.
  Actual ON/shadow/sandbox dispatch passes 7,680 completed hardware-free
  fixtures and all guards. The C257EC guard uses actual entry label 2, not
  its preceding fault label 0. Independent normal C passes 20,301 shadow /
  20,394 sandbox comparisons for C230E8/C23116/C23228; 86/7 shadow incompletes
  remain explicit, with no sandbox incompletes, hardware classifications or
  mismatches. Seven other entries have zero calls in the separate cold
  selection with those parents omitted. They retain fixture and original
  callability proof. Both generic zero-comparison rejections are retained;
  the unchanged hot-entry checker passes. Local DMA passes 575 / 18,400.
  Fresh combined and independent union pass 20,259 / 648,288, counting 171
  previous-overlap PCs once. Nineteen older generator outputs and shared
  production runtime/memory/bus/math/proof files remain unchanged.
  GNU/MSVC Release pass. The full 566-row gate passes 567,984 shadow /
  417,363 sandbox matches, zero mismatches, exact seals and poison frames.
  All 36,236 isolated live frames/seals match; family exact through frame
  600, ALL still 416/361. build/ is 1.137 GiB. See
  analysis/routines/native_c_flight_record_actions.md and
  analysis/figures/native_flight_record_actions_checkpoint.json.
  Its following inventory sealed C25B66/C26322/C26352/C266AE/C26C72/C26CC0/C26D8A/C28996/C28B16:
  1,537 unique / zero shared boundaries, sealed with original callability in
  analysis/data/flight_dynamics_scope_inventory.json. Reproduce its audit
  with tools/recomp/audit_flight_dynamics.py. Related C2C392 has actual call
  C25C6A but needs computed-transfer evidence at C2C46E before complete
  ownership can be sealed. The nine-owner inventory is not implementation.
  Remaining game functions are open; the goal has not reached the stop point.

- 490 of 624 translated entries plus sixty-six original source-only callable
  entries are registered: 556 rows and 376 source-timed entries (310 translated
  plus sixty-six source-only). Complete C149BE/C23A7E and upgraded existing
  C083E2/C25754/C24568/C2436A cover 1,416 unique / zero shared boundaries within
  this batch and 22 actual child sites. Four older adapters and their unused
  register helpers are removed. Full CPU/PC/SR/all-RAM proof passes 196,608
  complete cases without exclusions: 98,304 controlled-child and 98,304
  real-child. Controlled cases cover all 493/35/56/711/82/39 owned PCs and
  compare complete child-entry CPU/SR/RAM. Real owner coverage is 168/493,
  35/35, 55/56, 102/711, 70/82 and 39/39. C149BE's raw 217-PC observation
  includes nested owners; report only its 168-PC intersection. Real fixtures
  retain actual normalization/attenuation children and original class paths;
  no all-path real clock/sound/fault/projection-service completion is claimed.
  Test-only ordered word/byte read contracts prove the two original flag
  reloads. Minimum-word scale loop remains source behavior, not a completed
  whole-call fixture. DIVU-zero exception entry has instruction proof; the
  normalizer's complete ROM-handler return remains unproven.
  Actual ON/shadow/sandbox dispatch passes 4,608 complete fixtures and guards,
  requiring hardware-free completed classification. Independent normal C
  passes 15,468 shadow / 15,714 sandbox matches: parent checker 9,444 / 9,637,
  helper checker 6,024 / 6,077. C149BE retains five shadow / eight sandbox
  hardware classifications and 199 shadow incompletes; C25754 retains 53
  shadow incompletes. No mismatches or sandbox incompletes. The six-owner
  generic batch's C083E2 zero-comparison rejection and three absorbed-helper
  zero-count reports are retained; both separate unchanged checkers pass.
  Local DMA passes 1,416 / 45,312. A fresh combined run and the independently
  proven union pass 19,855 / 635,360, counting the 450 previous-overlap PCs
  once. All eighteen older generator outputs and shared runtime CPU/bus/
  memory/math/instruction fixtures remain unchanged. GNU/MSVC Release pass.
  The full 556-row gate passes 553,158 shadow / 417,328 sandbox matches, zero
  mismatches, exact seals and identical poison frames. Whole parents absorb
  nested calls, so the aggregate comparison count does not have to increase.
  All 36,236 isolated live frames/seals match; group exact through frame 600,
  ALL remains 416/361. Build/ is 1.100 GiB. C1612C remains inactive with frozen
  graphics-wait failure explicit. See
  analysis/routines/native_c_main_loop_flight_controls.md and
  analysis/figures/native_main_loop_flight_controls_checkpoint.json.
  Now completed in the leading checkpoint: C230E8/C23116/C23186/C23228/C233AA/C23578/C236AA/C23716/C2377E/
  C257EC: 575 unique / 245 shared boundaries, sealed in
  analysis/data/flight_record_actions_scope_inventory.json. Every owner has
  byte-backed original JSR/BSR evidence. This inventory implements none of
  them and replaces no OS service. Game-function porting remains incomplete;
  retain the stopping point and defer Kickstart and standalone timing work.
- At the preceding control-action checkpoint, 488 of 624 translated entries
  plus sixty-six original source-only callable
  entries are registered: 554 rows and 370 source-timed entries (304 translated
  plus sixty-six source-only). Complete C153FC/C15688/C159AE/C15AD4/C181A0 and
  the upgraded C15138 cover 537 unique / zero shared boundaries and ten actual
  child sites. The older offset adapter is removed; its caller-slot write,
  original frame locals and complete CPU/SR/source timing are retained.
  Full CPU/SR/all-RAM proof passes 147,456 complete cases without exclusions:
  49,152 controlled-child and 98,304 real-child cases. Controlled cases cover
  all 185/147/80/72/28/25 owned PCs and compare child-entry CPU/SR/RAM. Real
  owner coverage is 33/185, 108/147, 70/80, 72/72, 5/28 and 25/25. Advance uses
  the original expiry branch; alert uses the null guard. Launch/aim retain real
  direction/projection children, with nested owned PCs counted separately.
  No all-path real collision/render/sound-service completion is claimed.
  Actual ON/shadow/sandbox dispatch passes 4,608 complete fixtures and guards;
  all real fixtures require hardware-free completed classification.
  Five new owners are cold in all six step-disabled recording reports; the
  generic zero-comparison rejection for C153FC is retained unchanged. C15138
  independently passes 12,907 shadow / 12,926 sandbox normal-C matches, with
  19 shadow incompletes and no sandbox incompletes/hardware/mismatches. Its
  separate unchanged generic checker passes. Local DMA passes 537 / 17,184;
  the independently tested union is 18,889 / 604,448 with zero new overlap.
  Last fresh combined remains 16,384 / 524,288; no fresh combined run this
  batch. NEG.W memory and arithmetic-direction recipes are family-local;
  all seventeen older generator outputs and shared runtime CPU/bus/math/
  instruction fixtures remain unchanged. GNU/MSVC Release pass. The full
  554-row gate passes 555,565 shadow / 417,331 sandbox matches, zero mismatches,
  exact seals and identical poison frames. All 36,236 isolated live frames/
  seals match; group exact through frame 600, ALL remains 416/361. Build/ is
  1.061 GiB. C1612C remains inactive with frozen graphics-wait failure explicit.
  See analysis/routines/native_c_record_control_actions.md and
  analysis/figures/native_record_control_actions_checkpoint.json.
  Next complete C149BE/C23A7E control/flight parents and upgrade existing
  C083E2/C25754/C24568/C2436A: 1,416 unique / zero shared boundaries, sealed in
  analysis/data/main_loop_flight_controls_scope_inventory.json. That inventory
  implements none of these changes. Game-function porting remains incomplete;
  retain the stopping point and defer Kickstart and standalone timing work.
- At the preceding control/message checkpoint, 483 of 624 translated entries
  plus sixty-six original source-only callable
  entries are registered: 549 rows and 364 source-timed entries (298 translated
  plus sixty-six source-only). Complete C1518C/C32CEE owners cover 503 unique /
  zero shared boundaries and fifteen actual child sites. The message owner's
  earlier tails begin at C32BD2 and retain explicit ownership/start bounds.
  Full CPU/SR/all-RAM proof passes 49,152 complete cases without exclusions:
  16,384 controlled-child and 32,768 real-child cases. Controlled cases cover
  all 178/325 boundaries and compare every child-entry CPU/SR/RAM snapshot.
  Real coverage is partial at 57/178 and 273/325; the real control fixture keeps
  request/action children guarded off. No all-path real-service claim is made.
  Actual ON/shadow/sandbox dispatch passes 1,536 complete fixtures and guards;
  both real fixtures require hardware-free completed classification. Independent
  step-disabled readable C passes 19,397 shadow / 21,815 sandbox comparisons,
  with 153 control and 543 message shadow incompletes retained, zero hardware
  classifications/mismatches and no sandbox incompletes. The generic checker
  passes unchanged. Local DMA passes 503 / 16,096; the independently tested
  union is 18,352 / 587,264 with zero new overlap. Last fresh combined remains
  16,384 / 524,288; no fresh combined run this batch. Family-local LSR.W and
  existing DBRA expiry keep all sixteen older generator outputs unchanged;
  shared runtime CPU/bus/math/instruction fixtures remain unchanged.
  GNU/MSVC Release pass. The full 549-row gate passes 555,565 shadow / 417,331
  sandbox matches, zero mismatches, exact seals and identical poison frames.
  All 36,236 isolated live frames/seals match; group exact through frame 600,
  ALL remains 416/361. Build/ is 1.026 GiB. C1612C remains inactive with its
  frozen graphics-wait integration failure explicit. See
  analysis/routines/native_c_main_loop_control_messages.md and
  analysis/figures/native_main_loop_control_messages_checkpoint.json.
  Next complete six control-record/alert consumers C153FC/C15688/C159AE/
  C15AD4/C181A0/C15138: 537 unique / zero shared boundaries, sealed in
  analysis/data/record_control_actions_scope_inventory.json. C15138's existing
  fixed-charge adapter needs complete CPU/SR/source timing. The inventory
  implements none of these changes. Game-function porting remains incomplete;
  retain the user's stopping point and defer Kickstart and standalone timing.
- At the preceding timer/bounds checkpoint, 481 of 624 translated entries
  plus sixty-six original source-only callable
  entries are registered: 547 rows and 362 source-timed entries (296 translated
  plus sixty-six source-only). Complete C2527C/C25312/C2548A owners cover 157
  unique / zero shared boundaries and six actual child sites. The bounds owner
  is source-only; the timer adds a translated entry, and the existing readout
  gains full CPU/SR/source timing. Its 9999 path skips min/max as the source does.
  Full CPU/SR/all-RAM proof passes 73,728 complete cases without exclusions:
  24,576 controlled-child and 49,152 real-child cases. Controlled cases cover
  all 157 boundaries and compare child-entry CPU/SR/RAM. Real bounds/readout
  proof covers all 52/19 PCs; the timer's real fixture uses the original negative
  poll guard and covers 38/86 PCs. Its elapsed/poll loop is covered by controlled
  cases, not a completed real-service polling-loop proof. Production clock,
  countdown and normalization children retain original calls and return PCs.
  Actual ON/shadow/sandbox dispatch passes 2,304 complete fixtures and guards.
  Bounds/readout fixtures require hardware-free matches; timer fixtures actually
  touch hardware and require exact hardware classification. A forced zero
  divisor retains the original trap/ROM handler and exhausts a test-only
  100,000-instruction bound at FC30C2, exit 3. It is not a completed proof.
  Six raw step-disabled reports retain the cold C2527C row and generic zero-call
  rejection. Timer recording calls are 3,937 hardware + 146 incomplete in shadow
  and 4,230 hardware in sandbox, zero matches/mismatches. Readout body proof
  passes 504 shadow / 505 sandbox matches, with one incomplete shadow call.
  Local DMA passes 157 / 5,024; the independently tested union is 17,849 /
  571,168 with no new overlap. Last fresh combined remains 16,384 / 524,288;
  no fresh combined run this batch. New CPU recipes are family-local, all
  fifteen older generator outputs are unchanged, and shared runtime CPU/bus/
  math/instruction fixtures remain unchanged. GNU/MSVC Release pass. The full
  547-row gate passes 552,046 shadow / 413,271 sandbox matches, zero mismatches,
  exact seals and identical poison frames. All 36,236 isolated live frames/
  seals match; group exact through frame 600, ALL remains 416/361. Build/ is
  0.992 GiB. C1612C remains inactive with its prior failed frozen-reference
  integration explicit. See analysis/routines/native_c_main_loop_timers.md
  and analysis/figures/native_main_loop_timers_checkpoint.json.
  Next complete C1518C/C32CEE, 503 unique / zero shared boundaries, sealed in
  analysis/data/main_loop_control_messages_scope_inventory.json. That inventory
  implements neither owner. Game-function porting remains incomplete; retain
  the user's stopping point and defer Kickstart and standalone timing work.
- At the preceding input/display checkpoint, 480 of 624 translated entries
  plus sixty-five source-only callable entries were registered:
  545 rows and 359 source-timed entries (294 translated plus
  sixty-five source-only). Four complete gameport/text setup owners are added:
  C16D4C/C16FF4/C17066/C1787A. Each has an actual original call site.
  C1612C also has complete domain/CPU/step source but remains unregistered:
  its original graphics wait cannot complete inside frozen-event comparisons.
  The five implemented owners cover 388 unique / zero shared boundaries and
  retain 49 actual child sites, return PCs, arguments and original reloads.
  Full CPU/SR/all-RAM proof passes 73,728 complete cases without exclusions:
  40,960 controlled-child cases cover all five owners/all 388 boundaries;
  32,768 real-child cases cover C16D4C and C17066. The root's real fixture
  visits 51/92 owned PCs and all 26/19 nested controller/trigger PCs; this is
  partial root-path coverage, not complete real proof for error paths or the
  standalone controller owner. Controlled contracts cover the remaining
  paths, changed pointers, text descriptor reloads, palette/page/activity
  mutations and preserved child-entry RAM/CPU. Original diagnostic-exit
  children remain actual production calls; their test contracts returning
  do not prove the real error-service behavior.
  Actual ON/shadow/sandbox dispatch passes 1,536 fixtures for C16D4C/C17066,
  requiring native ON and completed hardware-free reference comparisons.
  Standalone C16FF4/C1612C fixtures exhaust an explicit test-only original
  instruction budget at AF3C90/C02776; C1787A stops at FC0FF0. None is counted
  as completed real or actual-dispatch proof. The budget wraps unchanged bus
  code only in these fixture builds and exits with failure, never a service
  result. Production runtime/OS/bus/liveness remain unchanged.
  All four registered owners are cold; six raw step-disabled recording
  reports and the generic C16D4C zero-comparison rejection remain retained.
  Temporary C1612C registration passed all 36,236 native ON frames and RAM
  seals; a separate 600-frame ON report proves 2,184 native calls. Its full
  and normal-call frozen-reference runs grew write logs and failed. The last
  live failing run was inspected (PPC FC5E90 -> PC C02776) and stopped; raw
  failures/diagnostic and temporary registry/live proof are retained. No
  recorded whole-call match is claimed for C1612C. Resolve that integration
  boundary before activating it; do not fake a hardware read or service return.
  Local DMA passes 388 / 12,416, including 80 inactive display PCs. The
  independently tested union is 17,692 / 566,144, with no new overlap. No
  fresh combined run this batch; last fresh combined is 16,384 / 524,288.
  Shared CPU/bus/math and instruction fixtures are unchanged; all fourteen
  older generator outputs are unchanged. Final GNU/MSVC Release builds and
  the 545-row gate pass 554,068 shadow / 413,271 sandbox matches, zero
  mismatches, exact seals and identical poison frames. All 36,236 isolated
  frames/seals for the final four-entry registry group match; those entries
  are cold, so this is integration evidence rather than executed body proof.
  Group is exact through frame 600; ALL remains 416/361. Build/ is
  0.947 GiB. See analysis/routines/native_c_input_display_setup.md
  and analysis/figures/native_input_display_setup_checkpoint.json.
  Next reconstruct five complete main-loop timer/control/message owners:
  C2527C/C25312/C2548A/C1518C/C32CEE, 660 unique / zero shared boundaries,
  sealed in analysis/data/main_loop_services_scope_inventory.json. That
  inventory implements none of them and replaces no OS service. Original
  game-function coverage remains incomplete; retain the user's stopping
  criterion and the deferred Copper fade.
- 480 of 624 translated entries plus sixty-one source-only callable entries
  are registered: 541 rows and 355 source-timed entries (294 translated plus
  sixty-one source-only). Eight complete input-device callback/setup owners
  cover 274 unique / zero shared boundaries. C1718E/C17456/C1748C add three
  translated entries; C174A0/C16CD8/C16B8C/C17104/C1712C add five source-only
  entries, each with an original call site. C17456 publishes the actual C1718E
  callback. Fourteen actual child sites retain original return PCs and stack
  arguments. Full CPU/SR/all-RAM proof passes 196,608 complete calls without
  exclusions: 65,536 controlled-child and 131,072 real-child cases; both
  layers cover all eight owners and all 274 boundaries. Contracts change
  caller arguments, request/port globals, saved page index and palette source
  across child calls to verify original reloads and partial-write order.
  Actual ON/shadow/sandbox dispatch passes 6,144 fixtures; each mode covers
  every owner PC and checks entry/mode/selection/source-write guards. Native
  ON continuation and exact reference classifications remain required.
  Shadow captures and replays the actual original mouse-counter read; sandbox
  retains its uncaptured hardware classification. The initial test-reference
  capture discrepancy is retained in the checkpoint; production dispatch and
  the strict classification assertion were unchanged by that fixture fix.
  Normal step-disabled callback C matches 36,236 shadow calls, with zero
  hardware/incomplete/mismatches. Sandbox classifies 36,385 hardware calls,
  zero matches/incomplete/mismatches. Seven setup/removal owners are cold;
  all six raw batch reports and generic C17456 rejection remain retained.
  An isolated normal-callback check passes separately. These full-call tests
  do not establish independent OS/hardware timing or resolve earlier file
  service source stops. Original service children remain actual calls.
  Local DMA passes 274 / 8,768. The independently tested union is 17,304 /
  553,728, with no overlap this batch. No fresh combined run this batch;
  last fresh combined remains 16,384 / 524,288. Shared runtime CPU/bus/math
  and instruction fixtures are unchanged; all thirteen older generator
  outputs are unchanged. The local instruction proof covers the new NOP
  generator recipe. The full 541-row gate passes 554,068 shadow / 413,271
  sandbox matches, zero mismatches, exact seals and identical poison frames.
  All 36,236 isolated live frames/seals match source OFF; group exact through
  frame 600, ALL still 416/361. GNU and MSVC Release pass; build/ is
  0.902 GiB. See analysis/routines/native_c_input_device_callbacks.md
  and analysis/figures/native_input_device_callbacks_checkpoint.json.
  Next reconstruct five complete game input/display setup and synchronization
  owners: C16D4C/C16FF4/C17066/C1787A/C1612C, 388 unique / zero shared
  boundaries, sealed in analysis/data/input_display_setup_scope_inventory.json.
  This inventory implements none of them and replaces no OS service.
  Continue the original game graph to the stopping point stated above.
  Stage D and the full C port remain open; Copper fade remains deferred.
- 477 of 624 translated entries plus fifty-six source-only callable entries
  are registered: 533 rows and 347 source-timed entries (291 translated plus
  fifty-six source-only). Five complete formatter/game-side file owners cover
  174 unique / zero shared boundaries within this batch. C0EF08 and three
  source-only callers are added. C0F56A was already registered/timed; its old
  whole-call adapter is now complete without counting a new entry. Its 41 PCs
  reuse the proven number-field timing engine through exact source ownership.
  Sixteen actual child sites retain the original return PCs/stack arguments.
  Full CPU/SR/all-RAM proof passes 57,344 complete calls, without exclusions:
  40,960 controlled-child cases cover all five owners and all 174 boundaries;
  16,384 real formatter cases cover all 41 formatter boundaries. Formatter
  fixtures include width 0x80 wrapping to 0x7F, all CCR combinations and
  output overlapping rewritten caller arguments. Actual ON/shadow/sandbox
  formatter dispatch passes 768 fixtures and entry/mode/selection/write
  guards; every mode covers every formatter PC and requires native ON or
  completed reference comparisons. The four file owners retain original
  case-0 FC0FF0 service stops: no complete real-child or real dispatch proof
  is counted for them. File-loading/independent OS timing parity stays open;
  production children remain actual source calls. Controlled contracts verify
  changed handles, buffers and saved result reloads across close.
  Normal step-disabled C matches nine formatter calls per mode, with zero
  hardware/incomplete/mismatches. The four file owners are cold; all raw zero
  rows and the generic C0EF08 rejection remain retained.
  Local DMA passes 174 / 5,568. The independently tested union is 17,030 /
  544,960, after the 41-PC overlap: 133 new PCs. No fresh combined run this
  batch; last fresh combined remains 16,384 / 524,288. Shared runtime CPU/
  bus/math and instruction fixtures are unchanged; all twelve preceding
  generator outputs are unchanged. The full 533-row gate passes 554,063
  shadow / 413,303 sandbox matches, zero mismatches, exact seals and identical
  poison frames. All 36,236 isolated live frames/seals match source OFF;
  group exact through frame 600, ALL still 416/361. GNU and MSVC Release
  pass; build/ is 0.867 GiB. See
  analysis/routines/native_c_postflight_file_callers.md and
  analysis/figures/native_postflight_file_callers_checkpoint.json.
  Next reconstruct eight complete game input callback/setup owners:
  C1718E/C17456/C1748C/C174A0/C16CD8/C16B8C/C17104/C1712C,
  274 unique / zero shared boundaries, sealed in
  analysis/data/input_device_callbacks_scope_inventory.json. That inventory
  implements none of them and replaces no OS service. Continue the original
  game graph, then Stage F and only necessary Stage E. Stage D and the full
  C port remain open; Copper fade remains deferred.
- 476 of 624 translated entries plus fifty-three source-only callable entries
  are registered: 529 rows and 343 source-timed entries (290 translated plus
  fifty-three source-only). Sixteen complete postflight-message/text owners
  cover 513 unique / zero shared boundaries within this batch, including all
  four original C111E8 table arms. C110A4/C11350 and thirteen source-only entries
  are added; C11078's fixed-charge adapter is completed. C15C36's original
  external JSR proves C0F4D8's independent callability. Thirty-seven actual
  child sites retain their source return PCs and stack arguments.
  Controlled children cover every 513 boundary in 131,072 complete full-CPU/
  full-SR/all-RAM cases, without exclusions. Real children cover thirteen owners
  and all 411 of their boundaries in 212,992 cases. The total is 344,064,
  without counting failed or replaced tests. Real children permit actual CIA
  reads (write-log mode 2). C0F4D8/C11478/C114D2 retain original service stops
  at FC1FC4/FC0FF0/FC0FF0; no complete real-child proof is counted for those
  owners. C110A4's actual file child uses its original nonzero global status
  gate; actual file-loading parity remains open. Actual ON/shadow/sandbox
  dispatch passes 9,984 complete fixtures over the thirteen real-completing
  owners, covering all their boundaries in every mode and retaining entry/
  mode/selection/write guards. The three service-dependent owners retain
  separate complete controlled-child proof, not real dispatch proof.
  Normal step-disabled C matches four shadow / four sandbox calls:
  C11078 once and C110A4 three times. The other fourteen owners are cold;
  raw zero rows and generic C0F4D8 rejection remain retained. No exclusions,
  fabricated child results or RAM-only CPU claims are introduced.
  Local DMA passes 513 / 16,416; the independently tested instruction union
  is 16,897 / 540,704, with 0 overlapping PCs. No fresh combined run
  this batch; the last fresh combined run remains 16,384 / 524,288.
  Shared runtime CPU/bus/math and instruction fixtures are unchanged;
  all eleven preceding generator outputs are byte-for-byte unchanged.
  The full 529-row gate passes 554,063 shadow / 413,303 sandbox matches,
  zero mismatches, exact seals and identical poison frames. All 36,236 isolated
  live frames and RAM seals match; the group is exact through frame 600 and
  ALL remains 416/361. GNU and MSVC Release pass; build/ is 0.833 GiB.
  See analysis/routines/native_c_postflight_messages.md and
  analysis/figures/native_postflight_messages_checkpoint.json for evidence.
  The earlier queue_mode_messages correction proposal was mistaken: its
  documented C10678 mapping and advance_menu_mode_messages delegate are
  correct. prepare_postflight_messages independently implements C110A4.
  Next reconstruct C0F56A/C0EF08/C162E4/C1631C/C16386, the related formatter
  and game-side mode-file callers: 174 unique / zero shared boundaries.
  analysis/data/postflight_file_callers_scope_inventory.json implements none
  of them and replaces no OS service. Continue the original game graph,
  then Stage F and only needed Stage E. Stage D and the whole C port remain
  open; Copper fade remains deferred.
- 474 of 624 translated entries plus forty source-only callable entries are
  registered: 514 rows and 327 source-timed entries (287 translated plus forty
  source-only). Thirteen complete postflight/reset/restart owners cover 205
  unique / zero shared boundaries within the batch. Four source-only entries
  are added and nine older fixed-cycle adapters are completed. C091E6 is an
  actual independently called root transform; its 34 instructions reuse the
  existing domain and share 32 PCs with the preceding timing union.
  Separate real and controlled children cover every boundary in 319,488 full
  CPU/SR/all-RAM calls, without exclusions. Actual ON/shadow/sandbox dispatch
  passes 9,984 complete fixtures and entry/mode/selection/write guards. All
  3,328 source calls per mode are hardware-free and require completed reference
  comparisons. The actual scene, view, buffer and table children remain intact;
  controlled children verify their complete CPU/RAM entry contracts before
  changing fields/registers. The shared local-to-world domain now uses defined
  unsigned wrapping for the three-product sum and position addition, preserving
  original ADD.L overflow before ASR.L. Real and controlled root fixtures cover
  the overflowing edge products and positions, including saved-stack RAM.
  Root fixtures also overlap the MOVEM stack with the position fields; the
  final flag input is read after those writes. Their focused real/controlled
  run replaces the initial root cases without counting duplicate tests.
  Recorded normal C matches 1,890 shadow / 1,891 sandbox calls across nine
  active owners; one incomplete C11788 shadow call remains retained, zero
  hardware/mismatches. C118FC/C11934/C1104C/C091E6 are cold; raw zero rows and
  generic C118FC rejection remain retained. Local DMA passes 205 / 6,560; the
  fresh combined oracle passes 16,384 unique instructions / 524,288 cases.
  There are 173 new PCs after the 32-PC overlap. Shared runtime CPU/bus/math
  and instruction-oracle fixtures did not change; all ten older generator
  outputs are unchanged. The shared domain correction was nevertheless followed
  by the fresh combined proof. The full 514-row gate passes 554,063 shadow /
  413,303 sandbox calls, zero mismatches, exact RAM seals and poison. All 36,236
  isolated live frames and seals match; the group probe is exact through frame
  600, ALL still 416/361. GNU and MSVC Release pass; build/ is 0.800 GiB. See
  analysis/routines/native_c_postflight_completion.md and
  analysis/figures/native_postflight_completion_checkpoint.json for hashes.
  Next reconstruct sixteen sealed postflight-message/text owners, including
  the complete C110A4 parent, C11350 and C0F4D8/C0F812 publishers: 513 unique /
  zero shared boundaries and all four original C111E8 table arms. See
  analysis/data/postflight_messages_scope_inventory.json; it implements none
  of them. The typed queue_mode_messages API correctly names C10678 and
  delegates to advance_menu_mode_messages. C110A4 requires a distinct domain;
  the earlier proposed delegation correction was mistaken.
  Keep actual OS/file children and the preceding parity limitation. Continue
  the original graph, then Stage F and only necessary Stage E. Stage D and the
  complete C port remain open; Copper fade stays deferred.
- 474 of 624 translated entries plus thirty-six source-only callable entries
  are registered: 510 rows and 314 source-timed entries (278 translated plus
  thirty-six source-only). Fourteen complete menu/context completion owners
  cover 441 unique / zero shared boundaries; all are new to the instruction
  union. Three translated and seven source-only entries are added; four older
  fixed-cycle adapters are completed. C10DAE includes all 137 instructions;
  C25070 owns its negative-return prefix C2506C without counting another entry.
  Real and controlled children independently cover every boundary, passing
  344,064 complete CPU/full-SR/all-RAM calls without exclusions. The real layer
  permits actual CIA reads (write-log mode 2). Its earlier blocking-mode fixture
  and the FE9136 divergence trace remain diagnostics, not real timer proof.
  Actual ON/shadow/sandbox dispatch passes 10,752 fixtures and entry/mode/
  selection/write guards. Each mode includes 3,040 hardware-free and 544
  hardware-bearing source calls. Reference modes require completed C matches
  only for hardware-free calls; hardware calls must retain exactly one hardware
  classification and zero matches/incomplete/mismatches. ON completes both
  categories with native continuations and full CPU/RAM parity.
  Normal C matches 4,998 shadow / 5,020 sandbox calls, including two independent
  C25070 matches per mode after the parent absorbs its batch calls. Four C10CFE
  calls per mode retain hardware classification; shadow retains four C10D8A
  and eighteen C10DAE incomplete calls. C16D04 has zero completed recorded
  comparisons: batch shadow 8,114 hardware / 112 incomplete, sandbox 8,578
  hardware; independent isolation shadow 8,118 hardware / 112 incomplete,
  sandbox 8,579 hardware. Preserve these separate reports and generic timer
  rejection. Seven source-only peers are cold; raw zeros and generic C10A24
  rejection remain retained. CPU/RAM timer proof does not complete independent
  OS/hardware timing parity; production still calls actual C53C78.
  Local DMA passes 441 / 14,112. The independently tested instruction union is
  16,211 / 518,752; no fresh combined run this batch. Last fresh combined remains
  15,370 / 491,840. Shared runtime CPU/bus/math and instruction-oracle fixtures
  did not change. Shift/decimal helpers and memory-ADD direction handling are
  family-local; all nine older generator outputs are unchanged.
  The full 510-row gate passes 554,063 shadow / 413,303 sandbox calls, zero
  mismatches, exact final RAM seals and poison. All 36,236 isolated live frames
  and seals match; the group probe is exact through frame 600, ALL still 416/361.
  GNU and MSVC Release pass; build/ is 0.765 GiB. See
  analysis/routines/native_c_menu_context_finish.md and
  analysis/figures/native_menu_context_finish_checkpoint.json for evidence hashes.
  Its then-next thirteen postflight/reset/restart owners and independently
  called root transform cover 205 unique / zero shared boundaries in
  analysis/data/postflight_completion_scope_inventory.json. They are now
  implemented and proven by the latest checkpoint above; the inventory itself
  implements none of them. Original OS/file-load parity retains the preceding
  limitation and the remaining original graph still requires reconstruction.
- 471 of 624 translated entries plus twenty-nine source-only callable entries
  are registered: 500 rows and 300 source-timed entries (271 translated plus
  twenty-nine source-only). Fourteen complete menu/context return owners cover
  207 unique / zero shared boundaries; all are new to the instruction union.
  Ten original source-only entries are added; four older translated adapters
  now have complete normal C and source timing. Each real-child and controlled-
  child layer covers every owner boundary, passing 344,064 complete CPU/RAM
  calls without exclusions. Actual ON/shadow/sandbox dispatch passes 10,752
  complete fixtures, every boundary in every mode, and non-call/OFF/selection/
  entry-write guards. C108FE preserves its genuine source-installed RTS-only
  body. Normal C matches 2,667 calls per reference mode: twelve C0FB70,
  1,863 C0FBB6, fifteen C101FC and 777 C10228, zero hardware/incomplete/mismatches.
  Ten peers are cold; raw zero rows and the generic C1064C rejection remain
  retained. Local DMA passes 207 / 6,624. The independently tested instruction
  union is 15,770 / 504,640; no fresh combined run this batch. Last fresh combined
  remains 15,370 / 491,840. Shared runtime CPU/bus/math and instruction-oracle
  fixtures did not change; generator changes only add the menu_return choice.
  The full 500-row gate passes 554,025 shadow / 413,303 sandbox calls, zero
  mismatches, exact final RAM seals and poison. All 36,236 isolated live frames
  and seals match; the group probe is exact through frame 600, ALL still 416/361.
  GNU and MSVC Release pass; build/ is 0.719 GiB. See
  analysis/routines/native_c_menu_return.md and
  analysis/figures/native_menu_return_checkpoint.json for evidence hashes.
  Its then-next fourteen sealed completion owners, including the larger
  C10DAE parent and C09192/C16D04/C25070 helpers, cover 441 unique / zero shared
  boundaries in analysis/data/menu_context_finish_scope_inventory.json.
  They were subsequently completed by the latest checkpoint above. The inventory
  itself implements none of them; remaining callbacks and the original graph
  still require reconstruction.
- The preceding baseline had 471 translated entries plus nineteen source-only
  callable entries: 490 rows and 286 source-timed entries (267 translated plus
  nineteen source-only). The nine delayed-menu/outcome and scan owners
  cover 238 unique / zero shared boundaries within the batch; 45 are shared
  with preceding groups. Original C105F4 JSR evidence activates the already
  complete C29368 scan, reusing its C body and original timing. Complete real-
  child and controlled-child layers independently cover every boundary in
  221,184 full CPU/RAM cases without exclusions. Actual ON/shadow/sandbox
  dispatch passes 6,912 complete fixtures, every owner boundary in every mode,
  and non-call/OFF/selection/entry-write guards. Normal C matches 724 shadow /
  724 sandbox calls: 318 C1072E, 394 C1075A, twelve C1078A in each mode, zero
  hardware/incomplete/mismatches. Six peers are cold; raw zero reports and
  generic rejection remain retained. The mode-nine null-hook C crash was fixed
  by forwarding existing countdown hooks; complete source and C coverage pass.
  Local DMA passes 238 / 7,616. The independently tested instruction union is
  15,563 / 498,016; no fresh combined run this batch. Last fresh combined remains
  15,370 / 491,840. Shared runtime CPU/bus/math and instruction-oracle fixtures
  did not change; generator reuse preserves prior output for seven older families.
  The full 490-row gate passes 554,025 shadow / 413,303 sandbox calls, zero
  mismatches, exact RAM seals and poison. All 36,236 isolated frames and seals
  match; the group probe is exact through frame 600 and ALL remains 416/361.
  GNU and MSVC Release pass; build/ is 0.686 GiB. See
  analysis/routines/native_c_menu_outcome.md and
  analysis/figures/native_menu_outcome_checkpoint.json for evidence hashes.
  Its then-next fourteen-owner return inventory is now implemented and proven
  by the latest checkpoint above. Completion continuations and the remaining
  graph remain open. Original OS/file-load parity retains the limitation below.
- The preceding menu-followup baseline had 470 translated entries plus
  thirteen original source-only callable
  entries: 483 total rows and 277 source-timed entries (264
  translated plus thirteen source-only). C1029E/C10418/C10458/C10678/C1643A
  cover 151 unique / zero shared original boundaries. Their normal C adapters
  pass 122,880 full CPU/RAM calls without exclusions; controlled children cover
  every boundary and compare complete child-entry CPU/RAM. Real children cover
  all four callback/message owners. Original C1643A stopped at FC0EC0, case zero,
  before the C comparison; its returning real fixtures retain nonzero initial
  status and cover six gate boundaries. Its remaining 53 file-owner boundaries
  require controlled-child proof. Keep the actual source-stop diagnostic distinct
  from original OS/file-load parity; production still calls actual OS children.
  Recorded normal C matches 19 shadow / 19 sandbox calls (18 C10678 and one
  C1643A in each mode), zero mismatch/hardware/incomplete. The file call's 74
  source cycles identify the initial status gate. Three source-only callbacks
  are cold; their generic zero-call rejection and raw reports remain retained.
  Actual ON/shadow/sandbox dispatch passes 3,840 complete CPU/RAM fixtures and
  non-call, OFF, selection and entry-write guards for all five owners. Local
  DMA passes 151 / 4,832; the fresh combined oracle passes 15,370 / 491,840.
  The full 483-entry gate passes 554,025 shadow / 413,303 sandbox calls, zero
  mismatches, exact RAM seals and poison. All 36,236 isolated live frames and
  seals match. The 600-frame group probe is exact; ALL remains 416/361.
  GNU and MSVC Release pass; build/ is 0.654 GiB. See
  analysis/routines/native_c_menu_followup.md and
  analysis/figures/native_menu_followup_checkpoint.json for scope and hashes.
  Its next C104C2/C105F4/C1072E/C1078A inventory has
  159 unique / zero shared boundaries and four original indirect-table arms.
  It implements none of those owners; the subsequent nine-owner batch now
  completes them and immediate continuations. Continue their remaining continuations
  and the remaining original graph; Stage D and the whole C port remain open.
- The preceding cold-menu baseline had 469 translated entries plus ten
  source-only callable entries. Those
  entries are registered in port/game/glue/ports.c: 479 total rows. There are
  272 source-timed entries (262 translated plus ten source-only). Complete
  C0FE36/C1017E/C10272/C103E4 callbacks and C09120/C09148/C29490/C2949A/
  C10B90/C16406 helpers cover 132 unique / 14 shared original boundaries.
  Their readable adapters pass 245,760 full CPU/RAM cases without RAM/register
  exclusions. Controlled-child proof covers every owned boundary; the real-
  child layer excludes eight C0FE36 load-path boundaries after original OS
  execution stopped at FC0EC0, case 352, in the held-event environment. Its
  returning fixtures retain a nonzero load-status gate for that route. Keep
  the actual source-stop diagnostic and full controlled-child proof distinct;
  that batch retained original C1643A and OS children. All ten entries are
  cold in recordings, and the generic zero-call rejection remains retained.
  Actual source-only ON/shadow/sandbox dispatch passes 7,680 complete CPU/RAM
  fixtures, including non-call, selection and source-write entry guards.
  Source-only references run original bytes in the existing runtime; normal
  glue contains no opcode handlers. The expanded dispatch proof exposed slow
  repeated write-log scans; the indexed lookup preserves raw addresses and
  exact first/last-write semantics. Its 4,096 independent sorted-sequence cases
  and both deliberate-byte-mismatch reference-mode tests pass. No liveness,
  custom-write ordering or stack/DMA exclusions changed. Active C0FCB4/C0FECE
  normal C still passes 5,476 shadow / 5,332 sandbox calls with eight incomplete
  shadow calls retained. Local DMA passes 132 / 4,224; the fresh combined
  oracle passes 15,219 instructions / 487,008 cases. The full 479-entry gate
  passes 554,025 shadow / 413,303 sandbox calls, zero mismatches, all RAM seals
  and poison exact. All 36,236 isolated live frames and seals match. GNU and
  MSVC Release builds pass; build/ is 0.620 GiB. See analysis/routines/native_c_menu_cold.md and
  analysis/figures/native_menu_cold_checkpoint.json for hashes and build size.
  The subsequent follow-up/file-load batch now completes its five sealed owners
  (151 unique / zero shared boundaries); source-only registration does not close the
  remaining original callback/call graph or complete Stage D.
  The preceding menu-setup baseline had 469 registered translated entries.
  The latest full gate for that registered set matched 554,025 completed shadow
  calls and 413,303 sandbox calls across three native recordings, with zero
  mismatches and identical poison frames. Ported parents absorb some formerly
  counted child calls, so the aggregate call totals need not rise monotonically.
  Its earlier build/recomp/ports_report_*.json described that 469-entry baseline;
  current reports have 514 rows. GNU and MSVC builds passed at 262 source-timed
  entries. Complete C0FBE0 menu
  setup and C1082C/C11BB0/C24FA4 helpers are newly registered; the existing
  C17B96 sound selector now has a complete normal adapter and source timing.
  Their 141 unique / zero shared boundaries each pass separate real-child
  and controlled-child proof: 122,880 complete CPU/RAM cases without exclusions.
  Independently isolated C0FBE0 and C17B96 complete one sandbox comparison each,
  zero shadow; two incomplete shadow calls remain retained, hardware/mismatches
  zero. The parent absorbs the selector's batch statistics, so the initial
  generic C17B96 zero-call rejection remains retained alongside its successful
  standalone proof. Three peers are cold; their generic zero-call rejection
  also remains retained. Local DMA passes 141 / 4,512. Independent instruction
  union coverage is 15,104 / 483,328; no fresh combined run this batch. The last
  fresh combined oracle remains 14,963 / 478,816. Shared runtime CPU/bus/math
  and instruction-oracle fixture setup did not change. The structural test-only
  variant resets inactive/OFF write-log bookkeeping between fixtures to avoid
  accumulated original busy-loop writes exhausting allocation; actual children,
  delays and complete RAM comparisons remain unchanged. All 36,236 isolated
  live frames and RAM seals match. Build/ is 0.575 GiB. See
  analysis/routines/native_c_menu_setup.md and
  analysis/figures/native_menu_setup_checkpoint.json.
  The separate menu_cold_scope_inventory.json seals 191 unique / 14 shared
  boundaries across ten source-only owners and one translated required helper,
  C1643A. That inventory is not implementation evidence; the subsequent cold
  menu batch implements and activates its ten source-only entries. C1643A
  was then a required original child; the current batch completes its normal
  C and timing while retaining the original OS children and proof limitation.
  The preceding six-entry complete
  menu-transition family covers 324 unique boundaries, including all six
  original table arms and 66 cold C0FECE instructions absent from its generated
  listing. Every boundary passes separate real-child and controlled-child
  proof: 147,456 complete CPU/RAM cases without exclusions. Main callback
  readable C passes 5,476 shadow / 5,332 sandbox comparisons; eight incomplete
  shadow calls remain retained. Four peers are cold; the generic zero-call
  rejection remains retained. Local DMA passes 324 / 10,368; the fresh
  combined oracle passes 14,963 instructions / 478,816 cases after adding
  source PC-relative forms to the shared operand helper. All 36,236 isolated
  live frames and RAM seals match. Build/ is 0.541 GiB. See
  analysis/routines/native_c_menu_transition.md and
  analysis/figures/native_menu_transition_checkpoint.json.
  The preceding five-entry
  context-publication group passes 153 instructions / 4,896 DMA cases;
  independent instruction coverage at that checkpoint was 14,650 / 468,800.
  Its last fresh combined run was 14,599 / 467,168; it was not repeated for that batch
  because shared runtime CPU/bus/math and fixture setup did not change.
  Its separate original-byte real-child, shared-body and controlled-child
  proofs pass 106,496 full CPU/RAM cases without exclusions, covering all
  153 source boundaries. All five entries are cold in recordings, and the
  generic zero-comparison rejection remains retained. All 36,236 isolated
  live frames and seals match; build/ is 0.506 GiB. Existing command owners
  still pass 798 shadow /
  891 sandbox comparisons after the shared view-helper refactor. See
  analysis/routines/native_c_context_publication.md and
  analysis/figures/native_context_publication_checkpoint.json.
  The preceding eleven-entry
  scheduler group matches all 36,236 isolated live frames and RAM seals;
  build/ is 0.485 GiB. Its 180,224 full CPU/RAM cases cover every owned boundary
  without exclusions, including nine cold entries. Active dispatcher and
  mode-nine independent C matches 6,005 shadow / 6,005 sandbox comparisons;
  generic zero-comparison rejections stay retained. See
  analysis/routines/native_c_postflight_scheduler.md and
  analysis/figures/native_postflight_scheduler_checkpoint.json.
  In addition to active planes and the earlier audio batch,
  24 registered glyph, input, page, notification, command/audio, buffer,
  polygon, face, postflight and followup entries now use source-timed steps.
  These isolated bridges match fresh source OFF output on all 36,236 frames. The
  earlier combined instruction oracle matched 9,627 instructions and 308,064 cases
  with DMA contention enabled. See
  analysis/routines/native_c_face_predicate_timing_batch.md for that earlier
  timing set; analysis/routines/native_c_registered_timing_batch.md
  retains the earlier 24-entry evidence.
  The new four-entry map/region batch is independently exact across all
  36,236 live frames. Its 735 instructions also match 23,520 fixtures with
  DMA bus contention enabled. See analysis/routines/native_c_map_region_activation.md.
  C279D0 is also newly registered: its independent live output matches all
  36,236 frames, and its 271 instructions pass 8,672 DMA-contention fixtures.
  Readable whole-call C separately matches 3,258 shadow and 1,967 sandbox calls,
  plus 4,096 structural cases including partial writes and edge clamps. See
  analysis/routines/native_c_grid_projection_activation.md.
  Eleven additional renderer, clipping, record-view and transform entries now
  use source timing and independently match all 36,236 live frames. Their
  1,094 instructions pass 35,008 DMA-contention fixtures. The broader DMA
  oracle also found and removed an extra stack read in C17B08's older timing
  bridge. See analysis/routines/native_c_renderer_timing_batch.md.
  A further 22 sound-start/random and screen-frame/corner entries now use
  source timing and match all 36,236 live frames together. Their 1,026 source
  instructions pass 32,832 DMA fixtures. The sound correction moves the
  combined first difference from frame 297 to frame 416. Isolated ON final
  RAM also matches each recording's sealed SHA-256. See
  analysis/routines/native_c_sound_frame_timing_batch.md.
  Fifteen more polygon, line, projected-segment and cell-template entries now
  use source timing and match every live frame and sealed final RAM together.
  Their 1,062 instructions pass 33,984 DMA fixtures. ALL still first
  differs at frame 416 by 361 pixels.
  See analysis/routines/native_c_drawing_cell_timing_batch.md.
  Twenty additional startup, number-field, orientation and direction-tracking
  entries now match every live frame and sealed final RAM in isolation.
  Their 1,293 instructions pass 41,376 DMA fixtures. Those ALL traces
  matched through terrain-refresh
  entry and identify display-list sorting and condition updates as the next
  timing targets. See analysis/routines/native_c_startup_timing_batch.md.
  A further 41 terrain-sort, condition, pixel-lane and interrupt-counter
  entries now match all 36,236 isolated live frames and sealed final RAM.
  Their 611 source instructions pass 19,552 DMA fixtures; 144 registered
  entries now have timing steps. Fresh traces match the entire startup CSV
  and first terrain return, resolving sorting/condition debt. The interrupt
  counter correction removes the frame-7 loop drift. That batch's outer-loop
  rows matched through frame 295 and located the C12098 update gap. See
  analysis/routines/native_c_terrain_pixel_timing_batch.md.
  Nineteen further view-control, record-rate and matrix-pipeline entries now
  match every isolated live frame and sealed final RAM. Their 1,107 source
  instructions pass 35,424 DMA fixtures; 163 registered entries now have
  timing steps. C12098's return matched source; that batch located C1C63E's
  first flight-parent gap after fixed-charge selection helper C230B0. See
  analysis/routines/native_c_view_matrix_timing_batch.md.
  Twelve further selection, fire-initializer, target/projection, attitude and
  cockpit/list helpers match all 36,236 isolated live frames and sealed RAM.
  Their 746 source instructions pass 23,872 DMA fixtures; 175 registered
  entries now have timing steps. The complete first flight parent now matches.
  Enclosing update rows match through the map-stage return in frame 310;
  C0F090 after C0DAEE is the next gap, +386 cycles. See
  analysis/routines/native_c_flight_update_timing_batch.md.
  Seven more matrix-marker, projection, circle and fault-return entries match
  all 36,236 isolated frames and sealed RAM. Their 344 instructions pass
  11,008 DMA fixtures; 182 registered entries now have timing steps. The
  marker return now matches; the enclosing first gap is after C1CB26 in frame
  311. Nested traces locate C1FB82 after the already stepped C2005C dispatch.
  See analysis/routines/native_c_marker_projection_timing_batch.md.
  Five further face/component predicates and scan line-style entries match
  all 36,236 isolated frames and sealed RAM. Their 125 instructions pass
  4,000 DMA fixtures; 187 registered entries now have timing steps. The
  scene-stream/grid/first scan returns now match. The enclosing first gap
  is C0F132 after C11BFC in frame 311, +2,344 cycles and SR 0004/0000.
  See analysis/routines/native_c_face_predicate_timing_batch.md.
  Seven scene initializer/root-setup entries now preserve source timing and
  match every isolated live frame and sealed final RAM. Their 311 instructions
  match 9,952 DMA fixtures; there are 197 timing-step entries. The initializer
  span now matches 16,280 source cycles and removes one of two delayed fade
  frames. Independent group coverage is 10,007 instructions / 320,224 cases;
  this is not a fresh full combined oracle run. ALL still first differs at
  416/361. See analysis/routines/native_c_scene_transition_timing.md.
  The complete C1E540 placement-ordering parent is now registered, with its
  full CPU adapter and six existing children newly source-timed. Coverage is
  422/624 and there are 204 timing-step entries. Its readable adapter matches
  8,192 structural calls and 4,075 shadow / 4,090 sandbox isolated whole-call
  comparisons with original liveness; 15 shadow calls remain incomplete.
  The nine-entry live group matches all 36,236 frames and sealed RAM. Local
  timing matches 649 instructions / 20,768 DMA cases; a fresh full combined
  oracle now passes 10,622 instructions / 339,904 cases. Full registered
  parent counts are 4,075 shadow / 5,796 sandbox, zero mismatches or hardware
  classifications. ALL remains 416/361. See
  analysis/routines/native_c_placement_order_domain.md.
  The complete C1D10C template-placement parent is now registered as entry
  423, with its normal CPU adapter and C1D722 source timing. At that checkpoint
  there were 206 timing-step entries. All 8,192 structural adapter cases and
  492 shadow / 1,046 sandbox isolated whole-call comparisons pass with original liveness.
  The seven-entry isolated group matches all 36,236 live frames and sealed
  RAM. Local timing passes 681 instructions / 21,792 DMA cases; the fresh
  combined oracle passes 11,303 / 361,696. ALL remains 416/361. See
  analysis/routines/native_c_template_placements_domain.md.
  The complete C1CB14/C1CB26 scene-placement pair is now registered, including
  the shared distance refresh, countdown/skip gate and descriptor-result tail.
  There are 208 timing-step entries. All 8,192 structural CPU/RAM cases and
  5,601 shadow / 11,446 sandbox normal isolated whole-call comparisons pass;
  2,565 shadow and 187 sandbox calls are incomplete, zero hardware/mismatches.
  Local source timing passes 99 instructions / 3,168 DMA cases; a fresh combined
  oracle passes 11,402 / 364,864. The 600-frame pair probe is exact; ALL remains
  416/361. The isolated pair also matches all 36,236 live frames and sealed
  final RAM. See analysis/routines/native_c_scene_placements.md and
  analysis/figures/native_scene_placements_checkpoint.json.
  The complete C1CCBC follow-up placement parent and C1D0A4 workspace-position
  entry are now registered, bringing the total to 427 and timed entries to 212.
  The parent matches 8,192 structural CPU/RAM cases and 288 shadow / 5,131
  sandbox normal whole-call comparisons; 3,790 shadow and 699 sandbox calls
  remain incomplete. Both position variants separately match 16,384 complete
  all-register/full-SR/all-RAM cases, without exclusions; C1D0A4 is cold in
  recordings. The parent, both position entries and C25876 match every isolated
  live frame and sealed RAM. Local timing passes 333 / 10,656 DMA cases and the
  fresh combined oracle passes 11,735 / 375,520, now restoring the sealed
  machine before every case so synthetic writes cannot contaminate later
  source bytes. ALL remains 416/361. See
  analysis/routines/native_c_followup_placements.md and
  analysis/figures/native_followup_placements_checkpoint.json.
  The complete C0F5F8 post-input parent is now registered: 428/624, with
  213 timing-step entries. All 16,384 complete original CPU/RAM cases match
  every register, full SR and all RAM without exclusions. Normal isolated
  readable C passes 15,929 shadow / 16,001 sandbox completed comparisons,
  retaining four hardware and 79 incomplete shadow calls and ten incomplete
  sandbox calls. Local timing passes 118 / 3,776 DMA cases; the fresh combined
  oracle passes 11,853 / 379,296. All 36,236 isolated live frames and sealed
  RAM match. ALL remains 416/361. GNU/MSVC pass; build/ is 0.305 GiB.
  See analysis/routines/native_c_post_input_tick.md and
  analysis/figures/native_post_input_tick_checkpoint.json.
  The complete C1C860 context-refresh, C08F26 bootstrap and C0F920/C0F992
  callback wrappers are now registered: 432/624, with 223 timing-step entries.
  All 32,768 complete CPU/RAM cases match every register, full SR and all RAM
  without exclusions. Normal readable C passes 3,743 shadow / 5,815 sandbox
  comparisons, including three independent bootstrap body comparisons.
  Incomplete calls stay separate. The proof tool now automatically isolates
  a called child when its parent absorbs the batch comparison; it still
  requires a completed original comparison for every entry. The ten-entry
  timing group passes 272 / 8,704 DMA cases and all 36,236 live frames/sealed
  RAM. Fresh combined timing passes 12,125 / 388,000. ALL remains 416/361.
  GNU/MSVC and the corrected typed context contract pass; build/ is 0.317 GiB.
  See analysis/routines/native_c_scene_bootstrap.md and
  analysis/figures/native_scene_bootstrap_checkpoint.json.
- The current recordings are captures/native/demo01,
  captures/native/qual_carrier_success, and
  captures/native/qual_fail_crashes. Each has state.bin, input.fa18in, and
  run.json with a sealed final RAM hash. Archived captures/uae runs, including
  run075, are historical evidence and are not an acceptance gate.
  Current RGB timing checks generate fresh source OFF and recreated-C ON
  streams with the same executable, state, input and machine model. The
  starting snapshot originated in UAE, but is shared by both runs. Their
  difference tests the replacement paths; agreement cannot detect a machine
  error shared by both runs or establish independent Amiga timing parity.
- The headless GNU and MSVC Release builds pass. The typed port's eight
  affected map/detail contract tests passed with the map source changes.
- Only .vscode/ is untracked; it belongs to the user. Leave it alone.
- C2AA9C, C2AB34, C2AB5A and C2B05A are now registered with resumable source
  timing. The map parent reuses port/map_packet_depth_stage.c; its normal/wide
  children reuse the established map packet core. Their readable whole-call
  glue is checked separately from the instruction steps by
  python tools/recomp/check_map_region_glue.py. This isolated registry variant
  matched 281 shadow and 3,988 sandbox calls without changing the normal runner.
- The region probe additionally matches original registers and RAM on 4,096
  direct structural oracle cases, including signed coordinates, sloped edges,
  endpoint exclusions and randomized high register halves. The oracle found
  and corrected the old C's near-endpoint exit: $C2B1E4/$C2B1FE abandon the
  directory walk, not just one segment. See
  analysis/routines/c2b05a_record_region_probe.md.
- The registered all-native path is not yet frame-faithful. Complete
  source-timed bridges now give exact isolated output for the
  C31226/C3129A/C31312 postflight group and C305AA polygon edge across all three
  sealed recordings. C0FA04 is also source-timed and exact across the three
  recordings. The complete registered ON demo now matches through frame 415
  and first differs at one-based frame 416 by 361 pixels. The preceding
  renderer batch was at frame 297 by 5,440 pixels. Source timing for the
  sound-start/random family removes that early frame difference; the new
  screen-frame/corner family is also exact in isolation through all recordings.
  No fixed mean fees were substituted. The latest 15-entry drawing/cell batch
  resolves the independent C1D3F4/C2FF48/C2EE4A differences at frames 415/416,
  matching all 36,236 frames and sealed final RAM in isolation. ALL retains
  the same frame-416 difference. The remaining bounded ranking finds
  C3201A/C31F4C/C20A40 at frame 424 by 34,144, C26EBE at frame 441 by
  12,238 and C0D04C/C20D68 at frame 484 by 17. The latest 41-entry batch
  removes the sorting/condition debt and the interrupt counter's frame-7
  drift. Fresh startup CSVs and the first terrain-refresh return match source.
  C1612C's 100-frame CSV and the eight-frame ROM CSV are also identical.
  Outer-loop rows match through frame 295. The first flight parent and view,
  projection, attitude, terrain, cockpit, map and matrix-marker returns now
  match. The enclosing update's first 31,275 instruction rows match every
  field, including the scene-stream, grid and first record-scan returns.
  Its first difference is C0F132 after C11BFC in frame 311, +2,344 cycles
  (source/ON 44,324,676/44,327,020), with SR 0004/0000. The face predicate
  bridges removed the earlier +2,024-cycle stream gap; source timing for
  C2F490 removed the subsequent -16-cycle record-scan gap. C11BFC is a
  registered message update with a fixed charge and no child calls.
  These observations locate complete-call debt, not substitute charges or
  proof of which instruction causes the frame-416 pixels. See
  analysis/routines/native_c_face_predicate_timing_batch.md.
  Shadow/sandbox matches do not establish live ON fidelity for the whole
  registered set.
- Plane shadow now replays the live source's ordered DMACONR inputs on saved
  entry RAM. It independently checks native outputs and write sequences, and
  fails extra/reordered/missing reads. All five former counter failures now
  compare and pass. Report fields busy_input_calls/reads expose this proof
  input model. Two intentionally incorrect bridge copies are rejected by
  tools/recomp/check_shadow_busy_inputs.py. Full ON replay separately proves
  the actual timing. No mismatch or hardware classification was suppressed.

## Newly activated grid projection packet

C279D0 now implements the complete three-table packet in
port/game/grid_projection_packet.c, with CPU effects and resumable timing in
separate glue files. It preserves source-selected depth gates, sparse-matrix
projection, negative record kinds, partial triangle writes, coordinate clamps,
and both pixel helpers. All 36,236 isolated live frames are exact.

The structural oracle found a word-overflow distinction at C27BD6/C27C98:
BLE after ADD.W tests the signed unwrapped sum, whereas subsequent CMP.W
instructions compare wrapped depth. That case is retained in 4,096 original-
instruction fixtures, including 12 partial-write and 13 edge-clamp cases.
Custom writes are held in these structural fixtures; the separate live replay
proves actual drawing and event timing.

The generic tools/recomp/check_whole_call_glue.py ENTRY... now supplies
independent readable-C checks for future batches. It reuses cached objects and
an isolated registry, running the three recordings concurrently. The earlier
check_map_region_glue.py delegates to it with its four default entries.

## Previous map and region batch

The previous map/region live blockers are resolved. C2AB34/C2AB5A share the
source-timed packet bridge, C2AA9C supplies the depth-stage frame and pass
sequence, and C2B05A retains the source's interrupt boundaries and unusual
D2/D4-D6 restore. Domain observers remain mathematical values; CPU effects
stay in glue. No new gameplay or placeholder behavior was introduced.

The first map bridge passed CPU-only instruction fixtures yet accumulated
bus delays from extra reads on CLR memory instructions. A boundary trace found
the first difference at C2AD22 in frame 297. Removing those reads made the
600-frame map trace identical and the complete isolated live batch exact.
The instruction oracle's new --bus option checks memory-access timing under
DMA contention so this distinction is covered before expensive full replays.

## New timing evidence

- Optional FA18_BOUNDARY_TRACE=PATH and FA18_BOUNDARY_RANGE=LO-HI (hex,
  exclusive HI) produce instruction/flow-target and chipset-service CSV rows,
  with CPU registers, SR, cycles, next event, frame/line and blit/interrupt
  state. tools/recomp/summarize_boundary_trace.py reports costs and event sites.
- On the complete sealed demo, 17 original region calls cost 20,214-47,376
  live cycles including waits and interrupts. Fourteen enter $FC0D14 and
  span the next frame. The first starts in frame 19,443 and interrupts at
  $C2B19E after 2,560 cycles. The traced replay matched sealed final RAM.
- First-600-frame traces include 22 wide-map calls, 5 normal-map calls and
  22 active-plane calls. Wide-map calls starting at frames 299, 327, 349,
  366 and 384 each include 416 blits before returning.
- Commands, artifact hashes, source interrupt sites and interpretation limits
  are in analysis/routines/native_c_call_boundary_timing.md. Filtered rows
  omit chipset events inside children and interrupt handlers outside the
  selected address range; start/end frames still prove those crossings.

## Planning review, 2026-10-02

The user asked to step back and plan because progress was too slow. The seven
batches after f2d0bc6c added 119 timing bridges (68 -> 187), but readable
coverage stayed 419/624 and ALL retained its frame-416, 361-pixel difference.
Those bridges have useful independent proof, but their count is not progress
in readable game coverage or the combined first-difference milestone.

The integration cost comes from mixing two execution models. Mechanical
translation preserves original instruction/event boundaries through the CPU
model. Many readable whole-call replacements still use fixed charges, so
matching registers and writes does not imply matching interrupt, drawing or
frame timing. Shadow retains the source timeline; live ON exposes this gap.
The final deliverable is readable game C with a plain-C backend, not another
complete CPU bridge. These proof and delivery milestones must stay explicit.

Following the earliest CPU-cycle gap selected C11BFC in machine frame 311.
That gap is real; it has not been shown to cause the first visible difference.
Do not automatically source-time it, then chase the next unrelated return.

Fresh 500-frame demo probes with the unchanged 50f6d6a7 executable give:

| Enabled entries | First RGB difference |
| --- | --- |
| ALL 419 | Frame 416, 361 pixels |
| All 187 source-timed entries together | None through frame 500 |
| All 232 entries without timing steps together | Frame 416, 361 pixels |
| ALL except C11BFC | Frame 416, 361 pixels |
| C30918 alone | Frame 416, 361 pixels |
| All 187 timing entries plus C30918 | Frame 416, 361 pixels |
| C321D2 and C32260 together | Frame 415, 361 pixels; each alone starts at 424, 34,144 pixels |
| C30D34, C30EAA, C309B6 and C30F78 together | Frame 416, 361 pixels; neither tested two-entry half differs through 500 |
| ALL except the seven HUD entries in the preceding three rows | Frame 416, 361 pixels |

The subset/complement controls expose multiple and interacting failures.
Equal pixel counts do not prove equal changed pixels or a single root cause.
Removing one failing subset does not eliminate the full failure. These are
bounded diagnostic results, not full-recording proof or independent Amiga
machine parity. Logs/selectors/results are cached as build/recomp/planning_*;
RGB scratch streams are removed automatically. The first four probes took
4.29 seconds, eight partition controls 7.55 seconds, 35 minimization queries
(32 distinct candidate replays) 28.67 seconds, and six further controls
5.92 seconds. Minimization reuses one source stream and cached repeated
selectors. C30918's 35 original instructions
and sole C2F60A child give a smaller visual reproducer than the unrelated
256-instruction C11BFC message update.

At the planning review the candidate tool reported four ready leaves:
C1D10C and three already deferred Kickstart trampolines. This did not mean
only one game function remained feasible. Whole-family selection and explicit indirect-child
contracts are needed to move beyond the leaf-only ranking.

## Next work

Readable milestone completed: C1EBB0 and C1EC84 are now complete, registered
workspace-selector helpers, including their shared tails. Coverage is 421/624,
with 190 timing-step entries. Each readable whole-call adapter matches 16,384
original-instruction structural calls (all registers, full SR, PC and RAM);
their 34 instructions pass 1,088 DMA cases. Both helpers are cold in sealed
recordings: the temporary recorded whole-call tool correctly rejects zero
completed comparisons, and its rejection is retained. The independent
complete-call oracle supplies their proof; full isolated live runs are
nonregression checks, exact on all 36,236 frames and sealed RAM. The full
421-entry shadow/sandbox/sealed-RAM/poison gate passes with unchanged completed
call totals. GNU/MSVC pass. Independent instruction groups now total 9,696 /
310,272 cases; the combined oracle was not rerun for these local groups.
See analysis/routines/native_c_workspace_record_helpers.md. This was the
421-entry checkpoint; both complete selector parents are now registered below.

Latest visible checkpoint: the user's Copper-fade observation led to a
verified correction. C0FAA4 and six root-setup entries now preserve source
timing. The initializer span matches the original 16,280 cycles and no longer
crosses an extra vertical blank. Source/ALL terminal state writes now occur
at machine frames 436/437, improved from 436/438. The fade increments remain
three frames apart; first dim output is now RGB frame 417 instead of 418.
ALL still first differs at 416/361. The remaining delay is inherited between
the first two C0F5F8 countdown ticks: source 313/342, ALL 313/343. The actual
C0EFD4 update entered in frame 313 has the same 152 instruction PCs but
returns 70,498 cycles later in ALL, accumulating debt through HUD calls.
Restoring original execution for the 17 HUD entries listed in
analysis/routines/native_c_scene_transition_timing.md matches the fade reset
and completion and moves the first RGB difference to 425/36,650. Neither
tested half nor any of three individual entries suffices. This is a diagnostic
complement, not removal of registered C or a complete parity fix. Before and
after frame/fade JSONs are preserved separately under analysis/figures/.
This is the second timing-only batch since the review; return to the complete
selector parents next, retaining the causal HUD checkpoint for later parity.

Selector milestone completed: C1E540 is now fully registered as entry 422,
including selection, proximity, special heights, planes, indexed polygons/
triangles and scratch partitioning. Its semantic observer supplies the live
CPU outputs without repeating children/writes; CPU effects stay in glue.
The original C1C9AE mask is retained by the new --glue recorded proof and by
check_whole_call_glue.py. Both give 4,075 shadow / 4,090 sandbox matches;
15 shadow calls remain incomplete. All 8,192 structural adapter cases and
the captured failing demo call pass. C1E7F6's inherited zero shift and partial
register high words are preserved. The earlier memory-only proof remains
available but is no longer the integration evidence.

The complete parent and six formerly fixed-charge children now use source
timing, sharing the existing C1EBB0/C1EC84 tails. Nine entries match all
36,236 live frames and sealed RAM. Their 649 instructions match 20,768 DMA
fixtures; the fresh combined oracle matches 10,622 / 339,904. The full
422-entry gate passes 688,103 shadow / 1,111,316 sandbox comparisons, zero
mismatches, sealed RAM exact and poison identical. Aggregate calls changed
because the new complete parent absorbs child calls. GNU/MSVC pass. There
are 204 timing-step entries. A fresh bounded ALL probe is still 416/361;
coverage advanced, combined parity did not. Build/ is 0.228 GiB after cleanup.
See analysis/routines/native_c_placement_order_domain.md and
analysis/figures/native_placement_order_checkpoint.json. C1D10C is next for
423/624, as completed below. The user has since deferred the HUD/fade work.

C1D10C integration milestone is now complete in port/game/template_placements.c/h:
all selector packs, fourteen-band expansion, 24-byte cache emission, reverse
linked descriptor copies and final control-list publication. It is registered
as entry 423; there are 206 timing-step entries. All 8,192 complete adapter
fixtures match original caller-live CPU outputs and RAM outside the source's
bounded 128-byte private stack, including 2,743 cache-limit and 1,941 linked
copy cases. Full isolated normal whole-call checks pass 492 shadow / 1,046
sandbox calls, zero mismatches/hardware; 539 shadow calls are incomplete.
Production caller masks are retained: all sixteen registers and all eight
data-register high words, plus N/V/C at C1C924. The historical memory-only
proof remains separately reproducible and is not the integration claim.
The parent and C1D722 timing match 681 instructions / 21,792 DMA fixtures;
a fresh combined oracle matches 11,303 / 361,696. The full registered gate
passes 669,031 shadow / 1,075,296 sandbox calls, sealed RAM exact and poison
identical. GNU/MSVC pass. The seven-entry isolated live group matches all
36,236 frames and sealed final RAM across all three native recordings.
ALL still first differs at 416/361; coverage advanced, combined parity did
not. Build/ is 0.271 GiB after replay cleanup. Do not redo this selector
milestone. See
analysis/routines/native_c_template_placements_domain.md and
analysis/figures/native_template_placements_checkpoint.json. The earlier
native_template_placements_domain_checkpoint.json is historical domain proof.

The complete primary/alternate scene-placement pair C1CB14/C1CB26 is now
registered at 425/624, with 208 timing-step entries. It owns the entire
99-instruction shared traversal, including distance refresh, negative-result
countdown and descriptor return storage; consumers remain explicit children.
The earlier typed traversal omitted these full-parent gates and cannot be
substituted for this complete domain. Original source distinguishes C1ED3C's
bypass and the -128 countdown overflow branch; both are retained.
All 8,192 structural cases match every register/high word, PC, SR control and
RAM outside a bounded 160-byte child stack. Normal isolated whole-call proof
passes 5,601 shadow / 11,446 sandbox calls, zero mismatches/hardware; 2,565
shadow and 187 sandbox calls remain incomplete. Caller masks are unchanged.
Local timing passes 99 / 3,168 DMA cases; fresh ALL instruction proof passes
11,402 / 364,864. GNU/MSVC pass. The full 425-entry gate passes 653,694 shadow /
1,069,233 sandbox calls, sealed RAM exact and poison identical. Changed call
totals reflect parents absorbing their children. The pair matches all 36,236
isolated live frames and sealed final RAM; ALL remains 416/361. Build/ is
0.283 GiB after automatic replay cleanup. The full live proof and retained call
classifications are in analysis/figures/native_scene_placements_checkpoint.json.
See analysis/routines/native_c_scene_placements.md. Do not redo this pair.

C1CCBC's complete selected-record, alternate-placement and final relative-point
parent is now registered, together with the complete signed workspace-position
entry C1D0A4: 427/624, with 212 timing-step entries. C1D0B6's shared position
body and C25876's list-point child also use source timing. The domain retains
bit-6/bit-4 precedence, signed workspace indexing, overflow-aware depth and
countdown branches, selected distance's partial publication, and all sixteen
relative-point slots. Position additions and list-matrix sums wrap explicitly.
Normal CPU outputs stay in glue and children are not repeated for outputs.
All 8,192 complete parent fixtures and 16,384 direct position-child fixtures
pass. Parent proof excludes only 160 private child-stack bytes; position proof
retains full SR and all RAM, without any exclusion. C1D0A4 is cold in native
recordings, so its live run is nonregression only. Normal isolated parent
proof passes 288 shadow / 5,131 sandbox calls, zero mismatches/hardware, with
3,790 shadow and 699 sandbox calls incomplete. Caller masks remain unchanged.
Local timing passes 333 / 10,656 DMA cases; fresh combined proof passes
11,735 / 375,520 with a sealed machine reset before every case/opcode read.
The full 427-entry gate passes 633,759 shadow / 932,215 sandbox calls, sealed
RAM exact and poison identical. All four entries together match all 36,236
isolated live frames and sealed final RAM. GNU/MSVC pass; build/ is 0.294 GiB.
ALL still first differs at 416/361. See
analysis/routines/native_c_followup_placements.md and
analysis/figures/native_followup_placements_checkpoint.json. Do not redo these
complete parent/helper milestones or count their internal labels separately.

C0F5F8 is now complete and registered at 428/624, with 213 timing-step entries.
Its LINK -4 frame, optional D1 offset terms, D0 high word, phase MOVEQ resets,
callback CCR input, actual C0F806/C0F808 call/return and child-owned state
changes match independent original execution. The older C0F804 report address
was a transcription error and is corrected. The domain retains the source
hexadecimal $4650 limit (18,000) and partial-write order. Its 16,384 structural
cases match all registers, full SR and all RAM, including stack, with no
exclusions. Normal readable-C replay matches 31,930 completed comparisons;
the full gate matches 644,155 shadow / 944,089 sandbox calls with no mismatches,
all seals and poison passing. All 36,236 isolated live frames and final RAM
match. Fresh combined timing passes 11,853 / 379,296 DMA cases. GNU/MSVC pass;
build/ is 0.305 GiB; ALL remains 416/361. Detailed classifications are in
analysis/figures/native_post_input_tick_checkpoint.json. Do not redo this parent.

The complete context-refresh/bootstrap/callback batch now raises the registered
set to 432/624 and source-timed entries to 223. C1C860 preserves request-bit
ownership, actual record flagging, both selector shifts, source save-frame
aliasing, ordered sort/cache/condition children and guarded pixel submission.
C08F26 includes all 80 instructions and all ten children, including the
C1C40C/C1C63E/C1C860 calls after C090AE. Its old initialization-only slice
ending at C090AD was not a complete parent. Both callback wrappers retain
their complete source branches and child-owned effects.

All 8,192 complete cases per new entry (32,768 total) match every register,
PC, full SR and all RAM without exclusions. Normal recorded proof matches
3,743 shadow / 5,815 sandbox completed comparisons, including C08F26's
independent body check on every recording. Shadow retains 347 context, three
wrapper and three standalone-bootstrap incomplete calls; no hardware/mismatch
calls are accepted. The proof tool now isolates recorded children whose batch
comparisons are absorbed by a parent; cold/unproven entries still fail, and
raw reports retain all classifications. The fresh combined oracle passes
12,125 / 388,000 DMA cases. Full registered proof passes 636,016 shadow /
930,150 sandbox calls, all seals and poison. The ten-entry timing group
matches all 36,236 live frames and sealed RAM. GNU/MSVC pass; build/ is
0.317 GiB; ALL remains 416/361. The typed context groundwork was corrected
to record flagging, ASR.W #8 for alternate origins, later request rereads and
the captured condition route; its focused contract passes. See
analysis/routines/native_c_scene_bootstrap.md and
analysis/figures/native_scene_bootstrap_checkpoint.json. Do not redo this batch.

The complete C22C80 record-update and C1C63E update-stage parents are now
registered at 434/624, with 225 source-timed entries. Their 338 instructions
pass 10,816 DMA cases; the combined oracle passes 12,463 / 398,816. All 16,384
complete original-byte CPU/RAM cases match full registers, PC, full SR and
all RAM without exclusions. Normal independent proof per entry matches
601 shadow / 10,140 sandbox calls, including an isolated C22C80 body check
on every recording; all incomplete/hardware classifications remain separate.
The full gate matches 599,422 shadow / 817,839 sandbox with all seals and poison.
Both entries match all 36,236 isolated live frames and sealed final RAM.
GNU/MSVC and the typed direct-origin contract pass; build/ is 0.329 GiB.
The 600-frame isolated probe is exact; ALL still first differs at 416/361.
C22C80 preserves slot 7's preparation-only sequence, slot 15's untouched
bit-zero mask, signed byte gates, child Z decisions and actual D5 save frame.
C1C63E preserves threshold requests, both key routes, partial D5.W restore
around the original C29042 child and final request publication. The proof
runner captures the child's return before local save frames, then carries
cold original bytes through runtime execution. No masks or exclusions change.
See analysis/routines/native_c_record_update_stage.md and
analysis/figures/native_record_update_stage_checkpoint.json. Do not redo these
parents; C08F26 now reaches the registered C1C63E owner.

The complete C29042 active-origin owner now adds **382** unique source instructions.
Its generated 153-instruction list omitted 229 cold internal instructions reached
through C28F2C's nine mode targets. The earlier 491-instruction batch estimate
was therefore incomplete: the two completed parents plus this full source
span total 720. analysis/data/active_origin_complete_source.json records the
sealed bytes, all targets and disassembly; port_info.instructions now includes
these paths and checks the source state hash. Readable domain C, independent
CPU adaptation and full source timing are implemented and registered at
435/624, with 226 timing-step entries. All 32,768 complete CPU/RAM cases pass,
including cold scan/preset contracts; they are not extra registered functions.
Normal readable-C proof passes 732 shadow / 732 sandbox completed calls, with
zero mismatches, hardware or incomplete classifications. Local timing passes
382 / 12,224 DMA cases; the fresh combined oracle passes 12,845 / 411,040.
All 36,236 isolated live frames and final RAM seals match. The full registered
gate passes 598,726 shadow / 817,657 sandbox comparisons with all seals/poison.
GNU/MSVC pass; build/ is 0.341 GiB. ALL remains 416/361; keep the fade deferred.
C2574A changes the reduced triple with length 0x200 while the caller preserves
the chosen shift. The older typed shift-only callback is now corrected and
its focused GNU contract passes. The complete owner uses real original children.
See analysis/routines/native_c_selector_origin.md and
analysis/figures/native_selector_origin_checkpoint.json. Do not redo C29042.
Complete C0EFD4 update, C0F3C4 pending input and C0D730 display owners are
now registered: **438/624, 229 timed entries**. The parent preserves all 210
instructions/68 child boundaries, live global rereads and saved-tick scheduling.
C0DA38 unlinks the enclosing frame; both C and timing continuations stop there.
All 24,576 real-child CPU/RAM cases, 3,072 distinct child-contract cases and
256 cold display timing calls pass with zero remaining continuations. The
child-contract fixtures cover every reachable parent boundary and compare
full CPU/SR/RAM before each child; they are not real-child execution claims.
Normal readable-C proof passes 13,083 shadow / 5,798 sandbox comparisons.
Source-first DMACONR and JOY0DAT/JOY1DAT/POTINP inputs are checked by PC,
address and read order; unsupported hardware/incomplete calls stay classified.
The proof registry retains these contracts when disabling timing steps, and
cold byte dispatches resume to the captured return/SP before classification.
Local timing passes 270 / 8,640 DMA cases; the combined oracle passes
13,115 / 419,680. Every isolated live frame (36,236) and RAM seal matches.
The full 438-entry gate passes 554,286 shadow / 394,909 sandbox comparisons,
all seals/poison exact. GNU/MSVC pass; build/ is 0.364 GiB. ALL stays 416/361.
The old typed flight threshold is corrected; the split pipeline remains
explicitly bounded groundwork. No handwritten glue invokes opcode handlers.
See analysis/routines/native_c_update_sequence.md and
analysis/figures/native_update_sequence_checkpoint.json. Do not redo these owners.
The complete C16EAE/C16BF2/C16C56/C13D34 input-event owners are now registered:
**442/624, 233 timed entries**. The sealed source audit covers all 109 original
instructions without cold omissions. Each entry passes 16,384 real-child
CPU/full-SR/all-RAM cases and 16,384 separate child-contract cases; the latter
cover every parent boundary and compare full child entry/return CPU/RAM.
Normal readable-C replay matches 32,126 shadow / 16,736 sandbox calls, zero
mismatches. C13D34 has zero recorded calls and complete independent structural
proof; do not count it as recorded gameplay coverage. The replay command
therefore lists C16EAE C16BF2 C16C56 explicitly. Empty external events skip
release, keyboard release preserves D0's other bytes, raw press/release widths
remain exact, and button levels use the original signed comparison.
Local timing passes 109 / 3,488 DMA cases; the fresh combined oracle passes
13,224 / 423,168. All 36,236 isolated live frames and RAM seals match.
The full 442-entry gate passes 555,538 shadow / 412,898 sandbox comparisons,
all seals and poison exact. GNU/MSVC pass; build/ is 0.386 GiB. ALL stays
416/361. See analysis/routines/native_c_input_events.md and
analysis/figures/native_input_events_checkpoint.json. Do not redo these owners.
The complete C1AC28 pending-command and C1AD74 keyboard owners are now
registered: **444/624, 235 timed entries**. The sealed source audit follows
all branches and shared tails, including C06BF0 reset and C1AC18 error exits:
562/1,024 instructions, 482 shared, 1,104 unique, no omitted cold paths.
Native domain C separates selection, aircraft, view/origin/zoom, indexed,
context and publication behavior. Existing view queue callers share the
proven publication body, preserving signed indices and aliasing write order.
Each owner passes 8,192 real-child complete CPU/RAM calls and another 8,192
child-contract calls. Component proofs add 190,464 cases; their union with
real-owner proof covers every source boundary without exclusions or code
patches. Controlled contracts compare full child entry CPU/SR/RAM and exact
entry/return addresses, then vary child outputs; they are not real children.
Normal readable-C replay passes 798 shadow / 891 sandbox comparisons with
timing bridges disabled and normal liveness retained. Both owners have
completed replay proof; internal action labels are not additional entries.
Local timing passes 1,104 / 35,328 DMA fixtures; the fresh combined oracle
passes 14,240 / 455,680. Exact owned-PC predicates allow noncontiguous tails
without treating children in source gaps as parent instructions. All 36,236
isolated live frames and RAM seals match. The full 444-entry gate passes
555,784 shadow / 413,307 sandbox calls, all seals/poison exact. GNU/MSVC pass;
build/ is 0.472 GiB. ALL stays 416/361. See
analysis/routines/native_c_command_dispatch.md and
analysis/figures/native_command_dispatch_checkpoint.json. Do not redo these
owners or count selector prefixes as completed functions. Next audit remaining
complete command/context publisher peers and the postflight-mode scheduler
family, preserving true child owners and explicitly proving cold peers.
C0F090/C0F132 remain internal labels, not additional functions.

The complete postflight mode scheduler family is now registered at
**454/624, 246 timed entries**. New owners are C09E06/C09E98/C09EC4/C0A002/
C0A12E/C0A15C/C0A1E0/C0A334/C0A364/C0A3EA; existing C0A2F0 now shares the
domain and has full source timing. C0A3A6/C0A3C6 shared tails are part of
their complete owners, not extra functions. The sealed source audit covers
359 unique instructions, 16 shared, with no omitted static paths.

All eleven normal CPU adapters pass 16,384 real-child original-byte cases
each (180,224 total), matching all registers/high words, PC, full SR and all
RAM including stack, without exclusions. Every owned boundary is exercised;
the pair/target fixtures explicitly cover their restore and near/far paths.
C0A364 preserves the event word high byte at completion. Long differences
preserve SUB/BGE overflow semantics, and the saved view is C458B2, not C457A7.
The older mode-nine API delegates to the same native domain.

Independent normal recorded C passes 4,086 shadow / 4,086 sandbox dispatcher
calls and 1,919 / 1,919 separate mode-nine calls, no hardware/incomplete or
mismatching classifications. The nine other entries have zero calls with
the dispatcher restored to source; generic whole-call checks reject their
zero-comparison groups and retain both rejection logs. Their complete
structural CPU/RAM proof supplies cold-entry evidence; live replay is only
nonregression for them. No caller masks or generic proof gates were weakened.
Local timing passes 359 / 11,488 DMA cases; fresh combined timing passes
14,599 / 467,168. All 36,236 isolated frames and seals match. The full gate
passes 554,025 shadow / 413,303 sandbox comparisons, all seals and poison
exact. GNU/MSVC pass; build/ is 0.485 GiB. ALL stays 416/361. See
analysis/routines/native_c_postflight_scheduler.md and
analysis/figures/native_postflight_scheduler_checkpoint.json. Do not redo
this family. Audit complete C1B7A6/C1BEE8/C1C214 command/context peers next;
C1B9CC is their shared internal publisher, not a separate translated entry.

Deferred Copper fade, by user instruction on 2026-10-02: the minor visible
difference may be ignored for current work and revisited later. Source/ALL
reset writes are machine frames 393/394 and terminal writes 436/437; each
fade step is three frames apart. The first visible RGB difference remains
416/361. The delay is inherited between countdown ticks (313/342 versus
313/343), with the demonstrated frame-313 HUD interaction preserved in
analysis/routines/native_c_scene_transition_timing.md and
analysis/figures/native_scene_countdown_checkpoint.json. This is an accepted
deferral, not a parity fix or a change to automated comparisons.

Gauge checkpoint completed after the planning review: $C30918 now has source
timing. Its 35 instructions pass 1,120 DMA fixtures; all 36,236 isolated live
frames and sealed RAM match. All 49 bounded parent trace rows match every
field, including skip and drawing returns. Before correction its isolated
frame-416 bytes exactly matched ALL's incorrect bytes; after correction the
isolated 600-frame probe is exact, while ALL still differs at frame 416 by
361 pixels. The full 419-entry gate remains 703,337 shadow/1,110,694 sandbox
matches, zero mismatches, sealed RAM exact and poison identical. There are
188 timing-step entries; independent group proofs total 9,662 instructions /
309,184 DMA cases (the combined oracle was not rerun for this local bridge).
See analysis/routines/native_c_gauge_timing_checkpoint.md for byte hashes and
call classifications. That was the first timing-only batch since the review. The
next readable implementation work is item 2, complete parent batches; retain
item 1's failing combined checkpoint rather than chasing another early gap.

1. Defer further Copper-fade timing work as the user requested. Preserve the
   corrected initializer and remaining frame-416 checkpoint. When revisited,
   parity work must follow the
   actual frame-313 update/countdown and the proven 17-entry HUD interaction,
   not return to an unrelated first cycle gap in frame 311. Compare
   C0FA0E/C0FA12/C0FA32 and terminal C1741A; original-code complements can
   reproduce the remaining cause but are not port fixes. ALL is still 416/361.
   Repeat bounded RGB and callback checkpoints after a source-backed candidate.
   Require a changed combined result before claiming improvement. Do not
   adjust average fees, counters or replay/frame conditions. C11BFC remains
   timing debt; the HUD list is not an automatic transcription queue.
2. Complete readable game owners in related batches. The current baseline
   is 537/624 translated plus seventy-three source-only callable entries,
   610 rows and 531 source-timed. The thirty-seven renderer entry/writer
   helpers and twelve selected-segment/projection/crossing upgrades are now
   complete. Next audit complete corner projection and view-record construction
   owners listed in the verified baseline; their original children and return
   peers remain part of the scope. The fifteen ground/HUD parents and twelve renderer
   helpers are now complete. The fourteen tested/list/grid/lattice face and
   edge parents are now complete, including C21060. Continue
   preserving original shared tails and actual child return PCs. The six
   stream/numeric/marker owners and two A0-base store upgrades are complete;
   original marker fixtures retain their bounded screen/plane limits and
   both strict recording rejections. The six new projection/readout owners and four shared projector
   upgrades are now complete. Their shared whole-call union is 268/272;
   actual production clamp segments from original internal boundaries prove
   the four otherwise unreachable PCs, separately from whole-entry coverage.
   Preserve individual bounded coverage and all generic cold rejections.
   Other older drawing parents still use legacy projection replay; complete
   their original-child/CPU behavior before declaring function porting done.
   All five grid/marker
   owners are now complete, with 468 unique source boundaries and full
   controlled complete-call coverage. All four previously
   registered geometry/history upgrades are now complete, with 1,015 unique
   source boundaries and full controlled complete-call coverage. The candidate
   owner's actual-child coverage is 816/819; its three unobserved PCs and the
   explicit controlled RAM return are recorded in the geometry checkpoint.
   Reconcile related C2C392's computed transfer at C2C46E before assigning
   its complete scope. Five helpers from the earlier nine-owner inventory are
   now complete, as are all four parents and the shared C28B34 stream. Keep
   C1612C unregistered until its frozen-event graphics-wait comparison can
   complete safely; its domain/CPU/step source, failed gates and successful
   temporary native replay remain explicit integration evidence.
   Preserve complete parent behavior, actual children, local frames, changed
   child outputs, original partial-write order and exact source ownership.
   C16084/C160D6 bounded initialization slices are not independently callable
   owners merely because historical reports discuss them separately.
   Preserve file/OS children; OS work remains deferred. Earlier C0FE36,
   C1643A and the mode-file family source-stop diagnostics remain distinct
   from their controlled-child proofs. Successful input-device fixtures do
   not resolve those separate stops. Track source-only callable entries
   outside the seeded 624-entry denominator, and require actual original
   calls or installed callbacks as entry evidence. Do not count table arms
   as extra routines. Prove readable domain C independently of its timing
   bridge, retain cold/hardware/incomplete rows and generic rejections, then
   run the required gates below. Keep whole-game graph coverage and native
   backend work open until their original-source requirements are met.
3. Keep parity and source coverage as separate measured outcomes. Do not
   spend another chain of timing-only batches without moving either ALL's
   first difference or readable coverage. Review after at most two such
   batches; if ALL still does not improve, return to complete readable parent
   batches while retaining the failing renderer checkpoint. This is a work
   selection limit, not permission to weaken proofs or declare parity done.
   The complete update and command dispatch owners are proven. Continue related
   complete command/context and postflight owners, preserving their shared tails,
   source child contracts and local frames.
4. Reduce repeated work: cache one source stream within each bounded probe
   round; run changed-group DMA fixtures and short live probes while editing.
   Run the full shadow/sandbox/sealed-RAM/poison gate and isolated full live
   replay once per coherent completed batch. Rerun the full combined
   instruction oracle when shared CPU/bus/math helpers or fixture setup change,
   or at an integration checkpoint; unchanged groups already have independent
   proof. Preserve all required comparisons and incomplete/cold classifications.
   Keep Ninja's shared objects, size caps, cleanup traps and disjoint recording
   parallelism; never relink executables still used by active checks.
5. Finish all original game functions, then stop when only Kickstart services
   and timing issues remain, as the user now requested. Do not proceed into
   service replacement or standalone timing fixes merely to satisfy the
   earlier full-port objective. Fresh OFF/ON comparisons test replacements on the
   same machine model; an independent UAE/Amiga timing check is a distinct
   proof and must not be inferred from them.
   Reconcile the complete original callback/call graph before declaring
   Stage D finished. The 624 translated entries are seeded from recordings;
   installed cold source-only owners and indirect targets also require C.

## Gate for a registered batch

- Read original instructions with python tools/recomp/port_info.py ADDRESS,
  then the corresponding analysis/routines report and typed port code. Preserve
  exact word arithmetic, high register halves, MOVEM.W sign extension, memory
  writes, and child effects. Reuse narrow register-effect helpers; never run a
  child with side effects twice.
- Build headless on this Windows workspace with
  & 'C:\Program Files\Git\bin\bash.exe' scripts/build_recomp.sh
  and build MSVC with
  cmake --build build/recomp-cmake --config Release -j 8
- Probe the new batch, then run
  & 'C:\Program Files\Git\bin\bash.exe' scripts/recomp_ports_check.sh
  over all three native recordings. It checks shadow, sandbox, sealed final
  RAM, and poison frames. Inspect per-entry calls, incomplete calls, and every
  mismatch. QUICK=1 is only a first-recording probe.
- Compare live `--ports on` RGB444 output with fresh `--ports off` source
  output for every affected recording, not just final RAM, frame count or blit
  totals. Set `PORTS_ONLY` to the comma-separated registered batch and run
  `& 'C:\Program Files\Git\bin\bash.exe' scripts/recomp_live_check.sh`.
  The same ON replay now also checks sealed final RAM, saving three extra
  replays per batch. Recordings run concurrently; temporary RGB/RAM outputs
  are removed. Build size is recorded with each integration checkpoint. An
  inactive entry needs temporary registration for a probe and must be removed
  if live output differs. A stepped SHADOW stream is not the live source
  oracle because source-first hardware-input replay can alter its timing.
- Increase the registered count and update this file only after every gate
  passes. Commit a coherent source batch with its evidence.
- Rank registered fixed-charge timing debt with
  `python scripts/probe_recomp_timing.py ENTRY... --frames 500`. Each positional
  argument is one entry, a comma-separated group, or `ALL`. The bounded probe
  reuses a single source stream and removes its scratch streams by default.
  `--rank-fixed N --differences-only` selects the N largest accumulated drifts
  from the latest demo gate report before probing them.
- Independently prove readable whole-call C with
  `python tools/recomp/check_whole_call_glue.py ENTRY...`; registered timing
  steps are disabled only in the temporary proof registry. A called child with
  no completed batch comparison is checked again in isolation automatically;
  each entry still needs a completed comparison. Raw batch/isolated reports
  retain hardware/incomplete classifications. The tool never relinks its
  temporary executable while a previous recording check is running.
  If a parent absorbs every child-call statistic, the generic batch correctly
  rejects the zero row without automatic isolation; run that child separately
  and retain both reports, as for the complete C0FBE0/C17B96 pair.
- The headless Ninja graph tracks source and header dependencies and shares
  objects with structural oracles and mutation builds. An unchanged build is
  subsecond. Do not manually touch generated C after a header change.

## References and constraints

- PORT.md: architecture and source conventions.
- port/game/glue/ports.c: actual registered set and ON cycle charges.
- scripts/recomp_ports_check.sh: native proof driver. Archived UAE replay and
  scripts/recomp_parity.py are not current gates.
- captures/native/*/run.json: sealed frame endpoints and final RAM hashes.
- Existing reports under analysis/routines/: original behavior evidence.
- Do not infer mechanics, constants, or object meaning without original source
  or capture evidence. Do not edit the user-owned scripts/check_native_build.py,
  scripts/native_frame_count.py, port/native_data_allowlist.txt, or .vscode/.
