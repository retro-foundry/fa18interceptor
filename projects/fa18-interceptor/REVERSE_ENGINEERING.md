# Reverse engineering a retro game into C source

The process this project uses to turn F/A-18 Interceptor (Amiga, 68000,
1988) into readable C, written so it can be reused on the next game on a
68000, Z80 or 6502 machine. It covers the whole path: original media,
emulator, recordings, analysis (Ghidra and P-code), mechanical translation,
the machine model and its timing, frame and state parity, recreating
routines as C with a per-call proof, OS replacement, and a native build.

Two rules shape everything:

1. **Get the whole original program running natively first, then make it
   readable.** The build is never dark: every step keeps a working,
   rendering game.
2. **Everything is measured against the original running in an emulator.**
   A change is done when a comparison says so, not when it looks right.

Section 12 collects the lessons that generalise; the steps refer to them.

## 0. What went wrong the first time

The first ~2,000 commits ported the game bottom-up, one routine slice at a
time, each with its own contract test. After about 400 C modules and 208
passing tests, the native build still drew zero pixels at the first cockpit
frame. The causes:

- **No end-to-end metric.** Tests and "bytes reconstructed" rose while
  "frames that match" stayed at zero.
- **Scaffolding was forbidden.** No placeholders, no emulator-derived state,
  no display until everything was finished, so the first visible result was
  the last thing built.
- **Understanding was a prerequisite for running.** Naming a routine,
  proving it and exporting its P-code came before any code executed.
- **Handoffs grew without limit** (5,000+ lines), so each session spent its
  attention on history.

## 1. Setup

1. **Keep the original media untouched** (the ADF here) and the ROM it
   needs (Kickstart 1.3), with SHA-256 hashes.
2. **Pin a scriptable emulator build.** Here an Engine9000 fork (UAE core)
   driven from Python through ctypes (`scripts/engine9000_bridge.py`). It
   must restore a savestate and replay input frame by frame; dump
   savestates, RAM and registers at any frame; screenshot any frame; and
   single-step with a per-instruction trace that includes the **cycle
   count** (PC, bytes, registers, cycles) and a custom-chip write log.
3. **Check determinism.** Two replays of the same input must give identical
   RAM, video and audio. Nothing downstream works without it.
4. **Note the emulator's configuration** (CPU model, cycle-exact or not,
   chipset, memory). It is the specification the native machine must meet
   (lesson 12.4).
5. **Keep the emulator's source next to the project** when it is open
   source (`tools/engine9000-public/`: UAE's `custom.c`, `blitter.c`,
   `cia.c`, `inputdevice.c`). When native and emulator disagree, the answer
   is usually a few lines in there (lesson 12.5).

## 2. Record and seal scenarios

- Record short human sessions as replayable input streams from a fixed
  savestate (`scripts/record_run.py`). Seal each with its start state,
  input file, config and tool hashes (`scripts/finalize_run.py`); sealed
  runs are read-only.
- Cover the modes early: menu, demo, a normal session, a success, a crash,
  a failure. Each recording becomes a proof and parity scenario.
- Find out how the recording format numbers frames and encodes input
  (here: frames count from the restore, keys are libretro `RETROK_*` codes,
  mouse motion is raw host deltas). Write that down before the first
  replay (lesson 12.6).
- **Once the native machine runs the game, record on it** (here
  `fa18_recomp --window --record`, sealed by `scripts/seal_native_run.py`).
  Its replays are exact by construction, so whole sessions become proof
  scenarios; the emulator's recordings would only replay exactly after
  porting its timing whole (lesson 12.8). Keep the emulator runs archived
  for the machine layer.
- **Key input to the program's own loop, not to video frames**, for a
  CPU-paced game: deliver live input only at the start of a main-loop pass
  and log it as delivered. Input then lands at the same point in the game
  whatever the timing. Do not expect more: a game that integrates elapsed
  time per pass still diverges when machine timing changes (lesson 12.18).

## 3. Load the program and map memory

- **Parse the executable format** (Amiga Hunks here: CODE/DATA/BSS and
  relocations; `scripts/parse_hunk.py`) and record where each segment lands
  (`scripts/resolve_hunk_runtime.py`). Translate the *loaded, relocated*
  image, which is what runs; a savestate RAM dump is the easiest source.
