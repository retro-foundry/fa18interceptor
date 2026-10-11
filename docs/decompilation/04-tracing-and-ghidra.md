# 4. Investigate behavior with traces and Ghidra

[Process manual](../../REVERSE_ENGINEERING.md) · [Previous](03-memory-and-hunks.md) · [Next](05-source-reconstruction.md)

Choose a question before collecting instructions. A useful investigation
connects a trigger, producer, state change and consumer; a long listing alone
does not explain a game mechanic.

In the default workflow, the [recompiled baseline](../recomp/porting_game.md)
already supplies executable behavior. Use this chapter to close observed
dispatch/loader gaps and then recover semantics for readable replacements.
Ghidra import and broad naming do not gate the first recompilation.

Choose the evidence path by coupling. Stable local math, clear calls and tables
can start with static analysis and an exact slice. Stateful physics, collision,
projection, dispatch or runtime mutation usually need an early recorded trigger
and producer/state/consumer trace. A widely used primitive can justify a focused
helper contract first; a playable scenario can justify a small connected batch.
Reassess when evidence stops explaining behavior. Do not turn a preferred
technique into a requirement to decode the entire program before observing it.

## Narrow the observation window

Start with normal replay/profiling to locate active code:

```console
python -m amiga_decomp --project your-game run profile_window --restore captures/baseline/state.bin --playback captures/uae/menu-to-game/playback.e9k --first-frame FIRST --last-frame LAST --output build/profile
```

Supply `--frame-offset` when the state represents a later labelled boundary.
The output `profile.json` contains observed PC sample/cycle counts. A sampled
PC suggests where to inspect; sample count is not an invocation count, and
absence from this bounded profile is not global non-execution.

Compare state around the trigger with `sample_replay_memory`,
`sample_runtime_memory_region` or `find_memory_transition`. Select actual
addresses and widths from the memory ledger. Preserve before/after bytes,
frame labels and observed writer/caller context. Two differing final snapshots
can locate storage but cannot identify who wrote it.

## Reach an entry normally, then step a bounded interval

```console
python -m amiga_decomp --project your-game run trace_from_breakpoint --restore captures/baseline/state.bin --playback captures/uae/menu-to-game/playback.e9k --address ENTRY_PC --arm-frame ARM_FRAME --frames SEARCH_FRAMES --return-pc RETURN_PC --max-instructions 10000 --output build/entry-trace
```

Use an observed return PC, not an assumed function end. The command delivers
input during normal replay before the breakpoint, snapshots entry RAM/state
and records PC, raw bytes, assembly, registers and next PC while stepping.
It writes `trace.jsonl`, `trace_summary.json`, `snapshot.json` and final
snapshots. It retains a capped trace and fails if the endpoint is not reached.

Future recorded input after the entry is rejected unless
`--ignore-future-input` is explicitly supplied. That flag changes the
experiment: later input is absent. It is not a way to establish event-faithful
parity. Repeated `retro_run` calls while stepping can alter held-input,
autorepeat and chipset/host scheduling; the report must state this limitation.

`trace_replay_breakpoint` provides a fixed instruction count, optional skipped
hits and settled frames. Those post-breakpoint operations also require a
no-future-input interval; its output shape differs and is not automatically
the same Ghidra import packet. Use the appropriate source/report contract.

Debug writes through `--write-memory ADDRESS VALUE SIZE` are reversible probe
experiments on derived execution, not unmodified original evidence. Preserve
the baseline run, exact patch, parent hashes and changed outcome. Do not turn
a patched result into an ordinary scenario without labelling the variant.

## Trace producers, dispatch and exits

For a table or record field, collect its producer PC, selected index, table
base/bytes, calculated address, stored value and downstream reader. For
indirect calls, record the resolved live target and stack/selector state.
A table entry or static target is not evidence that it executed.

For suspected code reuse or self-modification, watch the actual byte window:

```console
python -m amiga_decomp --project your-game run trace_instruction_memory_writes --restore captures/baseline/state.bin --playback captures/uae/menu-to-game/playback.e9k --breakpoint ENTRY_PC --arm-frame ARM_FRAME --frames SEARCH_FRAMES --watch-address WATCH_BASE --watch-size WATCH_BYTES --max-instructions 10000 --context-instructions 8 --output build/write-trace
```

