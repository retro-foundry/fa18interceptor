# Current playable port handoff

Latest direction, 2026-10-06: build a native runner from `port/game/`, starting
with intro -> credits -> pilot entry -> menu. This supersedes the incremental
emulation-removal work below and the former restriction against a new runner.
`fa18_native` is owned by `port/native/` and `port/game/native/`; shared loading
stays in `port/amiga/`. Build with `python scripts/build_native.py`. See
`port/native/README.md` for scope, source authority, validation and launch.
The emulator runners remain reference tools. Full native gameplay remains open.
The first functional intro/menu milestone is implemented: original splash,
credits and settled menu pixels match source OFF reference frames exactly;
first-tour callsign editing and 78-byte save/reload pass; SDL dummy presentation
runs. Native link-map omission check passes. Both reference builds and their
twelve tests pass. Native settled-screen timing is not frame-parity evidence.
Native menu continuation is now connected: original digit/function-key mode
selection, mission availability, pilot-log summary, Escape returns, SHIFT-2
reset and 78-byte save/reload. All five mode banners, the mission list and
pilot log match source settled pixels exactly. Both compiler reference builds,
the existing frontend checks and focused menu checks validate this batch.
Evidence: `analysis/native_menu_milestone.md`.
Native Free Flight now runs the C08F26 storage/pose/template-gate prefix,
C0FECE countdown/scene constructors, C101FC/C10228 viewport transition and
C10678 mode messages. Initial player pose, camera and all three template-gate
banks match a focused original checkpoint. Full record state still differs;
the final bootstrap record update is not connected. Endpoint: `scene-setup`,
C1072E. Other modes still stop at their banner. No native flight runs yet.
Evidence: `analysis/native_flight_start_milestone.md`. Rough flight-start
estimate 50%; this does not measure whole-game completeness.
Next: C1C63E/C22C80 bootstrap record updates, C1072E and interactive Free
Flight location/aircraft choices, then direct drawing and the flight loop.
Audio currently takes the original suppression path. Source data still uses
checked address-indexed host buffers, pending typed-state migration.

Updated 2026-10-06 after active-runner emulation metering, following the user's source-ownership correction, cleanup and
explicit instruction to prevent another costly detour. Read this handoff and
`AGENTS.md` before continuing. This file supersedes earlier resume instructions;
previous handoffs and removed source are preserved in git history.

The user's current comparison policy (confirmed 2026-10-06) ignores Copper
fade when comparing frames. Fade-only palette/brightness differences are
excluded; geometry, other rendering behavior and gameplay state still need to
match. Treat strict RGB hashes as diagnostics, not sufficient evidence of a
failure under this policy.

Latest measurement correction (schema 2): source-instruction adapters are now
counted at `step_begin`, where they fetch/decode an original opcode. Retained C
entry scheduling is excluded. The old schema omitted that work and overstated
CPU removal: cached **38.4011%** and old bounded **69.8507%** are superseded.
The corrected **800-frame, three-recording** raw CPU minimum is **0.4470%**;
per scenario demo **2.5530%**, carrier **0.4470%**, crash **2.4463%**. This is a
partial probe, not a full-suite result or completion percentage. The corrected
full-suite minimum is unmeasured. Accepted share unavailable: source comparisons
still fail. Memory/chipset/boot cutover **0%**, subsystem deletion **0/4**.
No gameplay behavior changed in this meter batch; old demo RGB/index/RAM and
all old profile fields match, MSVC/GNU profiles match, twelve CTests and profiling
invariance pass. No full replay repeated. Evidence:
`analysis/emulation_removal_adapter_meter_probe.json/.md`; the canonical
`analysis/emulation_removal_meter.json/.md` now contain this schema-2 partial
probe. Historical schema-1 full evidence remains in git/build history and must
not be used as the current estimate. Continue native game/parent ownership and
report future deltas using schema 2. The connected chain still removes 892
instruction cases, with explicit parity/timing/stack/matrix debt below.

Previous connected batch: native C1C63E retains game `RecordUpdateStageFrame`
and calls the C22C80 record loop directly. All 112 update-stage CPU cases are
deleted; the shared C22C80/C1C63E instruction body is now **100% removed**.
This connected chain has removed 892 instruction cases, retaining thin entries,
guest data and other original children. 4,096 original-child core cases match
all state; 4,096 production cases match registers/PC/SR and RAM outside old
CPU stack scratch. Strict all-RAM still fails (case 0 at C7FED1). Both compilers/
runners, twelve CTests and profiling pass. The MSVC/GNU demo matches. Bounded
recordings reach 43/15/75 native stage -> record-loop calls, with zero C22C80
CPU entry dispatch. Previous-build non-fade/index differences start at
381/446/263 (64/1,588/12,536 pixels); final RAM differs. Timing/parity stay open.
No full replay repeated. Bounded raw demo **69.8507%**, delta **-0.0008 pp**;
full raw **38.4011%** cached, accepted share unavailable, axes **0%**, gate **0/4**.
Raw meter excludes CPU-style adapters; repair that measurement before using it
for further progress estimates. Inventory unchanged. Evidence:
`analysis/emulation_removal_update_stage_batch.json/.md`.

