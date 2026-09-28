# Recreating a retro game as C source, end to end

A playbook drawn from F/A-18 Interceptor (Amiga, 68000, 1988), for the next
game on a 68000, Z80 or 6502 machine. It covers the whole path: original
disk, emulator, recordings, analysis (Ghidra and P-code), mechanical
translation, frame parity, readable C, OS replacement, and a native build
with no emulation left.

The one rule: **get the whole original program running as C first, then make
it readable. The build is never dark.** Every step below keeps a working,
rendering game.

## 0. What went wrong the first time

The first ~2,000 commits ported the game bottom-up, one routine slice at a
time, each with its own contract test. After about 400 C modules and 208
passing tests, the native build still drew zero pixels at the first cockpit
frame. The causes, so they are not repeated:

- **No end-to-end metric.** Tests and "bytes reconstructed" rose while
  "frames that match" stayed at zero.
- **Scaffolding was forbidden.** No placeholders, no emulator-derived state,
  no display until everything was finished. That made the first visible
  result the last thing built.
- **Understanding was a prerequisite for running.** Naming a routine,
  proving it and exporting its P-code came before any code executed.
- **Handoffs grew without limit** (5,000+ lines), so each session spent its
  attention on history.

## 1. Setup (day 1)

1. **Keep the original media untouched** (here, the ADF), plus the ROM the
   game needs (Kickstart 1.3). Record SHA-256 hashes of both.
2. **Pin a scriptable emulator build.** This project used an Engine9000 fork
   (UAE core) driven from Python through ctypes
   (`scripts/engine9000_bridge.py`). It must be able to:
   - restore a savestate and replay recorded input frame by frame;
   - save a savestate, Chip/Slow RAM dumps and registers at any frame;
   - screenshot any frame;
   - single-step with a per-instruction trace (PC, bytes, registers) and a
     log of custom-chip writes.
3. **Check determinism.** Two replays of the same input must give identical
   RAM, video and audio hashes. Nothing downstream works without this.

## 2. Record and seal scenarios (day 1-2)

- Record short human sessions as replayable input streams
  (`scripts/record_run.py`), starting from a fixed savestate. Seal each one
  with its start state, input file, config and tool hashes
  (`scripts/finalize_run.py`); sealed runs are read-only.
- Cover the game's modes early: menu, demo, a normal session, a crash, a
  save. Each recording becomes a parity test later.
- Pick one recording as the first target (here, run075: menu, demo selected
  by key `1`, cockpit and flight).

## 3. Load the program and map memory (day 2)

- **Parse the executable format** (Amiga Hunks here: CODE/DATA/BSS segments
  and 32-bit relocations; `scripts/parse_hunk.py`, `port/hunk.c`). Record
  where each segment lands at run time (`scripts/resolve_hunk_runtime.py`):
  translate the *loaded, relocated* image, which is what actually runs.
- **Start a memory map** (`analysis/memory_map.md`): what lives where (code,
  tables, workspaces, stacks, display buffers). Fill it as analysis goes;
  it becomes the global-variable list of the C source.
- A RAM dump from a savestate is the easiest source of the loaded image.

## 4. Analysis track: Ghidra and P-code (runs in parallel)

The analysis track exists to *understand* the code: to name routines,
variables and struct fields, and to separate code from data. The port does
not wait for it. Start it now and let it run alongside steps 5-8.

### Ghidra import

- Import the loaded image once into a Ghidra project with the right
  processor (`68000:BE:32:default`), at its runtime address, headless:
  `analyzeHeadless <project dir> <name> -import chip.bin -loader BinaryLoader
  -loader-baseAddr 0 -processor 68000:BE:32:default ...`
  (`scripts/import_ghidra.ps1`, scripts in `scripts/ghidra/`).
- Seed function starts from the executable's entry, the relocation targets,
  and the PCs seen in emulator traces; then run full auto-analysis once.
- Validate that Ghidra's instruction lengths match the CPU core's decoder on
  every traced instruction (`scripts/prepare_ghidra.py` checked bytes; also
  cross-check with a second disassembler).

### P-code export

- Export P-code **once for the whole program**, not per capture: for every
  instruction, its address, bytes, owning function, flows and raw P-code
  operations (`ExportFa18Pcode.java`, output like `pcode/raw/<name>/`:
  `instructions.pcode.jsonl`, `functions.json`, `observed.asm.txt`).
