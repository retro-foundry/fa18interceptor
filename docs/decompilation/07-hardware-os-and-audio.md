# 7. Establish hardware, OS and audio contracts

[Process manual](../../REVERSE_ENGINEERING.md) · [Previous](06-validation-and-divergence.md) · [Next](08-evidence-and-maintenance.md)

Program behavior includes its interaction with the machine. Reconstruct the
observed interface before deciding which parts a port can replace with native
services. Use [AMIGA.md](../../AMIGA.md) for the platform reference and
[AMIGA_FINDINGS.md](../../AMIGA_FINDINGS.md) for findings from the recorded A500
profile. Those findings need qualification on a different chipset or core.

## Trace consumers of hardware state

Map each custom-register/CIA access to its writer, reader, timing and purpose.
For display work, record bitplane pointers, dimensions, fetch/window settings,
palette, Copper changes, sprite state and buffer ownership. For blits, record
enabled channels, pointers, modulos, masks, shifts, minterm, size and completion.
Final RAM alone cannot reveal when a blit became visible or whether the CPU
observed busy/interrupt state before its next operation.

DMA and CPU accesses share resources on the relevant Amiga profile. A model
that computes correct bytes instantly can still change polling, interrupt
acceptance, memory contention or presentation. Separate pixel/byte correctness
from scheduling correctness. Keep clock units and observer limits in the report.

```console
python -m amiga_decomp --project your-game run capture_dma_frame --restore START_STATE --playback INPUT_LOG --frame FRAME --output build/dma-frame.json
python -m amiga_decomp --project your-game run inventory_blitter_jobs --help
python -m amiga_decomp --project your-game run inventory_dma_blits --help
python -m amiga_decomp --project your-game run inventory_dma_families build/dma-frame.json build/dma-families.json
python -m amiga_decomp --project your-game run inventory_dma_writes --help
```

Use `--playback-frame-offset` when the input log's frame origin differs from the
restore boundary. A narrow capture uses manifest `dma_capture_ranges` or explicit
`--address-start`/`--address-end`; report the excluded addresses. Broad capture
can reveal a missed consumer but is not unlimited evidence. Inspect dropped-log
errors and retain interval boundaries before treating rows as a complete stream.
`inventory_dma_families` takes positional capture/output JSON paths; its grouped
register patterns are investigation candidates, not inferred game semantics.

Do not read live hardware registers merely to obtain a convenient oracle value
without checking their side effects. Prefer retained state or an observer whose
read-only behavior is established. Ordinary RAM reads, debug stepping and normal
frame capture have different effects on execution; preserve that distinction.

## Discover OS contracts from real calls

An OS/library service's name or vector offset suggests an investigation target;
it does not provide the game's actual calling context. Record the resolved
entry, caller, registers, stack return, task, buffers and library base. Observe
whether it allocates, queues work, installs interrupts, blocks or calls back.
The same entry can be invoked by other tasks during startup.

Capture cold entry when restored state would bypass the initialization being
investigated:

```console
python -m amiga_decomp --project your-game run tools/amiga/capture_cold_entry.py --config local/game.uae --entry ENTRY_PC --frames FRAME_BOUND --output build/cold-entry
```

This tool never restores a state. It runs from power-on to the selected
breakpoint and writes `entry.json`, `state.bin` and configured RAM banks. The
report includes input/config/core hashes, host/core frame coordinates, cycles
and registers. Optional `--playback` and `--input-offset` apply desktop input at
an explicitly declared cold-start offset. Reaching a PC establishes that entry,
not the correctness of every loader/startup operation preceding it.

For a bounded entry/return contract:

```console
python -m amiga_decomp --project your-game run tools/amiga/capture_service_contract.py --config local/game.uae --state START_STATE --entry ENTRY_PC --return-pc CALLER_RETURN_PC --frames FRAME_BOUND --instructions INSTRUCTION_BOUND --output build/service-contract
```

`--return-pc` filters other callers at the same entry. A normal endpoint requires
the stack's return PC and the corresponding popped stack pointer. `--stop-pc`
instead selects an execution checkpoint; report that as a checkpoint contract,
not a completed service return. Both tools restrict binary output to the
selected project's `build/` or `local/` directory.

The service tool installs its breakpoint before unserialization because the
core can advance machine time during restore. Inspect that boundary rather than
assuming the first searched host frame owns every captured action. Artifacts
include `contract.json`, entry/endpoint RAM and state, `instructions.jsonl` and
`hardware_window.jsonl`. Instruction rows contain before/after registers,
cycles and a ten-byte instruction window; this is a different schema from the
trace packet accepted by `prepare_ghidra`. It needs an explicit conversion and
instruction-length validation before import.

Hardware logging flushes at a subsequent ordinary frame boundary. The retained
window therefore extends beyond the service interval. Its manifest explicitly
sets `hardware_window_requires_interval_filter`; filter by the measured entry
and endpoint clocks before claiming ordered effects of that call.

Use the resulting contract to test a replacement against actual calls and
edge conditions. Captured RAM/state is oracle evidence, not replacement runtime
initialization. A startup image filled with observed bytes does not establish
loader, allocator, OS or ROM independence.

