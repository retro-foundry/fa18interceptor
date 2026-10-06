# Plan: removing emulation from the active port

Scope: the playable runners `fa18_recomp` and `fa18_romfree` built by
`recomp/CMakeLists.txt` and `../scripts/build_recomp.py`. Written 2026-10-06
against the tree at `9c062b80`. Read `../CURRENT_PORT_HANDOFF.md` and
`../AGENTS.md` first; this plan is subordinate to the user's latest instructions.

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

Current counted state of the active tree:

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

Today C routines call each other by returning into the dispatcher. Give each
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
