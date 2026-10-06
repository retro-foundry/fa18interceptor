# Plan: removing emulation from the active port

Scope: the playable runners `fa18_recomp` and `fa18_romfree` built by
`recomp/CMakeLists.txt` and `../scripts/build_recomp.py`. Written 2026-10-06
against the tree at `9c062b80`. Read `../CURRENT_PORT_HANDOFF.md` and
`../AGENTS.md` first; this plan is subordinate to the user's latest instructions.

Frame-by-frame comparisons ignore Copper fade, as the user confirmed on
2026-10-06. Fade-only palette/brightness differences do not fail acceptance.
Geometry, drawing order, other rendering differences and gameplay state still
need to match. Strict RGB hashes remain useful diagnostics but cannot by
themselves reject a run under this comparison policy.

"Emulation removed" means one thing only, and it is binary per subsystem: the
shipped executable links none of

1. **Musashi** — the 68000 interpreter and its register file;
2. **the generated translation** — `recomp/generated/recomp_0*.c`,
   `recomp_static_deferred.c`, `recomp_table.c`, which execute original
   instructions through Musashi opcode handlers over a shared register file;
3. **guest memory** — the big-endian 68000 image and `machine/bus.c`;
4. **the chipset model** — `machine/blitter.c`, Copper, `display.c`, CIAs,
   interrupt model.

Everything below is ordered so that each phase removes a named dependency from
a path the runner actually executes, and so that progress is measured rather
than asserted.

## Where the dependency actually is today

Traced from the runner entry:

`recomp/recomp_main.c` parses arguments, boots either a UAE savestate
(`fa18_machine_load_state`) or, under `FA18_ROMFREE_MAIN`, the original ADF via
`romfree/profile.c` -> `amiga/` OFS/Hunk loading -> an explicit process handoff
that places hunks in guest RAM and sets the guest PC. The frame loop is
`fa18_machine_run_frame()` plus `fa18_loop_frame()` (`recomp_main.c:144-145`,
`:218-219`, `:530-531`). Inside a frame the machine steps Musashi; the recomp
hook dispatches translated routines (`recomp/recomp_runtime.c`), and a routine
whose address appears in `game/glue/ports.c` runs recreated C through its glue
instead. Chipset work is serviced only at instruction boundaries in
`fa18_machine_service()`, which is what keeps the two execution engines on one
timeline.

So **every** recreated C routine today is still reached because an emulated PC
arrived at its original address, operates on guest memory through
`game/memory.h` (`rd_*`/`wr_*` on `gaddr`), and drives output by writing
emulated custom registers. That is the shape of the problem: the C exists, but
the emulator is still the thing that calls it, holds its data, and renders for
it.

Counted baseline at `9c062b80` (historical; not a current reconstruction total):

| Quantity | Count | Source |
| --- | ---: | --- |
| Recreated routines registered as ports | 614 | `game/glue/ports.c` rows |
| Recreated `game/*.c` modules | 146 | file count |
| Glue translation units still required | 247 | `game/glue/*.c` |
| Translated routines / instructions | 624 / 34,309 | `recomp/generated/recomp_manifest.json` |
| Statically recompiled deferred entries | 85 (44 original ADF, 41 reference wrappers) | `recomp_deferred.json` |
| `rd_*`/`wr_*` guest-memory sites in `game/` outside glue | 7,858 | grep |
| `rd_*`/`wr_*` sites inside `game/glue/` | 1,130 | grep |
| `custom_write*` sites in `game/` | 175 | grep |
| `wait_blitter()` sites in `game/` | 23 | grep |
| Registered OS service rows in `os/` | 38 | grep |
| Active CTests | 11 | `recomp/CMakeLists.txt` (4) + `amiga/CMakeLists.txt` (7) |
| Sealed native recordings | 3 | `captures/native/` (demo01, qual_carrier_success, qual_fail_crashes) |

`recomp_deferred.json` still records `cpu_and_chipset_required: true`,
`readable_decompilation_complete: false` and
`whole_original_call_graph_complete: false`. The last of those is the honest
bound on any denominator below: code never discovered cannot be counted.

## Measuring progress