This compares a watched window across stepped instructions. Preserve the
writer and before/after bytes. Because hardware may advance during stepping,
interpret attribution with instruction operands and DMA context; do not
classify every observed memory difference as a pure CPU store automatically.

When an apparent call does not return to its fall-through address, investigate
tail calls, shared exits, stack manipulation, interrupt entry and computed
returns. Follow actual execution rather than forcing a conventional boundary.

```console
python -m amiga_decomp --project your-game run reverse_trace_call_stacks --trace build/entry-trace/trace.jsonl --anchor ANCHOR_PC --focus-target ENTRY_PC --output build/call-ancestry.json
python -m amiga_decomp --project your-game run generate_trace_call_graph --trace build/entry-trace/trace.jsonl --output build/call-graph
```

Ancestry uses observed JSR/BSR pushes, matching returns and stack-region
filters; a trace beginning mid-call or crossing interrupts has incomplete
ancestry. Dynamic graphs exclude calls outside selected RAM. Optional
`--source-directory` finds address annotations in C; those annotations are
navigation/coverage evidence, not proven C semantics.

## Prepare an observed-byte import

```console
python -m amiga_decomp --project your-game run prepare_ghidra build/entry-trace
python -m amiga_decomp --project your-game run import_ghidra build/entry-trace --name EntryInvestigation --headless GHIDRA_HEADLESS
```

Alternatively set `GHIDRA_HOME` to the installed Ghidra directory. Use a new
project name consisting of letters, digits, underscores or hyphens. Imports
go into the selected project's ignored `runtime_project/`; existing project
names are rejected rather than overwritten. Run headless operations
sequentially and close a GUI owner before writing its database.

The capture packet needs `snapshot.json` describing `memory` entries with
`name`, `start`, `size`, `file`, plus the matching bank files and `trace.jsonl`.
The preparation tool checks bank lengths, non-overlap and 24-bit bounds,
then matches traced instruction bytes to the snapshot. A changed code version
is rejected; retain a separate time-specific packet instead of forcing it.
Only instructions within those captured banks enter `instructions.tsv`.

The launcher reruns preparation and invokes headless Ghidra with `-noanalysis`.
`ImportAmiga.java` maps the banks and disassembles only validated observed
starts, checking decoded lengths. Observed call targets receive structural
functions, not behavioral names or proven complete function bodies.

Ghidra uses the language `68000:BE:32:default`; the exporter records its
68040-superset caution and the observed length cross-check. Treat instruction
semantics, privileged behavior and flags as questions for original-byte and
runtime validation, not automatic correctness from an IR export. Preparation
does not itself prove all ownership or validate every stored authority hash.

| Export | Meaning |
| --- | --- |
| `instructions.tsv`, `inventory.json` | Validated observed starts, hit counts and structural inventory. |
| `pcode/instructions.pcode.jsonl` | Address, bytes, bank offset, assembly, flow and raw P-code operations. |
| `pcode/observed.asm.txt` | Structural observed disassembly. |
| `pcode/functions.json` | Structural entry/body inventories. |
| `pcode/summary.json` | Counts, language identity and interpretation caution. |

`file_offset` in exported instruction rows is offset within the memory block,
not the original executable's disk offset. ROM instructions outside mapped
banks are excluded. RAM may include OS code; the import does not automatically
separate it from game ownership.

## Give every export a consumer

Use exports to compare observed byte coverage, attach verified Hunk identities,
locate callers, examine bounded data-flow or choose source reconstruction.
`annotate_pcode_segments` and `summarize_trace_edges` consume explicit inventory
and resolved-map files. Keep bank/version and relocation provenance attached.

Do not produce large exports merely because they are possible. Record the
question, report destination and next consumer before collecting them. Raw
P-code is not reconstructed C and is not executed by the framework as a game.

Acceptance: the trace answers a bounded question, its instruction bytes match
its preserved time window, and structural analysis remains distinct from
accepted routine/state meaning and whole-scenario behavior.