- Use it for:
  - routine reports (`analysis/routines/<address>_<name>.md`): contract,
    callers, state read/written, evidence;
  - cross-references when naming globals and fields;
  - an independent semantics check: P-code is Ghidra's instruction model,
    separate from the CPU core the translator uses.
- Do **not** make per-capture P-code exports a gate for porting. Here, one
  export per capture across ~120 captures cost weeks and covered only 16% of
  the code; the translator decodes everything directly.

### Optional: byte-exact assembly

- Reconstructing routines as assembly that reassembles byte-for-byte
  (`source_amiga/observed/`, `scripts/verify_reconstructions.py`) is a good
  way to record understanding and prove code/data boundaries. It is a
  documentation product; the C port does not depend on it.

## 5. Machine layer (days 2-4)

A small native model of the hardware the game touches, plus a savestate
loader, so native code can start from **any** recorded frame.

- **Memory map and bus**: RAM, ROM and custom-chip registers, with the
  machine's mirroring.
- **Custom chips.** Here: the blitter (area, fill, descending, line modes),
  Copper, bitplane display, interrupts and CIAs (timers, TOD, keyboard).
  Port the emulator's own algorithms; details matter. For example, Agnus
  ignores bit 0 of the blitter modulos, and without that every fill row
  drifted a byte.
- **Savestate loader** (UAE `ASF` chunks here: CPU, CHIP registers, CIAA/B,
  CRAM/BRAM RAM, BORO boot ROM area).
- **A proven CPU interpreter** as reference and fallback (Musashi for the
  68000, vendored in `tools/musashi/`).
- **One timeline.** Advance scanlines and take interrupts only at
  instruction boundaries, in one service routine used by both the
  interpreter and translated code. Otherwise the two diverge.
- **Milestone:** from a snapshot, the interpreter on your machine layer
  renders frames that match the emulator.

Debugging tools that paid off:

- **Lockstep compare.** Run the native interpreter against the emulator's
  instruction trace and report the first differing register
  (`scripts/recomp_lockstep.py`). It found the POTGO idle value, the TOD tick
  line and the UAE boot ROM area in minutes.
- **Chip-RAM diff per frame**, in plane-sized blocks.
- **Replay the real custom-chip writes** of one frame through your blitter
  and diff the result against the real Chip RAM (`tools/recomp/blit_replay.c`).

## 6. Mechanical translation (days 4-7)

`tools/recomp/recomp.py` turns the loaded machine code into C: one function
per routine, named by address.

- **Decode with the CPU core's own tables** (Musashi's disassembler and
  opcode-to-handler table, via `build/recomp/dasm_helper.dll`). A different
  decoder disagrees on edge cases (Capstone mis-sized some `SBCD` forms).
- **Discovery.** Recursive descent from every traced PC, following static
  calls and branches; every leader becomes a label and a dispatch entry, so
  execution can resume anywhere.
- **Semantics.** Control flow (branches, calls, returns) is native C; each
  data operation initially calls the interpreter's handler for that opcode,
  so the translation has the interpreter's exact behaviour.
- **One register file** shared with the interpreter. A routine can stop at
  any label (chipset event due, unknown target, invalidated code) and the
  interpreter or another routine continues.
- **Write watch.** A write to translated bytes invalidates that routine,
  which makes self-modifying code and misdecoded data safe.
- **Fallback log.** PCs the interpreter executes in game RAM are fed back as
  seeds (`--fallback-log`, `--seeds`).
- **ROM stays on the interpreter** at first.

## 7. Frame parity (week 2 onward)

- `scripts/recomp_parity.py` runs the native build from a recording and
  compares every frame with the emulator's. **The first diverging frame of
  each recording is the project's progress number.**
- Translated and interpreter-only runs must end with byte-identical state
  (`--ram-out` + `cmp`); that separates translation bugs from machine-layer
  bugs.
- Remaining divergence is usually timing: CPU bus contention with DMA, blit
  duration, interrupt latency. Fix the first divergence each time.
- Add a live window early (SDL2, `fa18_recomp --window`). A game you can
  watch and play from its menu is the best morale and debugging tool.

## 8. Readable C source (the actual work)