- **Start a memory map** (`analysis/memory_map.md`): code, tables,
  workspaces, stacks, display buffers. It becomes the global list of the C
  source (`port/game/globals.h`).

## 4. Analysis track: Ghidra and P-code (in parallel)

The analysis track exists to *understand* code: to name routines, variables
and fields, and to separate code from data. The port does not wait for it.

- **Ghidra import**, once, headless, at the runtime address
  (`68000:BE:32:default`; `scripts/import_ghidra.ps1`, `scripts/ghidra/`).
  Seed function starts from the entry point, relocation targets and traced
  PCs; run auto-analysis; check instruction lengths against the CPU core's
  decoder on every traced instruction.
- **P-code export**: per instruction its address, bytes, Ghidra's
  assembly, function, flows and raw P-code operations
  (`scripts/ghidra/ExportFa18Pcode.java`). Next time, export once for the
  whole program; here it was done per capture (see below).
- **Never gate porting on per-capture exports.** One export per capture over
  ~120 captures cost weeks and covered 16% of the code.
- **Optional:** byte-exact assembly reconstructions (`source_amiga/observed/`)
  document understanding and prove code/data boundaries. The C port does not
  depend on them.

### How the P-code is actually used

In practice the export has served as an **evidence record of what ran**:
each file lists the instructions one capture executed. No tool interprets
the P-code operations, and nothing on the path to the running C reads the
export at all.

**What is in it.** `pcode/raw/<capture>/` holds one export per capture (126
directories, frozen: Ghidra is not run on this account):

- `instructions.pcode.jsonl`: one row per executed instruction start
  (`address`, `bytes`, `assembly`, `function`, `flow_type`, `static_flows`,
  and the `pcode` operation list with varnodes);
- `observed.asm.txt`: the same instructions as a plain listing;
- `functions.json`: Ghidra's function inventory for those addresses;
- `summary.json`: counts and the language (`68000:BE:32:default`);
- `instructions.segmented.jsonl` and `segment_annotation.json`, added by
  `scripts/annotate_pcode_segments.py`: each row mapped back to its original
  Hunk segment and offset.

**Who reads which fields.**

| Consumer | Reads | Produces |
| --- | --- | --- |
| `scripts/coverage.py` | `address`, `bytes` of every row | the "trace-observed bytes" count in `analysis/coverage.json` and STATUS.md |
| `scripts/verify_reconstructions.py` | coverage from `coverage.py` | coverage reported beside the byte check of `source_amiga/observed/` |
| `scripts/list_unreconstructed_observed.py` | `address`, `bytes` | executed ranges without byte-exact assembly yet (a work queue) |
| `scripts/generate_ghidra_function_coverage.py`, `scripts/list_unrepresented_functions.py` | `functions.json` | function entries with and without assembly (`analysis/ghidra_function_coverage.md`) |
| `scripts/plan_captures.py` | `address`, `bytes` | CODE no export covers, ranked as recording targets (`analysis/capture_targets.md`) |
| Routine reports, by hand | `observed.asm.txt`, the P-code operations | `analysis/routines/*.md` name one export as the "canonical P-code" or authority for the routine's contract; `source_amiga/observed/` slices were written from these listings |

**What does not read it.** None of the following reads the export; they
all work from Musashi's decoder and the loaded RAM image:

- the translator (`tools/recomp/recomp.py`);
- the generated C;
- liveness;
- `port_candidates.py` and `port_info.py`;
- the shadow and poison proofs;
- the recreated C.

The export reaches the port only indirectly. `port_info.py` prints the
routine's report path, and the report's contract and names shape the C
and `globals.h`. The instruction listing the port works from is the one
in the generated C's comments, not Ghidra's.

**What this means for the next project.** Machine-read, the P-code was only
ever an execution trace with a disassembly attached. The emulator's own
instruction trace gives the same evidence without Ghidra. A P-code lifter
earns its cost only if something reads its semantics. For example, a
differential check: evaluate each instruction's P-code on recorded
registers and compare with the CPU core's result, or generate the
translation from it. Neither was done here (lesson 12.16).

