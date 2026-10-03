# Complete delayed-menu and outcome owners (2026-10-03)

C104C2/C105F4/C1072E/C1078A/C105A6/C10626/C1075A/C108DA/C29368 now have
complete normal C adapters, source timing and native activation. C1078A adds
one translated entry; six original source-only entries are activated, and
C1072E/C1075A replace older adapters. Coverage is 471/624 translated entries
plus 19 original source-only callable entries: 490 registry rows and 286 timed
entries (267 translated plus 19 source-only). This is not whole-game completion.

## Original source and readable behavior

`python tools/recomp/audit_menu_outcome_source.py` seals 238 unique / zero
shared instruction boundaries within this batch. Forty-five are shared with
the previous instruction union: eight C1082C countdown/return boundaries and
37 control-record scan boundaries already owned by C29042. Original C105F4's
JSR at C10614 independently calls C29368; that call justifies activating the
existing complete scan body as its own entry. Its domain, CPU observer and
timing are reused, rather than copying the scan or counting an internal prefix.

C104C2 preserves LINK -2, saved D2, event clearing, key/message latch and actual
C11312 reset. Negative countdown selects a mode from 3 through 8, queues 5B
and calls C24FA4 with its two distinct original argument layouts. D2 is restored
after the child. C105A6 retains the message/key gate, latch, reset, redraw byte
and subsequent C105F4 callback. C105F4 preserves its pause/countdown state,
actual control-record scan child and C10626 callback. C10626 retains the
context-state and smoothing transition.

C1072E reproduces the original byte subtraction and its flags before queuing
message four. C1075A retains context gates and calls actual C11312 before
publishing the outcome countdown. Their older typed APIs now delegate to the
same domains; the typed countdown API keeps its existing message-reset body.

C1078A preserves LINK -2, signed request handling and mode-word local. A
positive request queues 5D for modes 3 through 8 and calls actual C24FA4.
It then reloads the child-changed MODE_TABLE pointer, increments its word at
offset 36 with source wrap and retained high D0, and publishes the changed flag.
The other positive modes search the original keys from memory in reverse.
All four eight-byte records, branch instructions and destinations are sealed:
keys 9, 7D, 2 and 1. These are arms within one owner, rather than extra routines.
The mode-nine arm adapts the existing countdown domain's hooks, preserving
its byte/word stores and callback CPU outputs. The cancelled-request route
keeps the actual reset child's changed attempt byte before decrementing and
choosing C10678 or C108DA. C108DA retains the expired-countdown message 43
and source-installed C108FE callback.

## Complete CPU/RAM and dispatch proofs

`python tools/recomp/check_menu_outcome.py` passes 221,184 complete original
calls: nine real-child owners at 16,384 cases and nine controlled-child owners
at 8,192 cases. Both layers independently cover every owned boundary, including
all four original table arms and all 37 standalone scan boundaries. Every
fixture compares all registers/high halves, PC, full SR and all Chip/Slow RAM,
including frame and saved-register bytes, without RAM or register exclusions.
Child-entry snapshots compare the actual original argument stack and full
CPU/RAM. Controlled children change registers, CCR, MODE_TABLE/counter words
and the cancellation attempt byte; the C consumes those changes.

The first expanded contract run caught a C-side mode-nine crash at fixture
1632 after source execution returned: a null hook pointer was passed to a
helper requiring its hook object. The adapter now forwards the existing
countdown phases. The original diagnostic is retained separately; the complete
real/contract and dispatch coverage subsequently pass with the original arm.
This failure was neither an original OS stop nor an accepted proof limitation.

`python tools/recomp/check_menu_outcome_dispatch.py` passes 6,912 actual
ON/shadow/sandbox fixtures. Dispatch scenarios span every owned boundary in
each mode; structural scenarios separately repeat profiles across all 32 CCR
combinations. ON must start a native continuation. Each reference-mode fixture
must complete an actual matched comparison, with no hardware/incomplete/mismatch
classification. Full CPU/RAM entry/exit matches without exclusions. Non-call,
OFF and selection guards require zero port calls; entry writes disable future
dispatch without changing PC/SP. These checks establish entry guards, rather
than every possible self-modification path.

Normal-C recorded proof disables timing only in its temporary registry and
retains production liveness. Each reference mode completes 724 comparisons:
318 C1072E, 394 C1075A and 12 C1078A, with zero hardware/incomplete/mismatches.
The six source-only peers are cold. Raw reports and the generic
`C104C2: no completed comparisons` rejection remain retained; the original-byte
fixture layers supply their path coverage independently of replay statistics.

## Timing and integration

Local DMA proof passes 238 instructions / 7,616 cases, including the reused
scan timing. The independently tested instruction union is 15,563 / 498,016.
There is no fresh combined run for this batch; the last fresh combined run
remains 15,370 / 491,840. No shared runtime CPU/bus/math or instruction-oracle
fixtures changed. The generator's timing-reuse extension preserves its previous
output for all seven older families; no prior generated bridge was rewritten.

The full 490-row gate passes 554,025 shadow / 413,303 sandbox matches, zero
mismatches, exact final RAM seals and identical poison frames. All 36,236
isolated live frames and seals match source OFF. The 600-frame group probe is
exact; ALL remains frame 416 / 361 pixels. Copper/HUD fade remains deferred.
OFF/ON share the machine model and do not prove independent Amiga timing parity.
GNU and MSVC Release pass. Build artifacts occupy 0.686 GiB. Counts, scope and
evidence hashes are in `analysis/figures/native_menu_outcome_checkpoint.json`.

## Next source scope

`python tools/recomp/audit_menu_return.py` seals the next fourteen complete
menu/context return owners, including required child C10362: 207 unique / zero shared
boundaries. That inventory implements none of them. C108FE is a genuine
source-installed return-only callback; preserve its original RTS behavior
without inventing work. Follow the installed C10C08/C10C68/C10A24 continuations
and the remaining original call graph. Original OS/file-load parity remains
limited as documented in the preceding file-owner checkpoint.
Stage D game C -> Stage F plain native backend -> necessary Stage E remains
the full objective; generated source and the emulator are still scaffolding.
