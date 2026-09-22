# F/A-18 Interceptor: runtime reconstruction

Read **[STATUS.md](STATUS.md)** first when continuing this project.

GAME.md has information about the game itself.

The authority is the supplied 1988 Amiga disk, executed by the pinned
[Engine9000 v0.62-alpha](https://github.com/alpine9000/engine9000-public/tree/v0.62-alpha)
Amiga core. The goal is readable, byte-exact 68000 assembly backed by recorded
play, real frames and observed routine contracts. Raw Ghidra P-code is an
address-linked analysis aid, not the final source or proof of a routine's meaning.

## Record a session

The first bounded run is the scripted demonstration. It records the one menu
selection needed to enter Demonstration Flight and then supplies no flight
controls:

```powershell
python scripts/capture_attract.py --name attract_run001 --frames 9000
```

`9000` PAL frames is three minutes. This is the initial display, timing,
renderer and scripted-control oracle. Its output is sealed and replayable.

For later human runs:

```powershell
python scripts/record_run.py --name run001
```

The launcher opens the stock Engine9000 window, restores the preserved menu,
and records keyboard, mouse and controller events. Click the emulated screen
to give the game input. Choose **2: Free flight** for the first short session.
Play for about 2–3 minutes: fly, steer, change views, and try controls you know.
Then close Engine9000 and tell the agent the run is finished. Breadth matters
more than a successful flight. Later sessions can target training, missions,
weapons, landing and failure paths according to measured gaps.

The interactive recorder uses **F8** only to restore the initial savestate and
disables bare-F8 Restart. Function-key coverage is generated through the native
Engine9000 replay script, not Windows keyboard injection. Avoid Restore,
Rewind, Reset, Warp and configuration changes during an interactive recording.
To release captured mouse input, upstream's binding is Ctrl+Alt+Left Alt.

Seal a closed run, then replay a chosen window:

```powershell
python scripts/finalize_run.py captures/run001
python scripts/engine9000_bridge.py --restore captures/run001/initial_state.bin `
  --config captures/run001/config.uae --playback captures/run001/playback.e9k `
  --frames 300 --output build/run001_first300
```

`last_input_frame` in `run.json` is the last recorded event, not a guessed exit
frame. Every run retains its own initial state, exact configuration, raw events
and tool hashes. The originals are never edited. Play uses a write-protected
private ADF; the game's optional flight-log disk request is skipped.

## Focused evidence and P-code

```powershell
python scripts/engine9000_bridge.py --restore captures/baseline_menu/state.bin `
  --frames 120 --trace-frames 2 --output build/menu_focus_new
./scripts/import_ghidra.ps1 -Capture build/menu_focus_new -Project Fa18FocusNew
```

Output directories and new Ghidra project names must be unused. The bridge uses
the release's unmodified `ami9000.dll`, not a replacement CPU implementation.
`trace.jsonl` keeps each instruction's PC, bytes, registers, next PC, chipset
frame and cycle counter. `custom_writes.jsonl` keeps CPU/Copper provenance.
The importer checks every observed RAM instruction against the snapshot, then
disassembles only those starts. Call targets remain structural until explained.

`pcode/raw/menu/` is the first stable export. Every instruction has its runtime
address, RAM bank/file offset, bytes, function, flows and raw P-code varnodes.
See its authority manifest for the exact snapshot and trace. Ghidra's installed
68000 family language is a 68040 superset; decoded lengths are cross-checked
against Capstone's 68000 mode. This is not a claim of complete instruction
semantics validation.

For each routine, work from a small packet: snapshot + frame + entry/exit +
callers + registers/state + hardware effect + a narrow assertion. Promote names
only after static and runtime evidence agree. Preserve literal unknown values
with an explicit unknown meaning rather than inventing a symbolic explanation.

## Keep checks cheap

- Two independent 120-frame restores compare RAM, registers, cycles, video and
  audio in about one second total.
- `python scripts/check_breakpoint.py` checks one observed runtime breakpoint.
- `python scripts/verify_reconstructions.py` is the byte gate: it assembles every
  slice in `source_amiga/observed/`, compares it with the runtime bank at its
  `ORG`, warns on overlapping slices, and writes `analysis/coverage.json`. About
  two seconds. Run it after every source reconstruction change.
- A two-frame menu instruction trace takes about 1.6 seconds; Ghidra import and
  export take roughly four seconds. Do not run full-session instruction stepping
  as a routine test.
- Compare assembled bytes only for the routine/bank changed. Extend checks when
  a real uncertainty calls for it.

Single-stepping repeatedly polls the core frontend. Held-key/autorepeat parity
needs validation before using stepped input traces as definitive evidence.
Normal human recording and full-frame replay do not use instruction stepping.

## Coverage

Coverage is reported against three denominators, because no single one is
honest. The gate prints all three and writes them to `analysis/coverage.json`:

- **Of observed-executed code.** Bytes some capture has actually run. Only these
  can carry a behavioural claim under this project's evidence rules.
- **Of plausible instruction bytes.** All CODE hunk bytes minus segments with
  positive data evidence: never executed in any P-code export, and referenced by
  reconstructed source only as data operands, never as branch or call targets.
  AmigaDOS Hunks do not separate code from initialised data the linker emitted
  as CODE, so the raw CODE total understates progress.
- **Of the whole game ever exercised.** How much resolved CODE has executed at
  all. This is the real constraint: a routine no capture has entered cannot be
  reconstructed under these rules, only guessed at.

A segment that has never executed is *not* thereby data. It is equally likely to
be a subsystem no recording has reached, and that ambiguity is preserved rather
than resolved by assumption.

```powershell
python scripts/plan_captures.py
```

This ranks what is still dark into `analysis/capture_targets.md`: segments
reached by a reconstructed branch but never run, segments under half exercised,
and segments never touched at all. It aims recording sessions at measured gaps
instead of guesses. Reconstruction cannot outrun scenario coverage.

## Code, data, and display boundaries

Run `python scripts/inventory_runtime_regions.py` to regenerate the auditable
[runtime-region inventory](analysis/runtime_region_inventory.md) and its
machine-readable companion. It preserves original `HUNK_CODE`, `HUNK_DATA`,
and `HUNK_BSS` ownership, separately records exact inline data ranges proved
inside executable hunks, and lists Copper-visible buffers as mutable display
targets. It deliberately does not call a never-executed CODE hunk data, or a
visible bitplane buffer a static graphics asset.

## Renderer-observed model identification

The model-discovery output is deliberately based on the game renderer rather
than inferred mesh links. [The model identification catalogue](analysis/data/model_identification_catalog.md)
lists the traced source/controller paths and links every X-Y/X-Z/Y-Z sheet.
[The contact gallery](analysis/plots/model_identification_gallery.png) is the
quick visual entry point.

[Model-data export boundaries](analysis/data/model_export_boundaries.md)
separates traced immutable geometry from unresolved renderer families and
mutable workspaces that must not be exported as source models.

[Renderer model extraction status](analysis/renderer_model_extraction_status.md)
is the concise current handoff for the `$C45BEA` flight-object result, the
Golden Gate pylon/deck candidate, and remaining mixed bridge packets.

Filled faces are captured at `$C2FF48` and, where needed, before orientation
or clip rejection at `$C2005C`/`$C2469E`; line segments are captured at
`$C212B0`. Coordinates are mutable renderer workspaces, so every sheet states
its scenario and only connects vertices within an observed renderer record.
The [run035 Golden Gate viewport interval](analysis/data/run035_golden_gate_red_viewport_interval.md)
is an explicitly cockpit-free raster landmark for future face-to-pixel
correlation; it is not yet a terrain-model or LOD identification.

## Controls recorded as documentation

`GAME.md` is the authority until raw-keycode tracing confirms each mapping:
F1-F10 throttle (repeat F10 for afterburner), `=`/`-` incremental throttle,
Backspace airbrake, cursor keys pitch/roll, comma/period rudder, `G` gear,
`A` hook, `H` HUD, `J` ECM, `C` chaff, `F` flare, `M` map, `R` radar range,
`T` target, Return weapon select, Space fire, Shift+F rescue pod, Shift+E
eject, `P` pause, Esc restart, Shift+Esc return to menu, keypad camera views,
and brackets zoom. The full manual says `J` for ECM; a later fan sheet says
`K`, so that one is explicitly unverified.

See [analysis/memory_map.md](analysis/memory_map.md) for the observed runtime
memory map and its evidence rules.

## Pilot-log experiment

Rookie (`R`) is the controlled persistent-state baseline: its supplied flight
log has the first three combat missions complete. Create a disposable,
write-enabled copy—never alter `local/media/fa18.adf`—then save Rookie and one
single-variable pilot change. For example:

```powershell
python scripts/prepare_pilot_log_experiment.py --name rookie_baseline
python scripts/compare_pilot_log_adf.py captures/pilot_log_experiments/rookie_baseline/baseline.adf captures/pilot_log_experiments/rookie_baseline/pilot_log_working.adf --output analysis/pilot_log_rookie_diff.json
```

The resulting UAE config is in that experiment directory. Use the raw-byte
diff as an evidence lead, then identify the changed filesystem record and
cross-check its live RAM writers/readers before assigning qualification or
mission-completion semantics.

## Local dependencies

Python 3.13 with Pillow and Capstone; Java; local Ghidra 12.0.4 DEV; and the
downloaded Windows Engine9000 release. `scripts/setup_capture.py` creates the
private config/media and records hashes in `local/toolchain.json`. Before
running it, set `FA18_KICKSTART_ROM` to the path of a legally obtained
Kickstart 1.3 ROM image.
`AMIGA.md` supplies hardware context; original bytes and traces decide game
behavior. `../quest` is the reference workflow. No native port is being built.