## 5. Machine layer

A small native model of the hardware the game touches, and a savestate
loader so native code can start from any recorded frame.

- **Memory map and bus** with the machine's mirroring.
- **Custom chips**, ported from the emulator's algorithms rather than the
  manuals (lesson 12.5): here the blitter (area, fill, descending, line),
  Copper, bitplane display, interrupts, CIAs (timers, TOD, keyboard).
  Details matter: Agnus ignores bit 0 of the blitter modulos, and without
  that every fill row drifted a byte.
- **Savestate loader** (UAE `ASF` chunks: CPU, CHIP, CIAA/B, RAM, boot ROM).
- **A proven CPU interpreter** as reference and fallback (Musashi,
  `tools/musashi/`).
- **One timeline.** Chipset work (line ends, Copper, display, CIAs,
  interrupt acceptance) happens only at instruction boundaries, in one
  service routine that the interpreter and translated code both use.
- **Bus timing** (section 7) comes after the first rendered frames, not
  before; but it must come.
- **Milestone:** from a snapshot, the interpreter on the machine layer
  renders frames that match the emulator.

Tools that paid off:

- **Lockstep compare** against the emulator's instruction trace, reporting
  the first differing register (`scripts/recomp_lockstep.py`).
- **RAM diff per frame**, in plane-sized blocks for display memory.
- **Replaying the real custom-chip writes** of one frame through the native
  blitter and diffing against the real Chip RAM (`tools/recomp/blit_replay.c`).

## 6. Mechanical translation

`tools/recomp/recomp.py` turns the loaded machine code into C: one function
per routine, named by address.

- **Decode with the CPU core's own tables** (Musashi's disassembler and
  opcode-to-handler table via `build/recomp/dasm_helper.dll`). A different
  decoder disagrees on edge cases (Capstone mis-sized some `SBCD` forms).
- **Discovery** by recursive descent from every traced PC; every leader is a
  label and a dispatch entry, so execution can resume anywhere.
- **Semantics:** control flow is native C; each data operation calls the
  interpreter's handler for that opcode, so translation behaves exactly like
  the interpreter, including its timing.
- **One register file** shared with the interpreter. A routine can stop at
  any label (event due, unknown target, invalidated code) and the
  interpreter or another routine continues.
- **Write watch** invalidates translated code that gets written;
  **fallback log** feeds interpreter-run PCs back as seeds; **ROM stays on
  the interpreter** at first.
- **Invariant:** translated and interpreter-only runs end with
  byte-identical RAM and frames. Check it after every change to the machine
  layer, timing included.

## 7. Timing: matching the original's CPU time

A game whose main loop is paced by the CPU (one iteration takes as many
frames as the work needs) only reproduces a recording if native CPU time
matches the emulator's closely. On this project the frames of short runs
matched early, but long recordings drifted until timing was modelled.

**Measure first.** Two tools do the work:

- `scripts/recomp_timing.py <trace>` runs native from the trace's start
  state and pairs instructions with the emulator's per-instruction cycle
  counts. It re-synchronises after interrupts, and reports totals,
  differences by instruction or by address (`--by-pc`), in a time window
  (`--window`).
- `scripts/musashi_timing_audit.py` predicts the CPU core's cost of every
  traced instruction from its cycle table and reports opcodes whose most
  common difference is nonzero. Contention only ever adds time, so the mode
  of the difference is the table error (lesson 12.7).

**Then model, in this order, re-measuring after each:**

1. **CPU cycle table errors.** Musashi's 68000 table was off for long ALU
   operations from registers, ADDQ to address registers, ADDA.W #imm,
   ANDI.L, bit operations on low bits, and data-dependent MULS, DIVS and
   DIVU. Fix the table at init and the data-dependent cases in the core's
   template (`m68k_in.c`, regenerated with `m68kmake`), taking formulas
   from the emulator's source.
