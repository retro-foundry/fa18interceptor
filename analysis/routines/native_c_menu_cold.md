# Original cold menu callbacks and callable helpers (2026-10-03)

Ten original callable entries outside the recording-seeded translation now
have complete readable C, normal CPU adapters, source timing and native dispatch:
C0FE36/C1017E/C10272/C103E4/C09120/C29490/C2949A/C09148/C10B90/C16406.
These are installed callbacks or actual external JSR/BSR targets, including
shared tails. They are tracked separately from the 624 translated entries:
469 translated plus ten source-only entries, 479 total registry rows and
272 source-timed entries. Internal labels are not additional routines.

## Source and readable behavior

`python tools/recomp/audit_menu_cold.py` seals 132 unique / 14 shared original
instruction boundaries. C0FE36 preserves the action-one load/status gate,
actual C1643A child, child-changed mode-table pointer and unsigned table-word
comparison, action-two clear/reset/summary children, queue code 57 and common
action/mode clears. The no-action sequence gate republishes C0FBE0. C1643A
remains an original child, with no replacement OS behavior.

C1017E clears through its actual child, keeps all three LINK -10 locals and
filters modes 3 through 8 using the child-changed MODE_TABLE. The code-table
cursor advances only when a mode is enabled; it is not indexed by mode.
Queue order remains 40, available codes, 806E, zero, then C0FCB4 publication.
C10272/C103E4 retain their signed countdown gate, partial D0 writes, viewport
and auxiliary state, and distinct C1029E/C10418 callbacks. C103E4 first
publishes its context flag even when the countdown has not expired.

C09120/C09148 reuse the observer publisher with their exact source triples
and shared C0915A tail. Their adapters retain the negations and final MOVE.L
condition flags. C29490/C2949A reuse the already recreated selector-origin
candidate publisher, preserving MOVEM's unchanged flags and shared tail.
They are externally called from the delayed menu owner, rather than invented
new routines inside C29042. C10B90 calls actual position/update children on
either side of the cockpit bit-11 update. C16406 preserves LINK -6, both
local counters, all 39 word clears, final pointer/counter and changed flag.

## Readable-C proofs and the OS-dependent path

`python tools/recomp/check_menu_cold.py` passes 245,760 full CPU/RAM cases:
ten real-child owners at 16,384 cases each and ten controlled-child owners at
8,192 cases each. Every case compares all registers/high halves, PC, full SR
and all Chip/Slow RAM, including stack bytes, without RAM or register exclusions.
The controlled-child layer independently covers every owned source boundary,
including all C0FE36 load-path boundaries, with child-entry full CPU/RAM
comparisons and changed registers, CCR and table inputs.

The real-child layer has a distinct scope. C0FE36's load attempt stopped in
original execution at FC0EC0, fixture 352, in the held-event OS environment;
the retained source-stop log is not a C mismatch or successful comparison.
Its returning real-child fixtures retain a nonzero source load-status gate
for that route. Eight owner boundaries (C0FE5A/C0FE60/C0FE68/C0FE6E/C0FE72/
C0FE76/C0FE78/C0FE82) consequently require the controlled-child proof. The
other nine owners have complete real-child boundary coverage, and all remaining
C0FE36 boundaries are covered with actual children. No production child,
delay, liveness mask or source bytes are shortened or replaced. The test-only
inactive/OFF bookkeeping reset prevents completed fixture write logs accumulating.
This is not a claim of independent original OS/file-loading parity.

The recorded normal-C check retains real caller liveness and disables only
timing in its temporary registry. All ten entries are cold across all three
recordings, with raw zero reports and the generic
`C0FE36: no completed comparisons` rejection retained. The active enclosing
C0FCB4/C0FECE owners still independently match 5,476 shadow / 5,332 sandbox
calls after the dispatch change; eight incomplete shadow calls remain retained.
Cold original-byte proof and recorded nonregression are distinct evidence.

## Dispatch and timing

The runtime now recognizes selected source-only entries without adding them
to the generated function table. It applies the existing call/tail guards and
executes their native continuations in ON mode. Shadow and sandbox references
run original bytes through the existing runtime with native source-entry
dispatch suppressed. Handwritten glue executes no opcode handlers. Source
writes conservatively invalidate entry dispatch across each owner's outer
instruction range, including extension words and shared tails. Missing caller
liveness remains the existing all-live contract.

`python tools/recomp/check_menu_cold_dispatch.py` tests actual ON, shadow and
sandbox dispatch against original-byte entry/exit CPU/RAM snapshots, including
mode/selection/non-call/source-write guards. Reference-mode fixtures must
complete an actual matched comparison; silently continuing on a rejected
reference is a test failure. Detailed fixture counts and hashes are in the
checkpoint JSON. Instruction ownership stays separate from normal domain C.
The 7,680 expanded dispatch fixtures initially exceeded the 600-second limit
at the queue owner's shadow comparison. The proof write index now replaces
repeated linear searches, preserving exact first/last indices for each raw
address and every existing comparison/exclusion. An independent sorted-sequence
oracle passes 4,096 cases, including duplicates, aliases, zero keys and bucket
collisions. Deliberately changing a queue byte is rejected as a completed
mismatch in both shadow and sandbox. The complete 7,680-fixture run then passes
with the original case count and timeout. No native gameplay timing changed.

Local DMA timing passes 132 instructions / 4,224 cases. Because dispatch/runtime
integration changed, the fresh combined oracle passes 15,219 instructions /
487,008 cases. Shared tails account for the overlap with existing groups.
GNU and MSVC Release builds pass; build artifacts occupy 0.620 GiB.
The full registry gate passes 554,025 shadow /
413,303 sandbox calls, zero mismatches, exact final RAM seals and identical
poison frames. All 36,236 isolated live frames and seals match source OFF.
The 600-frame group probe is exact; ALL remains 416 / 361 pixels. Copper/HUD
work remains user-deferred. OFF/ON comparisons share the machine model and
do not establish independent Amiga timing parity.

## Next original owners

`python tools/recomp/audit_menu_followup.py` seals the next five-entry inventory:
C1029E/C10418/C10458/C10678/C1643A, 151 unique / zero shared boundaries.
It implements none of them. C10678 is already registered with an older adapter;
C1643A is a translated required helper, and the three others are source-only.
Recreate their complete follow-up/file-load behavior and preserve actual OS
children. Subsequent installed callbacks C104C2/C105F4/C1078A also require
source reconciliation; C1072E already has a registered older adapter.
Continue the complete original callback/call graph beyond the seeded translation.
Stage D game C -> Stage F plain native backend -> necessary Stage E remains
the full objective; the emulator and generated source are still scaffolding.