The user wants a percentage. A defensible one has to be measured from the
running game, has to be impossible to raise by writing disconnected code, and
has to reach 100% exactly when a subsystem becomes deletable. The design below
satisfies that; it replaces, and must not be confused with, the 614/699
readable-entry count, which measures routine reconstruction only.

### The headline number: emulated-work share removed

For each scenario in a fixed suite, run the same scenario twice and count the
original work still executed by an emulation engine:

```
removed(scenario) = 1 - emulated_instructions_ON / emulated_instructions_OFF
```

where `emulated_instructions` counts instructions executed by Musashi
(interpreted) plus instructions executed inside generated code — which still
runs a Musashi handler per data operation and therefore counts as emulated —
and OFF is the same scenario with `--ports off`, the original path with no
recreated C. The ON run is the real game. Nothing a disconnected module does
can move this number, because only instructions that stop being executed count.

**Report the minimum across the suite, never the mean**, together with the
per-scenario table. The suite is the three sealed native recordings plus the
two ADF-only launcher checks (MSVC and GNU); suite coverage is itself a stated
limit of the measurement, and undiscovered code enters the denominator only
when some scenario exercises it.

### The four axis meters

The headline covers the CPU axis only. Three subsystems remain after it reaches
100%, so report four axes side by side, each 0-100%, each with its own
denominator:

| Axis | Metric | 100% means |
| --- | --- | --- |
| **A. CPU** | emulated-work share removed, as above | no scenario executes a Musashi instruction |
| **B. Memory** | guest-bus accesses per frame issued from recreated C and its glue, against the `--ports off` baseline; plus converted `rd_*`/`wr_*` sites (0 of 8,988 today) | game state lives in C objects; `bus.c` unreferenced |
| **C. Chipset/IO** | chipset operations per frame actually serviced (blitter ops, Copper instructions, bitplane fetches, CIA/interrupt events) against the baseline | native drawing, audio and timing; `machine/` unreferenced |
| **D. OS/boot** | service dispatches per scenario reaching guest-resident OS wrappers or `m68k` state, against the baseline; plus the boot handoff | native entry, no guest PC, no hunk placement into guest RAM |

### The gate that keeps the percentages honest

Alongside the meters, report **deletable subsystems: n/4** — the number of the
four subsystems above that the runner can actually be linked without, proved by
building a configuration that omits those objects and passing the suite.
Today this is **0/4**, and it stays 0/4 until a phase completes end to end. A
percentage reported without this gate beside it is not a progress report.

### Building the meter (Phase 0 deliverable)

- Add cheap counters behind the existing `--profile` JSON: per-engine executed
  instruction counts (extend `FA18RecompStats`, which already carries
  `interpreted_game`, `generated_cycles`, `dispatches`); bus accesses split by
  originating engine in `machine/bus.c`; chipset operation counts in
  `machine/machine.c`, `blitter.c`, `display.c`; service dispatch counts in
  `os/service_dispatch_adapter.c`.
- Add `tools/recomp/emulation_meter.py`: runs the suite OFF and ON, joins the
  profile JSONs, writes `analysis/emulation_removal_meter.json` and a markdown
  table, and prints the four axes, the per-scenario minimum and the n/4 gate.
- Record the baseline in `CURRENT_PORT_HANDOFF.md` and commit it. Every later
  phase reports the meter delta it produced; a batch that moves no axis is
  reported as moving no axis.
- While doing this: `tools/recomp/inventory.py` still globs `port/*.h`, which no
  longer exist after `e70a7014`. Fix or retire it rather than feeding stale
  output into the meter.

## Phases

Each phase ends with: the suite passing, the meter delta recorded, and a commit.
Within a phase, work in small batches that are exercised in the runner before
the next batch starts, per `AGENTS.md`.

### Phase 0 — Call graph, meter, baseline

Produce the written startup/frame call graph from `recomp_main.c` through
`fa18_machine_run_frame` to the registered owners, as `CURRENT_PORT_HANDOFF.md`
requires, and the meter above. No behaviour changes.
Exit: baseline numbers published; 0/4 gate stated.

### Phase 1 — Native call graph (axis A)