2. **DMA contention** (`port/machine/bus.c`). Per scanline, a map of the
   colour-clock slots DMA owns: memory refresh, bitplane fetch in hardware
   order, Copper fetches from each WAIT position. A CPU access to the
   shared bus (Chip RAM, Slow RAM, custom registers) waits for the next
   free slot; ROM does not. Lines not yet drawn this frame use the previous
   frame's map; after a restore, simulate one Copper frame on a scratch copy
   to seed them.
3. **Blits** as per-cycle timelines built from the emulator's cycle
   diagrams. With blitter priority the CPU only gets idle steps; without it
   the CPU steals a cycle after waiting.
4. **Slow peripherals**: CIA accesses synchronise to the E clock.
5. **Access order within an instruction.** The core does all accesses at
   the instruction's start; the real CPU interleaves internal cycles.
   Place accesses in microcycle order: internal cycles first for taken
   branches and indexed or predecrement modes, and a jump's final fetches
   from its *target* (ROM targets are uncontended), after its stack
   accesses.

Where this ended here: per-scene error 0.01-0.5%, and 10/10 exact frames in
the parity window. That is not enough for a 10,000-frame replay: a beam-wait
loop entered 10 colour clocks early takes one extra pass, and the game
state diverges from there. Exact long replays need the emulator's own
cycle-exact CPU and blitter arbitration ported whole. Decide early whether
exact long replays are a requirement; if so, port the emulator's timing
model instead of approximating it (lesson 12.8).

## 8. Parity: frames and state

- `scripts/recomp_parity.py` compares every native frame with the
  emulator's from a snapshot. **The first diverging frame is the progress
  number.**
- `scripts/recomp_outcome.py --run <capture> <frames...>` compares frames of
  a whole recording from its restore (the outcome check: does run060 still
  land, does run062 still fail).
- `scripts/recomp_state_diff.py --run <capture> <frames...>` compares game
  RAM (ignoring the stack areas) and prints the first differing bytes. It is
  the sharpest tool: a static screen hides differences, RAM does not.
  Bisect with it to the first frame where state differs, then find the
  writer with `FA18_WATCH=lo-hi` (lesson 12.9).
- Frame numbering must agree with the emulator's before any of this is
  meaningful: check a per-frame counter in game RAM in both at the same
  frame numbers (lesson 12.6).
- Add a live window early (SDL2, `fa18_recomp --window`).

## 9. Readable C source (the bulk of the work)

Replace generated routines with hand-written C that reads like original
source: named functions and parameters, named globals with their evidence
(`port/game/globals.h`), named hardware registers (`port/game/hardware.h`),
structs and fixed-point types, comments on intent.

### The loop, per batch

1. `python tools/recomp/port_candidates.py -n 40` lists routines whose
   callees are already C, ranked by *glue burden*: registers and flags the
   routine modifies that some caller reads afterwards.
2. `python tools/recomp/port_info.py <address>` shows each routine's code,
   observed call sites and exactly what is live after it returns. Read its
   report in `analysis/routines/` if there is one.
3. Write the C in the module it belongs to (`port/game/<area>.c`), and the
   glue in `port/game/glue/glue_batchN.c`.
4. Register the ports: `python tools/recomp/register_ports.py "comment"
   ADDR=name:cycles ...`.
5. Build (`sh scripts/build_recomp.sh`), then run
   `sh scripts/recomp_ports_check.sh` once for the whole batch. Write ten to
   fifteen routines before checking (lesson 12.13). Fix the mismatches it
   names; they are almost always glue leftovers, not the C.
6. Confirm each new port was actually called (the check lists call counts;
   a port with zero calls is unproven, lesson 12.12). Check parity and
   commit.

### Glue

Glue (`port/game/glue/`) is temporary. It enters where the original routine
does, converts registers or stack arguments to C arguments, calls the C
function, and rebuilds every register and flag the callers read (per
liveness). It is deleted when its callers are C.

- **Liveness** (`tools/recomp/liveness.py`): per call site, which registers
  (data registers as low and high words) and flags are read before being
  overwritten; interprocedural over static and observed call edges. A
  return address outside translated code keeps everything live.
- **Shadow proof** (`--ports shadow`): on every call the generated routine
  and the glue+C run on the same state; live registers, flags, memory
  outside the dead stack and custom-chip writes must match. The game
  continues on the generated result.