Previous connected batch: C22C80 now runs a retained game `ControlRecordsFrame`
and calls the retained record-dynamics C owner directly. All 226 outer CPU
instruction cases are deleted; the sibling C1C63E's 112 cases are unchanged.
Together with the previous batch, **780/780 cases of these two targeted CPU
bodies (100%) are removed**. Thin entry adapters, guest memory and other
children's runtime boundaries remain, so this is not whole-plan completion.
4,096 original-child core cases match all state. 4,096 production continuation
cases match registers/PC/SR and RAM outside old CPU stack scratch C7FD00..C7FEFF;
strict all-RAM fails case 0 at C7FED5. IRQ/event timing is held in those proofs.
GNU/MSVC both runners, twelve CTests, profiling and the MSVC/GNU demo pass.
Three 800-frame recording probes reach 129/30/150 direct C22C80 -> C25B66 calls
and zero C25B66 CPU-entry dispatch. Non-fade comparisons versus the previous
batch fail at 346/446/263; final RAM differs. Timing/parity remain open. No full
replay repeated. Bounded demo raw **69.8515%**, delta **-0.0191 pp**;
full **38.4011%** cached, accepted share unavailable, axes **0%**, gate **0/4**.
Raw counters exclude CPU-style port steps and cannot represent whole-plan
completion. Counts remain 25/590/84/691. Evidence:
`analysis/emulation_removal_record_loop_batch.json/.md`.
Next: C1C63E parent ownership and native root/control/render children; preserve
explicit timing, non-fade, stack and matrix acceptance debt.

Previous connected batch: C25B66 now schedules retained game C dynamics and
composes its five native child owners directly. Its 554 CPU instruction cases
are deleted (42.3547% of the shared family); 754 other cases are unchanged.
A frame-end return fix prevents the interpreter consuming a pending C return.
16,384 held-child cases match full state and all 554 boundaries; six focused
runner frame pairs pass. GNU/MSVC both runners, twelve CTests, profiling and
an MSVC/GNU bounded demo pass. Original-child live matrix fixtures still fail
case 2. Three 800-frame probes complete but non-fade changes versus the previous
commit begin at 591/446/263 (39,478/2,989/187,594 pixels); final RAM differs.
These parity gates and unmodeled parent event timing remain open. No full replay
repeated. Bounded demo raw CPU **69.8706%**, delta **-0.0073 pp**;
full raw **38.4011%** cached, accepted share unavailable; other axes **0%**,
deletion **0/4**, counts 25/590/84/691. Raw counters exclude CPU-style port
steps, so they do not measure this body deletion or whole-plan completion.
Evidence: `analysis/emulation_removal_flight_parent_batch.json/.md`.
Continue outer C22 record-loop C ownership and direct parent integration.

Previous connected batch: native C2C392 now calls `normalize_record_vector` ->
existing `magnitude3` directly in game C. Its normal path no longer yields to
C2574A/C1D974 CPU entries. 12,400 production continuation cases match complete
register/PC/SR/RAM state with held events, including 396 calls to each child
and zero corresponding CPU-entry dispatch. A constructed real-runner frame
case reaches both edges and matches source RAM/RGB/indices. Its interrupts
are masked; outer instruction/event timing remains open. All six frame pairs,
GNU/MSVC runners, twelve CTests, profiling and the MSVC/GNU demo pass. Three
800-frame recordings match previous outputs/profiles and do not reach this
normalization path. Batch delta **0.0000 pp**, bounded demo **69.8779%**,
cached full raw minimum **38.4011%**, accepted share unavailable; other axes
**0%**, deletion **0/4**. Counts remain 25/590/84/691. Other callers still use
the normalization adapters. No full suite repeated. Evidence:
`analysis/emulation_removal_normalization_batch.json/.md`. Normal gameplay
children of this autopilot owner are now C calls; continue outer flight/loop
ownership, keeping the fault and unknown-transfer boundaries explicit and
the zone-exit timing regression below unresolved.