First connected batch complete: `C2D408` calls `C1342C` directly through the
game owner, with compatibility values published at the parent CPU boundary.
The leaf's CPU adapter/registration are removed; the original generated OFF
implementation remains. All five scenarios exercise the direct edge and retain
identical pre-change outputs/counters. This removes a shared-register child
dependency, but moves no measured axis: raw CPU **38.4011%**, delta **0.0000 pp**,
memory/chipset/boot **0%**, deletion **0/4**. Combined parity remains failing,
so Phase 1's exit gate is still open. Evidence:
`../analysis/emulation_removal_meter_after_native_side.json/.md`.

The seven CPU helper adapters below C1342C are also retired after known-call
ownership validation, parent source checks and unchanged results in all five
scenarios. There are now eight direct C entries and 606 readable CPU registry
rows; reconstruction remains 614 entries. Raw percentage and all cutover axes
are unchanged. Removing 36 unused adapter access sites is not memory cutover.
Evidence: `../analysis/emulation_removal_meter_after_matrix_leaves.json/.md`.
Following the user's instruction, use targeted comparisons and bounded runner
checks for routine batches; reserve full replays for substantial changes,
milestone acceptance and unresolved failures.

C2DD4E and its C2DE96/C2DEA2 helpers are now also C-owned without CPU adapters.
The bounded 800-frame parent probe executes 69 depth calls and preserves prior
outputs/counters; source comparisons and both builds pass. There are 11 direct
C entries plus 603 CPU registrations. No full replay was repeated: reuse the
preceding **38.4011%** raw minimum, with zero observed bounded counter delta,
cutover axes **0%**, deletion **0/4**. The parent's source timing mismatch still
requires work. Evidence: `../analysis/emulation_removal_matrix_depth_batch.json/.md`.

The matrix-side owner also passes its selected record explicitly to six C
helpers, removing their guest CURRENT_RECORD lookups. Three bounded probes
and parent source comparisons preserve behavior and instruction/device counts;
native guest reads decrease by 56/0/106. No full replay was repeated. Raw CPU
estimate remains **38.4011%** from the last full suite; cutover axes **0%**,
deletion **0/4**. Removing six lookup sites is not conversion of state to C
objects. Evidence: `../analysis/emulation_removal_record_arguments_batch.json/.md`.

The live flight parent's C25D9E matrix call now selects the game C owner with
ordinary copied arguments instead of dispatching a C2D408 CPU entry. Its CPU
entry adapter/registry are removed; outer CPU timing/result publication and
the existing 9,500-cycle atomic timing debt remain. Three bounded 800-frame
parent and combined before/after pairs preserve RGB, indices and final RAM;
combined dispatches decrease by 68/15/75. Both builds, twelve CTests, the DMA
parent oracle and argument/IRQ/reset boundary fixture pass. There are twelve
C-owned entries and 602 readable CPU registry rows. Bounded instruction delta
is zero; cached full raw CPU **38.4011%**, accepted percentage unavailable,
memory/chipset/boot cutover **0%**, deletion **0/4**. Combined source parity
still fails at demo/carrier/crash 565/446/263; Phase 1 is incomplete. Evidence:
`../analysis/emulation_removal_matrix_dispatch_batch.json/.md`.

Remaining C routines call each other by returning into the dispatcher. Give each
recreated routine a direct C entry point and let a C caller call its C callee
directly, keeping glue only for callers that are still generated code.
`PORT.md` already prescribes the end state: when every caller of a routine is
C, its glue is deleted.

- Start at the leaves of the already-C call graph, where both caller and callee
  are registered ports and neither takes an interrupt nor touches hardware
  inside the call — those are exactly the calls `shadow` already compares, so
  the oracle exists.
- Work outward toward the main loop. The main loop itself is the last step:
  once the frame body is C calling C, the dispatcher is reached only for the
  remaining generated entries.
- Measured by: dispatches per frame, glue files deleted, axis A.

Exit: the frame body is native, and the only emulated instructions left are the
deferred entries and undiscovered code (Phase 5).

### Phase 2 — Memory cutover (axis B, PORT.md stage F)

Replace `gaddr` plus `rd_*`/`wr_*` with real C structs and pointers, as
`game/memory.h` anticipates.

