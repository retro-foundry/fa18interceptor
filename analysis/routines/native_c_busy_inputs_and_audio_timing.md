# Plane proof inputs and source-timed audio

Updated 2026-10-01. Authority: original plane/audio instructions and the
sealed demo, carrier-success and qualification-failure recordings.

## Plane shadow input correction

The previous bridge matched all live frames, but five shadow calls compared
native busy counters after held blit writes against source counters after live
blit writes. These are different hardware inputs. The proof now offers an
explicit `FA18Port.shadow_busy_reads` option for stepped bridges.

For this option, the live source runs first. Its DMACONR words are recorded
with each reader PC. C then runs on saved entry RAM, with custom writes held,
consuming precisely that read stream. C independently computes every register,
flag, RAM write and custom write. An extra or reordered read aborts the proof;
unconsumed inputs and additional unsupported native hardware access fail the
comparison. Ordinary ports retain port-first shadow. Existing interrupt,
hardware, frame-end and DMA-write exclusions are unchanged.

The live RAM and CPU result, source write logs and next event are restored
after native comparison. No predicted source outputs are given to C. Reports
add `busy_input_calls` and `busy_input_reads`, so the input model is explicit.
Replay of hardware inputs proves the computation on those observed inputs.
The separately required live ON frame/RAM and boundary checks prove timing.

The plane-only 414-entry gate passed with 721,713 shadow matches, 1,169,611
sandbox matches, zero mismatches, identical poison frames and sealed RAM.
The plane accounted for 1,662 completed shadow comparisons, consuming
1,241,320 source busy inputs; all five former mismatches now compare and pass.
Incomplete source calls remain explicitly incomplete.

```powershell
python tools/recomp/check_shadow_busy_inputs.py
```

This regression check builds isolated incorrect copies under `build/recomp/`.
Changing the first busy counter is rejected with a RAM mismatch (exit 3).
Adding a DMACONR read at the wrong PC is rejected with the unexpected-input
diagnostic (exit 3). Normal game sources and the runner are unchanged by these
tests. Both checks passed before and after sharing bridge helpers.

## Audio timing batch

Two independent 400-frame registry bisections found the original fixed-charge
`set_voice_output` and `fade_master_volume` failing in isolation at frame 256.
The 413-entry all-native baseline failed at frame 255. The voice iterator can
bypass the output leaf's bridge, so the implementation covers the related
parent and helpers together:

| Entry | Operation | Source timing retained |
| --- | --- | --- |
| C24FE8 | master-volume fading | flag-dependent branches, quarter-unit arithmetic, clamping and shared early RTS at C24FE6 |
| C50158 | four-slot voice iterator | MOVEM save/restore, slot stack saves, child calls, period/volume increments and delay expiration |
| C501E0 | period floor / volume cap | high-word extraction, signed comparisons and Paula register writes |
| C50212 | voice program step | delayed field commands, indexed loop counters, relative stream cursor and completion callback |
| C4FFB4 | clear selected voice interrupt | source channel indexing, interrupt-mask read and INTREQ write |

`audio.c` remains the readable domain source. `glue_audio_step.c` implements
the source boundaries while the generated program still shares CPU state.
`glue_step.h` holds common bus accesses, arithmetic flags and branch plumbing
also used by active planes. No opcode handler executes these bridge bodies.
The optional `step_start` represents a source prefix before the registered
entry, needed by fading's existing shared return; it adds no registry entry.

```powershell
python tools/recomp/check_active_planes_step.py --group audio
python tools/recomp/check_active_planes_step.py
```

The audio oracle matches 105 instructions and 6,720 cases on all registers,
full SR, PC, instruction cycles and RAM. The shared plane oracle still matches
119 instructions and 7,616 cases. All CCR combinations, word boundaries and
random high halves are covered; writes are held and DMA waits disabled in
these structural tests. The live checks supply the remaining timing evidence.

First-600-frame source and combined native instruction/event CSVs match byte
for byte:

| Range | Bytes | SHA-256 |
| --- | ---: | --- |
| C50158-C5027C | 20,016,950 | 37c525ec6a03f914c33768316fe35be5fbb9b27ff56be9f6d00ed94236a8a1cc |
| C24FE6-C2502E | 1,002,588 | 4b897d5e21ea4e6a1ef779215e3be1de0df6171e0a7ae1e5cc970d35a703e496 |

Ignored CSVs are `audio_step_{off,on}_600.csv` and
`fade_step_{off,on}_600.csv` under `build/recomp/`. Each run uses the sealed
demo state/input, Kickstart ROM, 600 frames, the corresponding boundary range,
and either `--ports off` or `--ports on --ports-only
C2FD8C,C50158,C501E0,C50212,C4FFB4,C24FE8`.

Profiling also now measures elapsed absolute timeline time around dispatch.
A source shadow continuation can end the frame and discard its remaining
execute budget; the previous counter erroneously counted that discarded
budget as generated work. This fixes the observed 252% generated share while
keeping execution timing unchanged.

## Remaining work

The final combined 414-entry gate passes with 721,715 completed shadow matches,
1,169,611 sandbox matches, zero mismatches, identical poison frames and sealed
RAM. GNU and MSVC Release builds pass. Combined isolated ON for all six timing
entries matches every RGB444 byte and sealed RAM on 20,833 demo frames,
12,353 carrier-success frames and 3,050 failure frames. The RGB SHA-256 values
remain those in `native_c_stepped_plane_bridge.md`. Final reports and live
verification are under `build/recomp/audio_planes_final_*`; comparisons run
after the reference gate finishes writing its streams.

The historical all-414-entry 400-frame ON path differed at frame 255, and that
bisection isolated C330FE (`plot_glyph8`) at frame 260. The subsequent
18-entry source-timed batch fixes that blocker. Current evidence and the next
C31226 blocker are in `native_c_registered_timing_batch.md`. Exact live
equivalence for either isolated batch does not prove every other registered
port. Continue the inactive map/region batch and remaining readable game C
before the native backend and deferred OS services. `CURRENT_PORT_HANDOFF.md`
records the current full gate and live acceptance results.