Previous connected batch: native C2C392 now calls six game steering functions
directly with C values/results, removing their CPU dispatch from this owner.
49,600 production continuation cases match all registers/PC/SR/RAM with held
events; 17,118 direct calls cover all six children. Six source-constructed
real-runner frame pairs start at original C22D88 -> C25B66, stop at the original
autopilot return and match RAM/RGB/indices. IRQs are masked in those frame
fixtures; sealed gameplay and parent event timing are separate open gates.
Three 800-frame recording RGB/index/RAM and full profiles are unchanged; an
MSVC/GNU demo matches. Both runners/compilers build, twelve CTests and profiling
checks pass. Steering CPU adapters remain for other original/unknown callers;
counts stay 25 C-owned / 590 CPU rows / 84 deferred. Bounded demo **69.8779%**,
batch delta **0.0000 pp**, cached full raw minimum **38.4011%**, accepted share
unavailable. Other cutover axes **0%**, deletion **0/4**. No full suite repeated.
Evidence: `analysis/emulation_removal_steering_calls_batch.json/.md`. Continue
native normalization and flight parent ownership; retain the zone-exit timing
regression below as unresolved acceptance debt.

Previous connected batch: C25B66's C25BA6 zone-exit call now selects native
`update_dynamics_record_zone_exit` -> `advance_record_zone_exit`. Its C frame
retains fault/placement phases across original runtime children. C28E28's CPU
entry/registry/guard and sole wrapper file are retired. **16,384 production
continuation cases** match all registers/PC/SR/RAM, covering **79/79** source
instructions. Both runners/compilers build; twelve CTests, GNU profiling and
an MSVC/GNU bounded demo pass. Native calls in 800 demo/carrier/crash frames
are **2/0/2**, CPU-entry calls zero, aggregate CPU counts unchanged. Demo/carrier
outputs and all three final RAMs match the preceding build. Crash rendering
has a **185-pixel non-fade regression**, first at frame **556**; source first
differences still **619/446/263**. Rendering acceptance is open; parent
instruction/event timing is unmodeled. Component correctness does not close
that integration gap. This removes an entry/stepping dependency, not new
metered CPU work: delta **0.0000 pp**. Counts **25 C-owned / 590 readable CPU
rows**, 84 deferred entries. Bounded demo raw CPU **69.8779%**; full minimum
**38.4011%** is cached, accepted percentage unavailable; other cutover axes
**0%**, subsystem deletion **0/4**. No full replay suite repeated. Evidence:
`analysis/emulation_removal_zone_exit_batch.json/.md`. Follow up the rendering
timing debt before claiming native-flight acceptance; remaining child calls
and the outer C25B66 CPU/event parent are still removal work.

Previous dependency removal: C25B66's C25C6A call now selects the native
forty-arm C2C392 record autopilot. The original owner has **987 instructions**,
including **361 cold instructions** missed by the old 626-instruction body.
A retained C frame owns response limits and continuation phases; original
normalization/steering children run through the existing dispatcher.
49,600 component cases match all registers, PC, SR and RAM (864/987 boundaries);
12,400 production-continuation cases match the same state (841/987). Both cover
all 361 cold instructions. Unknown/modified action targets retain the exact
original transfer; arbitrary fault-target behavior is not claimed proven.
Both GNU/MSVC runners build; twelve CTests, GNU profiling, the parent DMA
oracle and continuation IRQ/SP/lifetime/reset fixture pass. GNU/MSVC demo
frames/indices/RAM match. In 800 demo frames, 27 calls are native, CPU-entry
calls are zero, and aggregate CPU instructions decrease **2,076**. Demo raw
CPU work avoided is **69.8779% (+0.0397 pp)**; carrier/crash are unchanged.
Source parity remains open: first non-fade differences **619/446/263** (demo
previously 565). Parent instruction/event timing remains unmodeled, alongside
outer CPU/guest state and source child dependencies. There are **24 C-owned
entries**, **591 readable CPU rows**, **84 deferred entries** and **691 direct
opcode bindings**. Full raw CPU **38.4011%** is cached; bounded three-recording
minimum **48.0797%** is unchanged; accepted CPU percentage remains unavailable.
Native memory/chipset/boot **0%**, subsystem deletion **0/4**. No full replay
suite was repeated. Evidence: `analysis/emulation_removal_autopilot_batch.json/.md`.
Next: native normalization/steering child calls, then C28E28 in the same
flight parent. This remains actual game ownership work, not a fixed-cycle
adapter repair.

Previous dependency removal: C25B66's live matrix call at C25D9E selects the
game C owner directly, with copied arguments retained across IRQ service.
C2D408's CPU entry adapter and registry row are removed. Six bounded
before/after pairs (three recordings, parent and combined selections) match
all 800 RGB/index frames and final RAM. Combined dispatches decrease by
68/15/75; instruction/device counts remain identical. The DMA parent oracle
and boundary lifetime/IRQ/reset fixture pass, both GNU/MSVC runners build,
twelve CTests pass, GNU profiling is invisible and an MSVC/GNU demo matches.
There are twelve C-owned entries and 602 readable CPU rows. This is a live
call-site dependency removal; the flight parent, guest memory, result publisher
and existing 9,500-cycle atomic matrix timing remain CPU/chipset dependent.
Bounded CPU-work delta **0.0000 pp**, cached full raw CPU **38.4011%**;
memory/chipset/boot cutover **0%**, subsystem gate **0/4**. Source parity remains
open at demo/carrier/crash frames **565/446/263** (Copper fade excluded).
Evidence: `analysis/emulation_removal_matrix_dispatch_batch.json/.md`.
Prioritize native flight ownership and ordered events next; do not restore
the removed child CPU adapter. The plan remains incomplete.

