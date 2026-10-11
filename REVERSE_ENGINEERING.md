# Reverse engineering an Amiga game

This workflow applies to a selected game project. Original media and observed
execution are the authority. The framework supplies capture, replay, Hunk and
trace analysis, hardware observers and Ghidra integration. A game's reconstructed
source and playable port belong to its own project or repository.

The method grew out of F/A-18 Interceptor. Its historical results and estimates
are [project evidence](projects/fa18-interceptor/analysis/reconstruction_history.md),
and do not describe every game's hardware configuration or prove a new port.

## Establish the reference

Preserve the original disk and ROM bytes and record their hashes. Pin the
emulator commit, core binary, configuration and observer ABI. Record CPU model,
RAM banks, video standard, cycle-exact settings, floppy speed and audio options.
The emulator configuration is evidence; do not change it to conceal a mismatch.

Create `projects/<id>/project.json` using the fields in [README.md](README.md).
Select the project explicitly:

```console
python -m amiga_decomp --project <id> info
python -m amiga_decomp --project <id> run engine9000_bridge --frames 5 --output build/startup
```

The memory map comes from the selected project. Shared tools must not assume
one game's loaded addresses, RAM sizes, data tables or input controls. Shared
Engine9000 source is at `tools/engine9000-src`; `engine_root`, `engine_source`
and `replay_engine` select the installation, source and replay executable.
Game-specific forks can instead be selected through those same manifest paths.

First prove boot, mounting and program loading. A successful LoadSeg establishes
loading, not entry to gameplay. For packed programs, preserve the container and
identify the byte image actually executed before interpreting addresses.

## Record reproducible scenarios

Choose bounded scenarios covering startup, menus, input, gameplay, transitions,
rendering, sound and exit. Randomness and interrupts are part of the original;
record inputs and restore boundaries rather than assuming a new run is identical.
Keep sealed captures immutable, including config, inputs, state and toolchain.

```console
python -m amiga_decomp --project <id> run record_run --initial-state <state.bin> --restore-frame <frame> --name <scenario>
python -m amiga_decomp --project <id> run finalize_run --help
python -m amiga_decomp --project <id> run playback_run --run captures/uae/<scenario>
python -m amiga_decomp --project <id> run render_run --help
```

The Windows GUI recorder requires a manifest `toolchain` path and the patched
Engine9000 boot-restore protocol. State and restore frame are explicit inputs;
there is no default game scenario. Disable host hotkeys that consume game input.
Keep host-frame labels, core calls and game ticks distinct. Restoring a state can
resume within a frame, so record the exact boundary and replay offset.

Use the replay host for sealed visual comparisons. Direct-core screenshots are
diagnostic state probes; they bypass host scheduling and do not establish replay
image parity. Relocation creates derived configs and copied save-slot headers;
it must not rewrite the sealed config or serialized core payload.

## Map loaded code and data

Parse the Hunk container, preserving CODE, DATA, BSS, memory flags, relocation
groups and symbols. Keep disk offsets, Hunk-relative offsets and runtime
addresses as separate values. Verify unchanged payload bytes and relocation
addends against captured RAM before accepting a loaded base.

```console
python -m amiga_decomp --project <id> run parse_hunk --help
python -m amiga_decomp --project <id> run map_loaded_segment --help
python -m amiga_decomp --project <id> run map_runtime_window --help
python -m amiga_decomp --project <id> run disasm_game
```

`disasm_game` uses manifest paths `executable` and `hunk_runtime_map`, plus
`disassembly_synthetic_base` for unresolved listing addresses. Synthetic bases
are explicitly labelled; linear sweeps are navigation aids and can decode data
as instructions. Neither implies a verified runtime address or semantic meaning.

## Trace and analyse observed execution

Reach a breakpoint with normal replay, then capture a bounded instruction
interval. Preserve instruction bytes, PC, registers, stack and relevant memory.
Instruction stepping changes scheduling; keep that qualification with the trace
and do not silently discard later required input events.

```console
python -m amiga_decomp --project <id> run trace_from_breakpoint --help
python -m amiga_decomp --project <id> run sample_runtime_memory_region --help
python -m amiga_decomp --project <id> run prepare_ghidra build/<trace>
python -m amiga_decomp --project <id> run import_ghidra build/<trace> --headless <analyzeHeadless>
```

Validate observed instruction starts and bytes before Ghidra import. Avoid
auto-analysis over unobserved data. P-code, structural functions and call graphs
help navigation; they do not prove gameplay semantics. Name a concrete consumer
before generating large exports. Use `annotate_pcode_segments` to attach verified
Hunk identities, `summarize_trace_edges` for mapped calls, and
`reverse_trace_call_stacks` for ancestry supported by stack pushes and returns.
Source address annotations in `generate_trace_call_graph` are coverage clues,
not proof that a C function implements those addresses correctly.

## Compare before naming and rewriting

Start with byte-exact reconstruction where possible. Convert bounded regions
supported by original bytes, callers, storage and hardware effects. Preserve
signedness, wrapping, alignment, calling context and update order. Keep uncertain
names as candidates until evidence supports a stronger interpretation.

Maintain a running end-to-end path alongside local routine checks. Test counts,
translated bytes and function counts measure their own scope; they are not a
completion percentage. Find the first divergence rather than adjusting a later
symptom. Compare RAM and registers as well as pixels; an image can hide corrupted
state. Caller-visible register leftovers, flags, interrupted routines and resumed
entries can matter even when a function's apparent result matches.

When a project uses mechanical translation or a temporary machine model, keep
that dependency explicit and replace it progressively. The framework does not
ship a translator, CPU interpreter or replacement chipset runtime. A shadow
comparison must not perturb the primary execution or count resumed calls twice.

## Check hardware effects and timing

Preserve CPU access order, interrupts, DMA contention, CIA clocks, blitter waits
and register masking when they affect the program. Approximate opcode totals
can conceal bus-access ordering errors. Use the original emulator algorithms
as references and record the source revision rather than inventing constants.
See [AMIGA.md](AMIGA.md) and [AMIGA_FINDINGS.md](AMIGA_FINDINGS.md).

`capture_dma_frame`, `inventory_blitter_jobs` and the DMA inventory tools report
actual chipset activity. Optional project `dma_capture_ranges` controls narrow
capture windows; only the blitter register range is common hardware policy.
Audio observers under `tools/amiga/audio` collect DMA words, consumed samples,
mixer intervals, clocks and retained CIA/Paula state. Game voice ownership is a
separate project observer. Observer telemetry requires paired whole-execution
and PCM preservation before it can be treated as reference evidence.

A correct fetch stream or filter primitive does not prove complete sound parity.
Compare complete recordings with an explicit onset and scheduling policy; keep
sample handoffs, filtering, resampling and presentation differences distinct.

## Keep evidence and caches manageable

Store addresses, game tables, fixtures, original media, scenarios and reports
inside the selected project. Shared algorithms and hardware guidance live at
framework level. Use temporary capture workspaces, compress reusable RAM and
retain failure evidence with hashes. Run `prune_build_artifacts` after captures;
the project artifact policy bounds its own build roots.

Keep a short handoff stating the active task, authority, last validation,
remaining uncertainty and next concrete action. Archive detailed proofs as
project reports. Commit validated batches and push submodule changes before
updating their parent pointers.