- **Poison check** (`--poison`): overwrite everything liveness calls dead
  after each call; frames must stay identical. It catches liveness mistakes.
- **Reusable register helpers.** When a routine calls another already-ported
  one and its callers read the callee's leftovers, factor the callee's glue
  into a `*_registers()` helper and call it from the new glue. Keep helpers
  that re-run a C function only for idempotent functions; otherwise split
  "what the registers will be" from "do it" (lesson 12.11).

### Order

Leaves first, then callers. When every caller of a routine is C, its glue
disappears. Routines that leak many registers are cheaper to port *with*
their callers than alone: when the candidate list shows only heavy
burdens, port the caller cluster together.

## 10. Operating system and cold boot

- Inventory the ROM entry points the game calls (fallback log and call
  edges). Replace only those with C: interrupt dispatch, the library calls
  used, disk loading. Game routines called *by* the ROM (interrupt servers)
  keep every register live until then.
- Replace the savestate start with a cold boot from the original media.

## 11. Native build, no emulation

When no generated routine and no ROM code remain: game memory becomes C
globals and structs (the `rd_u16(ADDRESS)` accessors of `port/game/memory.h`
are replaced mechanically), the chipset model becomes a direct backend
(blits as drawing functions, Copper lists as palette and split changes,
Paula as a mixer), and the parity runner keeps checking every recording.

## 12. Lessons that generalise

1. **One end-to-end number, visible every session.** The first diverging
   frame (or first diverging game state) per recording. Everything else is
   a proxy.
2. **Running beats understanding, at first.** A mechanical translation that
   runs gives a place to hang understanding; understanding without a
   running program gives nothing to test.
3. **Two execution modes that must agree** (interpreter and translated, or
   generated and hand-written) catch whole classes of bugs for free. Keep
   the invariant checked after every change.
4. **The emulator's configuration is the specification.** These recordings
   were made cycle-exact, so the native machine had to model bus timing.
   Read the config before assuming the emulator is "just an Amiga".
5. **Port the emulator's algorithm, not the manual's description.** The
   manuals describe intent; the recordings were produced by the emulator's
   code. Blitter modulos, key codes, mouse delivery and CPU-steal rules were
   each settled in minutes by reading the emulator source, after hours of
   guessing.
6. **Agree on frame numbering and input encoding before comparing
   anything.** Here: the emulator ends a frame when line 3 starts, not at
   line 0; its first frame after a restore runs two frames of machine time;
   keys are libretro codes (SDL 1.2 numbering: F1 = 282), not SDL2; mouse
   motion is released gradually as the game reads the counters. Each looked
   like a timing bug at first. A per-frame counter in game RAM, read in both
   runs at the same frame number, settles numbering immediately.
7. **Separate systematic error from noise statistically.** Per opcode, the
   most common difference against the reference is the table error;
   contention and interrupts only add. Collect many samples (several
   traces) before fixing anything.
8. **Approximate models plateau.** A careful approximation of cycle timing
   got to ~0.1%, which a long recording still amplifies into divergence
   (quantisation: a loop pass, a frame boundary). If exactness matters, port
   the reference model whole rather than tuning an approximation.
9. **Diff state, not pictures.** A static screen can hide dozens of frames
   of divergent state. Game-RAM diffs (ignoring dead stack) find the first
   difference exactly; a write watch then names the code.
10. **Beware re-synchronised comparisons inside loops.** Matching two traces
    by "the next three equal PCs" pairs different iterations of polling
    loops and hides drift. Measure drift at unique landmarks (a routine that
    runs once per frame) instead.
11. **Leftovers are the real contract.** Callers of hand-written assembly
    read whatever registers and flags happen to survive. The 68000 cases that
    cost time here: MOVEM.W to registers sign-extends; MOVEQ, CLR and MOVE
    set flags but leave X, so X comes from the last ADD, SUB or NEG; BTST
    sets only Z, leaving CMP's N, V and C; DIVU/DIVS overflow leaves the
    dividend unchanged; ASR on an unsigned product is still arithmetic; SWAP
    after EXT.L moves the sign word into the low half; compiled C writes
    back into its callers' argument slots. Let liveness list what is needed,
    and let the shadow proof find what was missed.