Latest validated prerequisite replaces C265E8's fixed scan charge with
source timing. All 65 instructions / 2,080 DMA cases and isolated 800-frame
comparisons pass; both builds, twelve CTests and GNU profiling checks pass.
Combined carrier moves 374 -> **446**, crash 213 -> **263**, demo stays 565.
No full rerun: cached raw CPU **38.4011%**, new delta unmeasured; native
memory/chipset/boot **0%**, gate **0/4**. This removes a timing error, not a
CPU dependency. Evidence: `analysis/emulation_removal_slot_timing_batch.json/.md`.
The user's concern about slow removal progress is valid: prioritize connected
native call ownership and deletion next, rather than further adapter expansion.

Latest connected timing batch repairs the C310AA/C12242 compass/selection
interaction. All 39 instructions / 1,248 DMA cases match; three paired
800-frame drawing/RAM probes and every shadow/sandbox call pass. Both builds,
twelve CTests, GNU profiling checks and an 800-frame MSVC/GNU paired demo pass.
Combined demo first non-fade difference moves from 316 to **565** (683 pixels);
carrier/crash still differ at 374/213. No full replay rerun: raw CPU **38.4011%**
is the cached full-suite figure, new delta unmeasured; memory/chipset/boot
**0%**, gate **0/4**. Evidence:
`analysis/emulation_removal_selection_timing_batch.json/.md`.

Historical failure before the slot scan timing repair was the crash
**C265E8,C2D408** interaction: a
220-frame minimized pair first changes indices at 213; neither member alone
differs by 220. Reused isolated matrix captures first differ at demo 584,
carrier 446, crash 263. Preserve native side/depth ownership, ordered events
and original arithmetic; do not tune fixed averages or restore removed CPU
child adapters as game behavior. Continue toward the native frame entry and
remaining removal phases; source timing bridges do not close any removal axis.

Latest timer prerequisite: C25482's fixed 30-cycle charge is source-timed.
All four instructions / 128 DMA cases match, three isolated 800-frame drawing/
RAM comparisons match OFF, and all 27/15/36 shadow/sandbox calls pass. Both
builds, twelve CTests and GNU profiling checks pass. Message/image/timer group
matches demo through 800. ALL still differs at 316; neither half of the full
603-entry registry now reproduces it alone. Minimize the interacting paths
next. No full rerun: cached raw CPU **38.4011%**, new delta unmeasured;
memory/chipset/boot **0%**, gate **0/4**. Evidence:
`analysis/emulation_removal_timer_timing_batch.json/.md`.

Latest image prerequisite: C30EAA's fixed 3,000-cycle blit adapter is now
source-timed and reuses the existing C30F46 plane-stream tail. Its 62 source
instructions / 1,984 DMA oracle cases match. Three isolated 800-frame drawing/
RAM comparisons pass; sandbox passes, shadow has zero mismatches with hardware
interruptions classified separately. The message-plus-image pair matches demo
through 800, but ALL still differs at 316. A 603-entry short subdivision now
isolates C25482's timer charge. Both builds, twelve CTests and GNU profiling
checks pass. No full rerun: cached raw CPU **38.4011%**, new delta unmeasured;
memory/chipset/boot **0%**, gate **0/4**. Evidence:
`analysis/emulation_removal_image_timing_batch.json/.md`.

Latest timing prerequisite: C11BFC's fixed 3,000-cycle message adapter is
now source-timed. All 256 instructions / 8,192 DMA oracle cases match; each
800-frame isolated recording matches OFF drawing/RAM and every shadow/sandbox
call (42/15/74, none incomplete). Both builds and twelve CTests pass. Combined
ON still differs at demo frame 316; fresh short subdivisions isolate C30EAA's
fixed image-blit adapter as another reproducer. Address that real rendering
path next. No full replay rerun: raw CPU **38.4011%** is the cached measurement,
new full-suite delta unmeasured; memory/chipset/boot **0%**, gate **0/4**.
Evidence: `analysis/emulation_removal_message_timing_batch.json/.md`.

The stores-icon prerequisite batch replaces the active `C30A00` / `C30AE2`
fixed-cycle adapters with source-derived steps in
`port/game/glue/glue_hud_stores_step.c`. Runner entry -> machine frame -> recomp
hook -> active port registry reaches these steps during recorded flight. This
removes their fixed-cycle timing error, not their PC/register/bus dependency.
All 90 instructions / 2,880 opcode-oracle cases match; parent and independent
child shadow/sandbox checks pass; all three full isolated recordings match RGB
and sealed final RAM. Both toolchains build and all twelve CTests pass.