## Connect loader results to disk bytes

For DOS-loaded programs, inspect the segment chain and verify allocations,
payload placement and relocations against the executable/Hunk inventory:

```console
python -m amiga_decomp --project your-game run tools/amiga/inspect_segment_list.py --state START_STATE --executable local/extracted/game --inventory analysis/hunks.json --segment-list SEGMENT_LIST_BPTR --output build/segment-layout.json
```

`--segment-list` is a DOS BPTR: the tool multiplies it by four to locate the
first chain node. Do not pass the already-expanded byte address. It validates
chain extent, allocation/payload bounds and 32-bit relocations, and reports
nonrelocated changes. The current savestate reader uses uncompressed `CRAM`
and `BRAM` at the A500 Chip/Slow bases; it is not a general expanded-memory
loader oracle. See [chapter 3](03-memory-and-hunks.md) for other mapping paths.

## Establish the original audio recording first

Sound has several separate ownership layers:

| Layer | Evidence to retain |
| --- | --- |
| Game request | Trigger, selected sample/voice, queue, repeat and chain rules. |
| Paula setup | Ordered pointer, length, period, volume, DMA and interrupt writes. |
| Physical consumption | Actual words/bytes fetched, address wrapping and reload boundaries. |
| Mixer/output clock | Original accumulator, interval, count and output boundaries. |
| Filtering | Active configuration, LED/filter state, coefficients and persistent history. |
| Recorded/presented output | Complete PCM, rate/layout, callback blocks, onset and device conversion. |

Game voice-table readers and acceptance checkpoints belong in the project.
Shared stream parsers, Paula/CIA state readers and observer sources belong in
the framework. Their historical `fa18`/`FA18` ABI identifiers remain stable so
existing captures stay readable; those names do not make game semantics generic.

Begin with ordinary core execution, matching audio and nonaudio baselines:

```console
python -m amiga_decomp --project your-game run engine9000_bridge --restore START_STATE --playback INPUT_LOG --frames FRAME_BOUND --wav --output build/original-audio
python -m amiga_decomp --project your-game run tools/amiga/audio/check_original_audio_capture.py --capture build/original-audio --baseline build/original-baseline --out build/original-audio-check.json
```

The bridge stores `original.wav`, `audio_chunks.jsonl` and a `recorded_audio`
descriptor in `snapshot.json`. Every callback block has its call coordinate,
first sample, sample-frame count and PCM hash. Initialization/restore output is
excluded from the declared ordinary replay scope; preserve that exclusion when
comparing onset. `--wav` cannot be combined with stepped `--trace-frames`.

The validator checks complete block coverage, full PCM/WAV hashes and unchanged
baseline execution. Its current bank/image checks use the retained Chip/Slow
capture layout; audit compatibility before applying it to another RAM profile.
A prior audio baseline also requires identical complete PCM and block records.
This validates the original recording, not the replacement's waveform.

## Qualify observers and filtering separately

`--audio-events` needs `--wav`, a restore state and the manifest's game event
observer. `--audio-dma` builds on events; `--audio-samples` additionally needs a
compatible observer installation selected by `AMIGA_DECOMP_ENGINE_ROOT`.
The bridge checks its build descriptor/exports. Narrow mixer ranges use
`--audio-mixer-call-range FIRST LAST` with ordinary call coordinates, not an
arbitrary host timestamp. Consult `engine9000_bridge --help` for current gates.

Create an isolated observer build only when the existing core cannot provide
the required evidence:

```console
python -m amiga_decomp --project your-game run tools/amiga/audio/build_original_audio_probe.py --out build/audio-observer --mixer-timeline --mixer-clock
```

This requires the selected Engine9000 source/build objects and compatible
MinGW tools. It does not replace the authority installation. Qualify observed
and unobserved execution before trusting the new stream; bind every report to
the actual DLL hash and source/build identity. `--word-state` and
`--restore-state` provide additional specific observations, not inferred data.

The shared clock checker consumes a capture, uninstrumented mixer baseline and
timeline report. The prefix checker tests a bounded recording against an
already validated complete PCM source. Their options are listed in the
[tool catalog](../../tools/README.md); they do not search for a favorable shift.
The prefix checker currently assumes the retained 44100 Hz stereo 16-bit profile.
Chip-size defaults of 512 KiB must be overridden for different physical layouts.

Retained CIA/audio state can establish endpoint LED/filter flags, but not every
intermediate switch time. Recover missing transitions from qualified writes or
the original mixer. Preserve filter history across sample changes and model
startup/reset ownership from evidence. A low-pass filter chosen because it
sounds plausible is not the original filter contract.

Finally compare complete original/replacement recordings under the policy in
[chapter 6](06-validation-and-divergence.md): onset, silent intervals, repeated
samples, chain handoffs, filter switching and all sample tails. Report event,
physical-byte, clock and waveform results separately. Negative controls should
reject removed blocks, shifted handoffs and altered filter state.

Acceptance: contracts retain causal ordering and clock scope; hardware/service
evidence is not used as hidden initialization; original audio is complete and
any replacement sound claim names its actual comparison layer.