12. **A proof that never ran proves nothing.** Check that each port was
    called. Labels inside loops, which other routines branch into rather
    than call, never reach the port hook; drop them rather than keep
    unproven ports.
13. **Batch the work, not the checking.** Checking a whole recording set
    takes minutes; writing ten to fifteen routines between checks keeps
    that cost small. Mismatches still point at the exact port and register.
14. **Let the build catch silent errors.** The build compiles generated code
    with warnings off, so a global defined twice silently took the later
    value and broke an already-proven routine. The build now fails on a
    duplicate name in `globals.h`. Add such checks wherever warnings are
    suppressed.
15. **Handoffs stay one page.** Goal, current numbers, next blocker,
    commands. History lives in git.
16. **Know what reads each analysis product.** Here, the P-code export fed
    coverage counts and hand-written reports; the translation, proofs and C
    never read it (section 4). Before producing an analysis artefact, name
    the tool or step that will consume it; an artefact nothing reads is
    documentation, and should be priced as such.
17. **Replay register flow on a snapshot when the C changes what the replay
    reads.** For recursive or stateful routines (the polygon clipper), the
    glue copies the state the original will read, runs the C on the real
    state, then replays the registers from the copy. Anything the replay
    reads that only the C produces (the clipped vertex list) must be read
    after the C runs.
18. **Know what a replay depends on.** Here game state at the same loop
    pass differed after a timing change even with identical input: the
    simulation advances by the frames each pass took. Exact replays need a
    frozen machine and translation; record the hashes of both with each
    recording and re-record after changing them.
19. **The proof must not perturb what it proves.** The first shadow mode
    held interrupts off and replayed custom writes after each compared
    call, which shifted timing, so long replays drifted under the proof
    itself. Run the reference live, exactly as a plain run would (servicing
    events and stepping to the next resumable point inside the call), and
    skip comparing the calls something external interrupted; keep the
    perturbing mode only as an extra pass for those calls.
20. **Count program events once.** A translated routine can stop before its
    first instruction for due chipset work and be dispatched at its entry
    again; counting entries counted some passes twice. Mark resumptions
    (the stop PC and stack pointer) and skip them.

## Rules for agents

- Start every session from the parity numbers: first diverging frame or
  state per recording, and the next blocker.
- Scaffolding (snapshot starts, the interpreter, generated code, glue) is
  allowed and tracked as backlog.
- Never present an emulator frame as native output. Oracles are for
  comparison only.
- A readable-C change is done only when the shadow proof and poison check
  pass over the recordings and parity is unchanged.
- Commit per verified batch.

## Layout

```
captures/            sealed recordings (read-only)
analysis/            memory map, routine reports, inventories
pcode/               Ghidra P-code exports (evidence of what ran; section 4)
source_amiga/        optional byte-exact assembly
tools/<cpu-core>/    vendored reference CPU core (timing-corrected)
tools/<emulator>/    emulator source, the reference for machine behaviour
tools/recomp/        translator, liveness, porting tools
port/machine/        hardware model, bus timing, savestate loader, input
port/recomp/         runtime, port dispatch/shadow, generated/ (regenerated)
port/game/           the recreated game source
port/game/glue/      temporary register adapters
scripts/             emulator bridge, parity, outcome, state diff, timing
```

## Time budget

| Step | Target |
| --- | --- |
| 1-2. Emulator, determinism, recordings, formats | 1-2 days |
| 3. Executable loading, first memory map | 1 day |
| 4. Ghidra import and one P-code export | 1-2 days, then ongoing reading |
| 5. Machine layer, interpreter frames from a snapshot | 2-3 days |
| 6. Translation with fallback, first translated frame | 3-5 days |
| 7. Timing (to the level the recordings demand) | 1-2 weeks; port the emulator's model if exact replays matter |
| 8. Parity on the recordings, live window | alongside 7 |
| 9. Readable C | the bulk of the project, always shippable |
| 10-11. OS replacement, cold boot, native backend | after most routines are C |