Combined ON matches demo01 through frame 564 under strict RGB comparison;
assess subsequent differences with the fade exclusion. The full meter rerun
in `analysis/emulation_removal_meter_after_hud_stores.json/.md` gives raw minimum
CPU-work removal **38.4011%** (+0.0090 percentage points); final-RAM/iteration
acceptance remains open. Memory/chipset/boot cutover remain **0%**, deletion
gate **0/4**. This is validated integration scaffolding, not Phase 1 completion.

Fade exclusion is implemented, not just documented: headless `--index8`
records the selected palette index alongside RGB444. The shared comparator in
`scripts/compare_recomp_frames.py` requires matching indices and excludes only
same-index colours from the source's C08510 fade table. It preserves checks for
upper-palette/other colours, drawing during black frames and unequal frame
counts. The meter, live gate, timing probe and image report use it (NumPy is
required for these Python comparisons).

The full fade-aware report excludes millions of actual fade differences, but
combined ON still fails drawing/state parity: first index differences occur at
demo01 frame 316 (64 pixels), carrier 374 (55,901), crashes 213 (1,905), and
ADF GNU/MSVC 1460 (26). Frame 565 changes indices, so it is not fade-only.
All three full isolated stores recordings pass the new frame/RAM gate. Both
toolchains build, all twelve CTests pass, and capture/profiling remain invisible
to RGB/RAM/stdout/stderr in all three CPU modes on both toolchains. Raw CPU,
native-memory/chipset/boot axes and deletion gate are unchanged by diagnostics.

Phase 1's first connected batch is `C2D408` -> `C1342C`: runner entry -> machine
frame -> recomp hook -> active `glue_C2D408` ->
`update_record_nonclass_matrix` -> `load_record_velocity` -> direct
`update_matrix_side_record()`. The child no longer exchanges Musashi registers
with its parent. Before/after compatibility observations carry ordinary C
values, published only at the outer CPU adapter. `glue_matrix_side_record.c`
and the C1342C registry/prototype entries are deleted. Known static callers
and all five suite profiles establish this ownership within that coverage;
`port/game/native_call_graph.json` preserves it for tooling. The game child has
no custom-register/service/interrupt boundary within this existing atomic
parent call. Guest memory and the parent's fixed timing charge remain; this
batch is not an emulation-free frame body. The isolated 800-frame demo executes
56 direct child calls in 70 parent calls; RGB, indices, RAM and existing counters
are byte-identical to the prior ON runner. Parent shadow: 65 matches, five
incomplete, zero mismatches; sandbox: 70 matches, zero incomplete/mismatches.
The full suite executes 2,809 / 3,341 / 312 / 106 / 106 direct calls (three
recordings, GNU/MSVC ADF). Every previous meter counter, output hash, runner
statistic and fade-aware comparison is unchanged. See
`analysis/emulation_removal_meter_after_native_side.json/.md`: raw CPU removal
**38.4011%**, delta **0.0000 pp**; accepted CPU share remains unavailable because
the combined baseline still fails parity. Memory/chipset/boot **0%**, gate
**0/4**. Next: retire proven C-only leaves inside this native child; keep
original generated reference implementations for OFF/source comparisons.

The next validated batch retires all seven CPU entry adapters under C1342C:
C13A2A, C13A8E, C13B5A, C13BA0, C13C0A, C13C64 and C13CDE. Their known original
callers are native C owners, and none independently dispatches in any full-suite
ON profile. Domain behavior and generated reference code are retained. The
800-frame parent source shadow/sandbox checks still give 65/70 matches, zero
mismatches (five shadow comparisons incomplete). Both toolchains build; twelve
CTests and GNU profiling invisibility pass. All five full-suite output hashes,
counters, direct edge counts and comparisons are identical to the prior batch.
See `analysis/emulation_removal_meter_after_matrix_leaves.json/.md`.
Eight C entries now have no CPU adapter; 606 readable CPU registrations remain
(531 translated plus 75 source-only), with reconstruction still 614 entries.
Raw minimum **38.4011%**, delta **0.0000 pp**, cutover axes **0%**, gate **0/4**.
The 36 deleted guest-access sites belonged to unused adapters: **zero** game
state sites were converted. Current access-site count is 8,952.

The user clarified that full replays should run infrequently because of their
cost. Prefer affected source comparisons and bounded runner checks for routine
batches; reuse these full-suite reports. Run full replays for substantial
behavior changes, milestone acceptance or unresolved failures requiring them.
Next connected closure: C2D408 -> C2DD4E -> C2DE96/C2DEA2. Its parent already
passes depth results as C values. Parent fixed timing is a separate open issue:
the isolated 800-frame C2D408 probe first differs from OFF at frame 584 (488
non-fade pixels), despite matching source call results.