- Per memory region, prove with the Phase 0 counters that no generated or
  interpreted code reads or writes it in any suite scenario; only then convert
  it. Regions still shared with generated code convert last.
- Convert region by region, not file by file: the unit of safety is the data,
  not the module.
- Keep word-exact arithmetic; the conversion must not change behaviour, and
  live RGB/RAM comparison remains the gate.

Exit: the guest image holds only what still-generated code uses, and axis B
states that remainder exactly.

### Phase 3 — Native IO backend (axis C, PORT.md stage F)

Replace chipset-mediated output with direct host output behind the interfaces
`game/hardware.h` already funnels everything through: `custom_write()`,
`custom_write_ptr()`, `wait_blitter()` — 175 and 23 sites, a small and already
centralised surface.

- Reimplement the drawing those writes express (bitplane targets, blits, Copper
  colour and display setup) as direct framebuffer operations, keeping the
  existing RGB444 frame as the comparison surface so the sealed recordings stay
  the oracle.
- Audio (Paula) is currently not modelled at all; it is new work here, not a
  removal, and must be reported as such rather than counted on axis C.
- Frame timing moves from CIA/beam events to the host pacer already present in
  `recomp/frame_pacer.h`.

Exit: a scenario renders identically with the `machine/` chipset services unused.

### Phase 4 — Boot and OS (axis D)

- Replace the ADF -> Hunk -> guest-RAM -> guest-PC handoff in `romfree/` with a
  native entry that calls the recreated start-up directly. `amiga/` disk, OFS
  and Hunk loading is host code and stays.
- Reduce the 38 service rows to those the native game still calls; services that
  exist only because guest code made a library call disappear with their caller,
  so most of this follows Phases 1-3 rather than leading them.
- Save/load of the original configuration file must keep working against the
  same on-disk bytes.

Exit: no guest PC, no guest stack, no supervisor state.

### Phase 5 — Residual original code (closes axis A)

- The 44 deferred entries backed by original ADF code are real game behaviour
  and need readable decompilation; the 41 reference wrappers should be shown
  unreachable in the native build and dropped, not reimplemented.
- `whole_original_call_graph_complete` is false: before claiming completion, run
  the suite with the interpreter made fatal rather than permissive, so any
  undiscovered path fails loudly instead of falling back silently.
- Then delete Musashi, `recomp/generated/`, the dispatcher and the glue
  directory.

### Phase 6 — Deletion and acceptance

Remove the four subsystems from the build, not merely from the call path — the
gate is a link without them. Acceptance is the whole game, not the suite:
startup, menu, every mission mode, mission outcomes and progression, carrier
success, flight teardown, save/load. The handoff lists mission outcomes and
progression, carrier success and active-flight teardown as unverified even under
emulation; they must be verified before any completion claim.

## Ordering and why

Phase 1 precedes Phase 2 because a native call graph is what makes "no generated
code touches this region" provable. Phase 2 precedes Phase 3 because the drawing
code must own its own data before it can own its own output. Phase 4 trails
because most OS services die with their guest callers. Phase 5 is last because
the deferred entries are the expensive, low-information residue and should not
block the structural work.

## What this plan deliberately does not do

- It does not estimate a completion date. Any estimate must come with its scope,
  assumptions and measured basis, per `AGENTS.md`.
- It does not reuse the 614/699 figure as a progress percentage.
- It does not create a parallel implementation outside `game/`, and adds no C
  sources or headers at the top level of `port/`.
- It does not treat a passing standalone component test as runtime progress;
  only the meter and the suite move the number.

## Execution record — 2026-10-06

Phase 0 call graph and measurement deliverables are published in
`../analysis/emulation_removal_call_graph.md` and
`../analysis/emulation_removal_meter.json/.md`. The runner's existing profile
now includes all executed interpreter/generated/residual opcode instructions,
including three OS RTE helpers, bus API origin/page counts, chipset operations,
service dispatches and native port calls/steps. The inventory header glob now
uses the active `game/` directory.

The full five-scenario baseline's raw minimum CPU-work share removed is
**38.3921%**. Every scenario fails RGB parity and the three native ON final RAM
seals fail; ADF ON/OFF iteration counts differ. The accepted CPU share is
therefore unset. Memory conversion, native chipset and native boot remain
**0%**; deletion gate **0/4**; batch removal delta **0 percentage points**.

