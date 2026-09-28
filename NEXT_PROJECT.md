# How to decompile/port the next game

Lessons from F/A-18 Interceptor, written for the next retro-game port
(Amiga or any other 68000/Z80/6502-era target). Read this before writing
any code. The short version: **get the whole original program running as C
first, then make it readable. Never let the build go dark.**

## What went wrong here

The first ~2,000 commits ported the game bottom-up: one bounded routine
"slice" at a time, each with its own contract test. After about 400 C
modules and 208 passing tests, the native build still drew zero pixels at the
first cockpit frame. Each slice was correct in isolation. Nothing forced them
to be wired together, so every new slice revealed another missing caller or
owner, and the renderer's call tree was never complete enough to run.

Contributing causes:

- **No end-to-end metric.** "Tests pass" and "bytes reconstructed" went up,
  while "frames that match" stayed at zero.
- **Rules that forbade scaffolding.** No placeholders, no emulator-derived
  state, no display until the native path was finished. Rigor aimed at the
  wrong stage makes the first visible result the *last* thing you build.
- **Handoff documents that grew without limit** (5,000+ lines). Every agent
  session spent its attention re-reading history instead of working.
- **Reverse engineering and porting were coupled.** Naming and understanding
  a routine became a prerequisite for running it. They are independent.

## The method

### 1. Pin the authority and the oracle (day 1)

- Keep the original media untouched. Put a pinned emulator build in the repo
  tooling, and make it scriptable: savestates, per-frame screenshots,
  instruction traces, and memory/register dumps at any frame.
- Record a few real play sessions as replayable input streams. Seal each one
  with its start state and hashes.
- Check that replay is deterministic: two replays give identical
  RAM/video hashes.

### 2. Build the machine layer (days 1-3)

- Write a small native hardware model for the target: memory map, the custom
  chips the game actually touches (on the Amiga: blitter, Copper, bitplanes,
  interrupts, CIAs), and a framebuffer. Accuracy per scanline is enough to
  start with.
- Write a loader for the emulator's savestate format (UAE's `ASF` chunks
  here), so native code can start from **any** recorded frame. Starting deep
  inside gameplay, not from a cold boot, is what makes early results possible.
- Embed a proven CPU interpreter (Musashi for the 68000) as the reference and
  as the fallback. Run it from a snapshot on your machine layer and get
  pixels. This validates the hardware model before any translation exists.

### 3. Mechanical whole-program translation (days 3-7)

- Write a generator that turns machine code into C mechanically. Emit one C
  function per routine; control flow (branches, calls, returns, jump
  targets) becomes native C with a label at every leader. Data operations can
  start as calls into the interpreter's per-opcode handlers. That gives
  identical semantics for free, and you replace them with inline C later.
- **Decode with the same tables the CPU core uses** (Musashi's disassembler
  and opcode-to-handler table here). A different decoder will disagree on
  edge cases; Capstone mis-sized some 68000 `SBCD` forms.
- Seed discovery with every PC from real traces, then follow static calls and
  branches recursively. Register every label as a dispatch entry, so
  execution can resume anywhere and indirect jumps just work.
- Keep all CPU state in one shared register file. Then a translated routine
  can stop at any label (slice budget, interrupt, unknown target), and the
  interpreter or another routine picks up with nothing lost.
- **Watch writes to translated bytes.** A write invalidates that routine and
  falls back to the interpreter. This makes self-modifying code and
  misdecoded data safe.
- Log every PC the interpreter runs in game RAM. Feed that log back to the
  generator as seeds. Coverage grows with every run, and the log doubles as
  code/data evidence for the RE work.
- Leave OS/ROM code on the interpreter at first. Count which OS entry points
  the game uses, then replace only those with C (high-level emulation).

### 4. Frame parity is the only progress metric

- Run every sealed recording natively and compare each frame with the
  emulator oracle. On a mismatch, diff per-frame RAM and hardware-register
  write logs, and fix the **first** divergence.
- Track one number per recording: the first diverging frame. A change that
  doesn't move it (or keep it) waits.
- Keep an interpreter-only mode (`--no-recomp`) as a built-in differential
  baseline. Translated and interpreted runs must agree bit for bit.

### 5. Make it readable, one routine at a time

- Only after parity: replace generated routines with hand-written C, using
  real names, structs and fixed-point types.
- Every replacement is differentially tested. Run the generated routine and
  the hand-written one on the same captured machine state; memory,
  registers and hardware writes must match. The whole run must keep its
  first-divergence frame.
- Naming and documentation (the RE deliverable) happen here. Evidence rules
  (a name needs behavioral proof) are good; they just must not block
  execution.

### 6. Widen

- Add recordings for each game mode (menus, missions, crashes, saves).
  The remaining generated routines and fallback-log entries are the
  measured backlog.

## Where Ghidra and P-code fit

Not on the critical path. The translation needs only the loaded machine code
(decoded with the CPU core's own tables) and emulator traces to show which
code runs. This project exported Ghidra P-code per capture before any C
existed; that duplicated what a decoder provides and was slow. The analysis
built from it (routine reports, a memory map, named globals) did pay off
later, as reading material for the readable-C stage.

Next time:
- Skip per-capture P-code exports.
- Load the program into Ghidra once, statically, and use its decompiler view,
  cross-references and data typing as a reading aid while writing each
  routine's C (stage 5).
- Keep names and struct layouts in the C source and the memory map, not in a
  separate RE database that has to be kept in sync.

## Rules for agents working on it

- The first question in every session: "what is the first diverging frame
  of each run, and what is the next blocker?" If an agent can't answer it,
  it shouldn't start writing slices.
- A handoff is one page: goal, per-run first-divergence frame, the single
  next blocker, and the commands to reproduce. History lives in git.
- Scaffolding (snapshot-seeded state, interpreter fallback, generated code)
  is explicitly allowed and tracked as backlog, not forbidden.
- Never show an emulator frame as native output. Oracles are for
  comparison only. This rule was right; keep it.
- Prefer a tool that produces a result today over a proof that it would be
  correct next month.

## Suggested layout

```
captures/        sealed recordings (read-only)
tools/<cpu-core>/ vendored reference CPU core
tools/recomp/    generator + decoder helper
port/machine/    hardware model, savestate loader
port/recomp/     runtime contract + generated/ (regenerated, not hand-edited)
port/game/       hand-written replacements (stage 5)
scripts/         oracle rendering, parity/diff runners
```

## Time budget that should have been

| Step | Target |
| --- | --- |
| Oracle, replay, snapshots | 1 day |
| Machine layer + interpreter pixels from a snapshot | 2-3 days |
| Generator with interpreter fallback, first translated frame | 3-5 days |
| Frame parity on the first recording | 1-2 weeks |
| Readable C and naming | open-ended, incremental, always shippable |