The depth closure is now validated and committed as the next batch: C2DD4E,
C2DE96 and C2DEA2 CPU adapters are retired, including `glue_matrix_depth.c`.
The game owner already calls these functions directly with C values; the outer
parent now profiles its actual depth call. The 800-frame isolated probe runs
69 direct depth calls and 56 side calls. Previous counters and RGB/index/RAM
bytes are identical; source shadow/sandbox counts remain 65/70 matches, zero
mismatches (five shadow incomplete). Both toolchains build and twelve CTests
pass. No full replay was repeated. Evidence:
`analysis/emulation_removal_matrix_depth_batch.json/.md`.
Current inventory is 603 CPU registrations plus 11 direct C entries, still 614
readable entries. Reused full-suite raw minimum **38.4011%**, observed bounded
counter delta **0**; accepted CPU share unavailable, memory/chipset/boot **0%**,
gate **0/4**. Next: investigate the parent's source timing boundary before
extending toward its generated callers; additional leaf retirement alone will
not fix its live frame mismatch.

The 600-frame timing trace identifies the actual outer caller as active
`glue_C25B66_step` -> JSR at C25D9E -> C2D408 -> C25DA4. Across 28 calls, OFF
boundary intervals vary from 10,240 to 20,622 cycles (mean 12,532.57), versus
ON's 9,520-9,530. Three OFF calls cross frame end, versus two ON. These intervals
include JSR/return bus settlement, not just child instruction time. Evidence:
`analysis/emulation_removal_matrix_parent_timing.json/.md`. Recover dynamic
source access/arithmetic/child timing and event boundaries; do not tune the
fixed charge to an observed average or restore reference execution as gameplay.
This bounded discovery changes no axis and does not repeat the full suite.

The native matrix-side helper chain now takes its selected record as a C
argument rather than rereading guest CURRENT_RECORD in six helper bodies.
The owner still publishes the original pointer; math/field accesses are
unchanged. Three isolated 800-frame probes preserve RGB/index/RAM, runner
results and all instruction/device/call counts. Native pointer reads decrease
by 56/0/106 (demo/carrier/crashes), only on page C18000. Parent source shadow
matches 65/66 calls (five/ten incomplete), sandbox 70/76, zero mismatches.
Both toolchains and twelve CTests pass; no full replay was rerun. Evidence:
`analysis/emulation_removal_record_arguments_batch.json/.md`.
Access sites now 8,946, zero state sites converted. Reused raw CPU minimum
**38.4011%**, bounded instruction delta **0**, cutover axes **0%**, gate **0/4**.
This makes native call inputs explicit ahead of the remaining timing work.

## What went wrong and must not recur

About fourteen hours of elapsed work on 2026-10-05 expanded a separate gameplay
implementation in the abandoned top-level `port/` tree. Its standalone proofs
were reported as progress toward emulation independence without establishing
that the playable runner called it. This wasted substantial user time and money.
Some shared host components were integrated and remain useful; this does not
excuse the disconnected gameplay work or its misleading progress reports.

The user clarified that `port/game/` is the active port and requested deletion
of the abandoned sources. Commit `0855f5d6` removed 759 tracked source/header
files and the old build definition. Commit `e70a7014` moved the 48 retained
dependencies into `port/game/` and updated builds, includes and active tools.
Do not recover the deleted implementation as a parallel port or resume its
bootstrap/pose/control-owner plan. Git history is reference evidence only.

The unsupported three-to-six-week estimate was withdrawn. The later "0%" answer described
the absence of a verified complete emulation-free gameplay path, not the amount
of work completed; it was misleading as an overall progress percentage. Neither
number is an accepted project estimate. The measured 614/699 (87.84%) figure is
only readable routine reconstruction within the known inventory. It is neither
whole-game discovery coverage nor emulation-independence completion.

## Required checks before substantial implementation

1. Verify the candidate implementation is compiled by
   `port/recomp/CMakeLists.txt` or `scripts/build_recomp.py`, and trace its caller
   from the actual runner entry. A library link alone does not establish use.
2. Identify the specific CPU, guest-bus, generated-code or chipset dependency
   the batch will remove, and the real startup/menu/flight scenario exercising
   that change. Record the active caller and planned integration in this handoff.
3. Make the first batch a small connected change exercised in the playable
   runner. Do not accumulate more disconnected owner modules and expensive
   component proofs while startup/frame integration remains absent.
4. Validate the changed runner path and the relevant original behavior. Report
   separately what is compiled, what is actually exercised, which dependency
   was removed, and which dependencies remain. Standalone proofs alone do not
   complete a runtime milestone.