Both active toolchains build, all twelve active CTests pass, both full ADF-only
launcher checks pass, and profiling is invisible to RGB/RAM/stdout/stderr in
OFF/ON/interpreter runner checks on both toolchains. An independent unchanged
`9c062b80` build reproduces the full crash replay and ADF startup failures byte
for byte. This validates the instrumentation while retaining existing combined
ON timing debt. Phase 0's suite acceptance prerequisite remains open, as do
Phases 1-6. No emulation subsystem was removed or declared complete.

RAM-page counters observe the guest API. Direct DMA and host compatibility
memory accesses are separate and their absence from those counters must not
be used to claim exclusive native ownership in Phase 2.

### Stores-icon timing prerequisite

The runner's `C30A00` / `C30AE2` stores-icon entries previously executed the
existing C owners with fixed 900/500-cycle charges. That coupled drawing to
the wrong beam timeline: their isolated ON replay first differed at frame
424 (34,144 pixels). The active registry now calls source-derived instruction
steps in `game/glue/glue_hud_stores_step.c`, including the shared early return.
This is a temporary CPU adapter repair, not a native-call-graph cutover.

Both GNU/MSVC runners build; all twelve active CTests pass. The active opcode
oracle matches 90 instructions / 2,880 cases including bus contention. Parent
and independent child shadow/sandbox probes match. The full isolated live gate
matches all three recordings' RGB streams and sealed final RAM. Combined ON
now matches demo01 through frame 564 under strict RGB comparison; frame 565
must be assessed with the Copper-fade exclusion before it is treated as a
rendering failure.

`../analysis/emulation_removal_meter_after_hud_stores.json/.md` records the full
five-scenario rerun: raw minimum CPU work removed **38.4011%**, a **+0.0090
percentage-point** observation. Its strict RGB parity result is diagnostic
under the user's fade exclusion; final-RAM/iteration gates remain open.
Memory/native-chipset/native-boot cutover remain **0%**, deletion gate **0/4**.
No phase or emulation subsystem is completed by this repair.

### Copper-fade comparison implementation

Both runners now expose optional `--index8` output from the existing display
model. The shared comparison checks those selected indices even during black
frames. An RGB difference is excluded only when its unchanged index is 0-15
and both colours belong to that index's original fade sequence at C08510;
upper-palette and unrelated colour changes remain failures. Unequal frame
counts are reported as parity failures, with the common prefix compared.
The meter, live gate, timing probe and comparison image report share this rule.
The historical strict RGB baseline remains in git; the after-stores report now
records the fade-aware results.

The full suite excludes 5,171,713 fade pixels in demo01, 2,061,205 in the carrier
run, and 626,291 in each ADF run. Combined ON still fails drawing/state parity:
the first differing indices are demo01 frame 316 (64 pixels), carrier frame
374 (55,901), crashes frame 213 (1,905), and both ADF runs frame 1460 (26).
Demo01 frame 565 changes indices too; it is not a fade-only difference.
Different native replay frame counts and final RAM seals remain visible.

Both toolchains build, all twelve CTests pass, diagnostic capture preserves
RGB/RAM/stdout/stderr in OFF/ON/interpreter checks on both toolchains, and the
three full isolated stores replays pass the new gate. Policy regression cases
cover allowed fade, unrelated/upper-palette colours, equal-RGB index changes,
black-frame drawing changes and unequal lengths. The full native captures'
RGB/RAM hashes, all profile counters and runner stats match the pre-diagnostic
batch; their completed captures were reused after repairing length handling.
This batch changes no removal axis: raw CPU **38.4011%**, memory/chipset/boot
**0%**, deletion gate **0/4**, removal delta **0 percentage points**.

### Cockpit message timing prerequisite

The real C0EFD4 frame-update step calls C11BFC at C0F12C. Its fixed
3,000-cycle charge independently reproduces the demo frame-316 index mismatch.
The live registry now uses source-timed message boundaries. All 256 original
instructions / 8,192 DMA oracle cases match. Isolated 800-frame demo/carrier/
crash drawing and RAM match OFF; all 42/15/74 shadow and sandbox calls pass.
Both toolchain builds and all twelve CTests pass.

