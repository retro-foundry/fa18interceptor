# Complete main-loop control records and message sequences

Updated 2026-10-03. Complete C1518C/C32CEE domains, CPU adapters and source
timing are activated. The registry has 483/624 translated entries plus 66
original source-only entries: 549 rows and 364 timed entries (298 translated
plus 66 source-only). Game-function porting remains incomplete. C1612C remains
inactive with its frozen graphics-wait integration failure explicit.

Authority is the sealed original state in `captures/native/demo01/state.bin`,
audited in `analysis/data/main_loop_control_messages_source_scope.json`.
The two owners contain 503 unique instruction boundaries, zero shared
boundaries and 15 actual child sites. Original JSRs at C0F11E/C0F3BA establish
callability. C32CEE's earlier internal tails begin at C32BD2; ownership and
registry start retain them without treating those labels as new functions.

| Owner | Complete behavior |
| --- | --- |
| C1518C | Traverse ten original control records, decrement cooldown/countdowns, consume packed requests under original guards, allocate budget, publish special selections, retain distinct tone/alert calls, start and advance records, and restore idle request/cooldown state. |
| C32CEE | Initialize and advance message selectors and segments, retain signed delay/wait/repeat counters, command-event and keymap handling, typed-text deletion and capacity rules, restart/live cursor triplets, colour/pace flags, tones and all four glyph-plane calls. |

Readable behavior is in `port/game/main_loop_control_messages.c`; register/SR
outputs and actual original child calls are in the corresponding CPU adapter.
Source register halves, signed comparisons, wrapping arithmetic, stack locals,
child arguments/return PCs, pointer reloads, buffer write order and distinct
glyph-plane offsets are retained. Sound/control children remain original calls.
The message owner reloads actual child outputs and changed plane/style/colour
inputs between glyph calls. No source service is replaced by this batch.

Full CPU, PC, full SR and all RAM match in 49,152 complete calls without
exclusions: 8,192 controlled-child and 16,384 real-child cases per owner.
The controlled layer covers every one of the 178/325 owned boundaries and
compares complete CPU/SR/RAM at each child entry. Test contracts scramble
caller registers and CCR, change selected flags and budget/mode after sound,
and vary glyph output registers, plane pointers and ready state. These are
test-only contracts; production retains all original children.

Real-child coverage is partial: 57/178 control PCs and 273/325 message PCs.
The real control fixture retains the original reset child and guards later
request/action children off. It does not prove completion of every real sound
or record-action path. The real message fixture exercises actual children on
valid original data; remaining branches are covered by the controlled layer.
Neither layer independently proves Kickstart or hardware timing parity.

Actual ON/shadow/sandbox dispatch passes 1,536 complete fixtures, 256 per
owner/mode. ON requires a native continuation; both fixtures are hardware-free
and reference modes require exact completed matches. Non-call, OFF, selection
and source-write invalidation guards also pass. Classifications are unchanged.

Independent step-disabled readable-C recording proof passes 19,397 shadow
and 21,815 sandbox comparisons. Control totals are 3,930 shadow / 5,806 sandbox;
message totals are 15,467 / 16,009. Shadow also retains 153 incomplete control
and 543 incomplete message calls; sandbox has none. All six raw reports have
zero mismatches and zero hardware classifications. The independent registry
keeps ownership, start/end, busy and tail contracts while disabling steps.
The generic checker passes unchanged.

Local instruction/DMA proof passes 503 instructions / 16,096 cases, comparing
CPU, SR, PC, cycles and all RAM. The independently proven union is now 18,352 /
587,264 with zero new overlap. LSR.W is family-local; DBRA uses the existing
expiry helper. All sixteen older generator outputs remain byte-identical.
Shared runtime CPU/bus/math and instruction fixtures are unchanged. No fresh
combined run was made; the last fresh combined remains 16,384 / 524,288.

GNU and MSVC Release builds pass. The full 549-row gate passes 555,565 shadow
and 417,331 sandbox comparisons, zero mismatches, exact final RAM seals and
identical poison frames. All 36,236 isolated live frames and seals match.
The group is exact through frame 600; ALL remains 416/361, with fade deferred.
Raw report hashes, per-owner coverage and counts are recorded in
`analysis/figures/native_main_loop_control_messages_checkpoint.json`.

Next complete the six control-record/alert consumers C153FC/C15688/C159AE/
C15AD4/C181A0/C15138, audited in
`analysis/data/record_control_actions_scope_inventory.json`: 537 unique / zero
shared boundaries. C15138 already has an older fixed-charge adapter; complete
CPU/SR and source timing remain in this scope. The inventory implements none
of these changes. Continue game-function work to the user's stopping point;
leave Kickstart services and timing issues explicit afterward.