5. Reuse existing source and evidence. Run checks appropriate to the affected
   behavior; repeat or broaden them only for new changes, failures or unresolved
   concerns. The retired 249-test suite is not the active runner's acceptance
   suite. Do not spend another long sequence validating the wrong target.

Report useful progress regularly during execution. If evidence shows a batch
is disconnected or targets the wrong build, correct the implementation direction
before continuing it. Do not substitute routine counts, passing test counts,
commit counts or elapsed time for a measured runtime result. Give any future
time/effort estimate with its scope, assumptions and measured basis; do not
invent an overall percentage or extrapolate a deadline from function counts.

## Work scope

Work on `port/game/`, `port/game/glue/` and the actual `port/recomp/` runtime.
The goal is the playable game without Kickstart or emulation. Do not build a
parallel gameplay implementation in the top-level `port/` directory.
A component proof counts as runtime progress only when the active runner uses it.

The abandoned `fa18_port` source/build and disconnected native replacement
components were removed. The 48 retained shared source/header files have now
been moved into `port/game/`: disk/Hunk loaders, map-packet code, its projection
type header and three command type headers. No top-level `port/*.c` or
`port/*.h` files remain. Build paths and active includes use the new locations.
The CMake runtime now compiles the map core with the other game sources; the
Amiga loader library compiles disk/Hunk once. The GNU source glob includes all
of these without duplicate explicit source lists. See `port/README.md`.

Historical `analysis/routines/native_*.md` notes without the `native_c_` prefix
and standalone `tools/recomp/check_native_*` component tools may reference the
removed implementation. They are historical evidence, not a resume plan or
runnable acceptance gates for the active game. Inspect their source dependencies
before using them. The disconnected uncommitted shared-origin batch was dropped.

Removal validation: both active runners build with MSVC Release and GNU;
all eleven active CTests pass. The GNU and MSVC ADF-only launcher checks pass
185 Hunk/8,441 relocation construction, both CPU modes, splash, credits and
keyboard-to-demo, with zero ROM accesses or unsupported services. Hash checks
before relocation confirmed all 697 tracked active-tree files, all 48 retained
sources and the three user-owned guard/allowlist files were unchanged.

Relocation validation also passes both MSVC/GNU runner builds, all eleven active
CTests and both ADF-only launcher checks. All 48 moved implementations are
unchanged apart from four loader include paths; the GNU manifest has 450 unique
existing source files. Loader/OFS checks match all 185 hunks, 8,441 relocations
and 22 resource hashes. The retained RGB4 and viewport comparison tools now use
the moved loaders and exclude checkpoint dependencies on deleted components:
16,384 RGB4 and 32,768 viewport construction/merge calls match their frozen host
implementations. These shared host components are used by the active runner.

## Active runner state

`fa18_romfree` loads the original game/resources from an ADF without Kickstart
or a savestate, using reusable host compatibility/loading in `port/amiga/`,
Interceptor configuration in `port/romfree/`, and guest adapters in `port/os/`.
It still uses Musashi, guest RAM and the chipset model. Removing the abandoned
sources does not establish emulation independence.

The last active function milestone (`05fa7453`) statically recompiles 85 deferred
entries in `port/recomp/generated/recomp_static_deferred.c`. They retain explicit
`STATIC_RECOMP`/`TODO(decompile)` markers and per-entry debt in
`recomp_deferred.json`. The inventory includes 539 readable translated entries
(528 CPU entry adapters and eleven direct C entries without CPU adapters)
and 75 additional readable source-only callable entries. Static compilation
still depends on shared CPU/machine state and is not readable decompilation.
See `analysis/routines/static_recomp_deferred.md` and its checkpoint for the
existing validation scope and original call-graph limitations.

Existing ROM-free verification covers clean startup/demo, menu and active
mission entry/restart, and original configuration save/load. Full mission
outcomes/progression, carrier success and active-flight teardown remain open.
See `analysis/routines/romfree_machine_startup.md`,
`analysis/routines/romfree_exact_followup.md` and
`analysis/routines/romfree_performance_followup.md` for evidence and limits.

## Next work and validation

Before sustained implementation, document the real startup/frame call graph
from `port/recomp/recomp_main.c` through machine execution and the registered
`port/game/glue/ports.c` owners. Inspect `port/game/memory.h`, hardware access
and child hooks for the remaining CPU/bus/chipset dependencies. Distinguish the
44 deferred original ADF entries from the 41 reference wrappers; only the actual
required game paths define the cutover work. The known inventory does not prove
the complete original callback/call graph has been found.

Use that evidence to choose the first small dependency-removal batch in the
active runner. The intended milestone is clean launch into active flight with
Musashi absent from that executable; it is not delivered yet. Chipset removal
and complete game-mode acceptance remain additional requirements. Keep each
change connected to the actual game/runtime call graph and record its measured
result before extending the batch. Reuse retained shared code where called.