Combined ON still differs at frame 316; a second 320-frame subdivision isolates
C30EAA. This repair removes a timing error, not a CPU/guest-bus dependency.
No full suite rerun: last measured raw CPU **38.4011%** is cached, new full-suite
delta unmeasured; accepted CPU share unset; memory/chipset/boot **0%**, gate
**0/4**. Evidence: `../analysis/emulation_removal_message_timing_batch.json/.md`.

### Image blit timing prerequisite

C30EAA's fixed image charge is replaced by source boundaries for clipping,
blitter waits and four ordered submissions, reusing the C30F46 timing tail.
All 62 instructions / 1,984 DMA cases match; three isolated 800-frame drawing/
RAM comparisons pass. Shadow has zero mismatches (1/3/7 incomplete); sandbox
passes all calls. Both builds, twelve CTests and GNU profiling checks pass.
Message-plus-image timing matches demo through 800. ALL still differs at 316;
a fresh short subdivision over all 603 entries isolates C25482.

This repairs integration timing, not CPU independence. No full suite rerun:
cached raw CPU **38.4011%**, new full-suite delta unmeasured, accepted CPU share
unset; memory/chipset/boot **0%**, gate **0/4**. Evidence:
`../analysis/emulation_removal_image_timing_batch.json/.md`.

### Signed-byte timer timing prerequisite

C25482's fixed 30-cycle adapter independently reproduced demo frame 316. It
now preserves original signed-byte test, decrement and return boundaries.
All four instructions / 128 DMA cases match; three isolated 800-frame drawing/
RAM comparisons match OFF and all 27/15/36 shadow/sandbox calls pass. Both
builds, twelve CTests and GNU profiling checks pass. Message/image/timer group
matches demo through 800. ALL still differs at 316; neither complete registry
half reproduces it alone now. Minimize the remaining interaction next.

This is a temporary CPU timing repair. No full suite rerun: cached raw CPU
**38.4011%**, new delta unmeasured; accepted CPU share unset; memory/chipset/boot
**0%**, gate **0/4**. Evidence:
`../analysis/emulation_removal_timer_timing_batch.json/.md`.

### Compass/selection timing interaction

The C310AA/C12242 pair independently reproduced demo frame 316, while either
entry alone passed that bounded interval. Source-timed multiply/divide/shift
and selection cleanup boundaries now replace both fixed charges. All 39
instructions / 1,248 DMA cases match; three paired 800-frame drawing/RAM probes
and every shadow/sandbox call pass. Both builds, twelve CTests, GNU profiling
checks and an 800-frame MSVC/GNU paired demo pass.

Combined demo first non-fade difference moves from 316 to **565** (683 pixels),
with Copper fade excluded. Carrier/crash still differ at 374/213. A minimized
220-frame crash pair **C265E8,C2D408** reproduces 213; each alone passes that
interval. Reused isolated matrix captures first differ at 584/446/263.
Address original event timing without restoring retired CPU children or tuning
average fees. This is integration repair, not native frame completion.

No full suite rerun: cached raw CPU **38.4011%**, new delta unmeasured; accepted
CPU share unset; memory/chipset/boot **0%**, gate **0/4**. Evidence:
`../analysis/emulation_removal_selection_timing_batch.json/.md`.

### Flagged-slot timing prerequisite

Latest validated prerequisite replaces C265E8's fixed scan charge with
source timing. All 65 instructions / 2,080 DMA cases and isolated 800-frame
comparisons pass; both builds, twelve CTests and GNU profiling checks pass.
Combined carrier moves 374 -> **446**, crash 213 -> **263**, demo stays 565.
No full rerun: cached raw CPU **38.4011%**, new delta unmeasured; native
memory/chipset/boot **0%**, gate **0/4**. This removes a timing error, not a
CPU dependency. Evidence: `../analysis/emulation_removal_slot_timing_batch.json/.md`.
The user's concern about slow removal progress is valid: prioritize connected
native call ownership and deletion next, rather than further adapter expansion.