Replace generated routines one by one with hand-written C that reads like
original source: named functions and parameters, named globals
(`port/game/globals.h`, each with its evidence), named hardware registers
(`port/game/hardware.h`), structs and fixed-point types, and comments on
intent.

- **Temporary glue** (`port/game/glue/`): enters where the original routine
  does, converts registers to C arguments, calls the C function, and
  rebuilds the register and flag state the callers read. Glue is deleted
  when its callers are C.
- **Liveness** (`tools/recomp/liveness.py`): for every call site, which
  registers (data registers as low and high words) and flags the caller
  reads before overwriting them. It is interprocedural, over static and
  observed call edges (`--edges`). A return address outside translated code
  (a ROM interrupt dispatcher, say) keeps everything live.
- **Shadow proof** (`--ports shadow`): on every call, the generated routine
  and the C run on the same state. Live registers, flags, memory outside the
  dead stack, and custom-chip writes must all match. The game continues on
  the generated result.
- **Poison check** (`--poison`): overwrite everything liveness declares dead
  after each call; every frame must still render identically. It catches
  liveness mistakes.
- `sh scripts/recomp_ports_check.sh` runs both over a whole recording.
- **Choosing what to port.** `tools/recomp/port_candidates.py` ranks ready
  routines (callees already C) by glue burden (modified registers that are
  live after return). `tools/recomp/port_info.py <address>` shows a
  routine's code, call sites and required outputs. Use the Ghidra/P-code
  track here: read the routine's report, decompiler view and
  cross-references before writing its C.
- **Order.** Leaves first, then callers, bottom-up. When every caller of a
  routine is C, its glue disappears. Routines whose outputs leak through many
  registers are easier after their callers are ported.
- **Reuse** earlier work where it passes shadow. Here, the packed-BCD and
  rounded-divide code carried over; a line module with a known bug did not.

## 9. Operating system and cold boot

- Inventory the ROM entry points the game calls (the fallback log and call
  edges show them). Replace only those with C: interrupt dispatch, the
  library calls actually used, disk or file loading.
- Replace the savestate start with a cold boot: load the executable from the
  original media, relocate it, and start at its entry point.

## 10. Native build, no emulation

When no generated routine and no ROM code remain:

- Game memory becomes ordinary C globals and structs. The hybrid-phase
  accessors (`rd_u16(ADDRESS)` in `port/game/memory.h`) are replaced
  mechanically.
- Replace the chipset model with a direct native backend: blitter
  operations become drawing functions, Copper lists become palette and split
  changes, Paula becomes an audio mixer.
- Keep the parity runner: the native build must still reproduce every
  recording's frames.

## Rules for agents

- Every session starts with the parity table: first diverging frame per
  recording, and the next blocker.
- A handoff is one page: goal, current numbers, next blocker, commands.
  History lives in git.
- Scaffolding (snapshot starts, the interpreter, generated code, glue) is
  allowed and tracked as backlog.
- Never present an emulator frame as native output. Oracles are for
  comparison only.
- A readable-C change is done only when the shadow proof and poison check
  pass over the recordings and parity is unchanged.
- Commit per verified batch.

## Suggested layout

```
captures/            sealed recordings (read-only)
analysis/            memory map, routine reports, inventories
pcode/               Ghidra P-code export (one static export)
source_amiga/        optional byte-exact assembly
tools/<cpu-core>/    vendored reference CPU core
tools/recomp/        translator, liveness, porting tools
port/machine/        hardware model, savestate loader, input
port/recomp/         runtime, port dispatch/shadow, generated/ (regenerated)
port/game/           the recreated game source
port/game/glue/      temporary register adapters
scripts/             emulator bridge, oracle, parity, lockstep, checks
```

## Time budget

| Step | Target |
| --- | --- |
| 1-2. Emulator, determinism, first recordings | 1-2 days |
| 3. Executable loading, first memory map | 1 day |
| 4. Ghidra import and one P-code export | 1-2 days, then ongoing reading |
| 5. Machine layer, interpreter frames from a snapshot | 2-3 days |
| 6. Translation with fallback, first translated frame | 3-5 days |
| 7. Live window; parity on the first recording | 1-2 weeks |
| 8. Readable C | the bulk of the project, always shippable |
| 9-10. OS replacement, cold boot, native backend | after most routines are C |