Build the MSVC runners with `cmake -S port/recomp -B build/recomp-cmake` and
`cmake --build build/recomp-cmake --config Release`. Headless GNU builds use
`python scripts/build_recomp.py` and `python scripts/build_recomp.py --romfree`.
Run `ctest --test-dir build/recomp-cmake -C Release --output-on-failure`.
For changed gameplay use `scripts/recomp_ports_check.sh` and affected live
RGB/RAM comparisons with `scripts/recomp_live_check.sh`, plus targeted original
routine proofs. Keep known combined readable-C timing debt explicit.

Original disk bytes and source behavior remain the authority; emulator/ROM
runs and sealed captures are validation evidence. Do not alter
`scripts/check_native_build.py`, `scripts/native_frame_count.py` or
`port/native_data_allowlist.txt`. The former `fa18_port` guard is retained as
user-owned historical tooling; its removed target is not an active build.
Preserve unrelated `.vscode/` content. Commit completed validated batches as
previously requested. The emulation-independence goal remains unfinished.

## Emulation removal: measured baseline (2026-10-06)

The active objective is `port/EMULATION_REMOVAL_PLAN.md`, with commits after
validated batches and scoped percentages at each step. Phase 0's call graph is
`analysis/emulation_removal_call_graph.md`; the meter is
`tools/recomp/emulation_meter.py` and its results are
`analysis/emulation_removal_meter.json/.md`.

`--profile` preserves routine-count keys and adds `_emulation`. It counts every
interpreted instruction including ROM, every executed generated/static opcode
helper, original-byte execution between labels and the three OS RTE helper
paths. Guest API accesses are attributed to interpreter, generated, residual,
port, OS, chipset or host, with conservative overlapping 4 KiB RAM-page hits.
Direct DMA and host-compatibility memory accesses are outside that page count:
absence does not establish exclusive native ownership. Chipset counts cover
live blits, Copper instructions, bitplane word fetches, CIA events and interrupt
requests. OS dispatches and port calls/steps expose remaining adapter work.

Baseline suite: full sealed demo, carrier-success and crash recordings, plus
the original ADF keyboard-to-demo sequence on GNU and MSVC. Raw CPU-work shares
removed are respectively **85.5191%, 69.8738%, 69.7856%, 38.3921%, 38.3921%**;
minimum **38.3921%**. **All five fail OFF/ON RGB parity**; all three native ON
final RAM seals also fail. ADF OFF/ON update iteration counts differ. These are
observations of the existing port, not accepted removal progress or whole-game
completion. The report's accepted CPU share is null while parity fails.

Memory cutover: **0%**, 0/8,988 guest access sites converted. Native chipset and
native boot: **0%**. Deletable subsystems: **0/4**. This instrumentation batch
removes no dependency and has **0 percentage-point removal delta**. Phase 0's
measurement deliverables are published; its suite acceptance requirement and
Phases 1-6 remain open.

Validation: both active GNU and MSVC runners build; all **12** active CTests
pass, including new profiling visibility checks when local reference inputs
exist. `tools/recomp/check_emulation_meter.py` passes on both toolchains:
120-frame OFF, ON and interpreter runs retain identical stdout/stderr, every
RGB frame and final RAM with profiling enabled or disabled. Both full ADF-only
launcher checks pass construction (185 hunks/8,441 relocations), both CPU modes,
splash, credits and keyboard-to-demo, with zero ROM accesses/unsupported services.
An independent unmodified `9c062b80` GNU build reproduces the full crash
recording and 2,600-frame ADF sequence in OFF and ON byte for byte (all RGB,
RAM and statistics). Their parity failures therefore predate the counters.
`inventory.py` now finds active `game/*.h` citations: 332 translated routines
have module matches instead of none from the obsolete top-level glob.

Next: retain the failing baseline and investigate the first affected live
transition, using existing source timing evidence. Then remove the guest-PC
child dispatcher for a hardware-free, already-C parent/leaf pair actually
exercised by the runner. Many ON paths still execute instruction-shaped steps;
their readable domain C is often used only by comparison. Do not treat a step
count, decreased bus traffic or an uncalled domain module as native integration.
The hot C02776 reference entry is an OS VBeamPos wrapper, not an original ADF
game helper to recreate; `recomp_deferred.json` records that origin.

Reproduce:

```text
python tools/recomp/emulation_meter.py --launcher-checks
python tools/recomp/emulation_meter.py --scenario demo01 --frames 120 --out build/meter-probe.json
python tools/recomp/check_emulation_meter.py
ctest --test-dir build/recomp-cmake -C Release --output-on-failure
```

The full meter deliberately exits 1 after writing its reports when parity
fails. Probe results are explicitly partial, never a substitute for suite or
whole-game acceptance. Disposable RGB/RAM outputs live in `build/` and are
removed after hashing; sealed recordings and user-owned files are untouched.
