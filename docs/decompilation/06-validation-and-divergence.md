# 6. Validate results and find the first divergence

[Process manual](../../REVERSE_ENGINEERING.md) · [Previous](05-source-reconstruction.md) · [Next](07-hardware-os-and-audio.md)

Decide the comparison scope before changing behavior. Validation answers a
specific question under specified inputs, machine state and timing; it does
not certify every path merely because its count is large.

## Use cumulative evidence

| Level | What a passing result establishes |
| --- | --- |
| Address/pointer preflight | The accessed bytes, offsets, widths, stride and bounds are correct. |
| Exact source assembly | The specified source view reproduces its authority bytes. |
| Primitive/digest checks | Tested decode, math, draw or table outputs match for declared inputs. |
| Routine/state contracts | The sampled entry/caller/exit paths satisfy their stated effects. |
| Live trace comparison | Registers, memory and ordered events match at observed runtime boundaries. |
| Scenario output | Declared images, sound, outcomes and saved state match across the selected scenario. |
| Runtime independence | The exercised implementation no longer uses the named CPU/ROM/chipset/oracle dependency. |

Later results do not erase failed earlier evidence. A screenshot can conceal
state corruption; final RAM can conceal wrong intermediate frames; a callable
routine can remain disconnected from gameplay. State the exercised target.

For gameplay qualification, include independent startup where initialization
matters, repeated individual scenarios where random outcomes matter, and
transition checks where state reset matters. Whether an uninterrupted campaign
is required belongs to the game's agreed acceptance policy; the framework
does not impose a universal mission sequence.

## Write the timing policy

Declare the comparison boundary and permitted differences before looking for
a passing alignment. Record:

1. Host frame, core call, game update or semantic landmark used for alignment.
2. Input delivery order and restore/frame offset.
3. Clock unit, sample rate, output cadence and presentation boundary.
4. Exact state/output fields versus explicitly tolerated presentation effects.
5. Any permitted bounded variation and the original evidence justifying it.

An exclusion needs a demonstrated cause. For example, a project can compare
palette indices separately from a source-proven fade table; that table and
policy are game evidence, not a generic excuse to hide differing pixels.
Do not shift entire recordings, alter physics or resample audio until a mismatch
disappears. Preserve the raw comparison alongside the declared policy result.

If trajectories agree but a HUD selector changes on a different tick, trace
the selector writer, its clock/input source and draw consumption. Determine
whether the difference is a permitted host/timing effect or a state-ordering
fault. Matching position alone does not answer that question.

## Diagnose in dependency order

First confirm identical executable/ROM/config/core identities and input/state
boundaries. Next check loaded placements and code versions. Then compare the
first differing register/store/branch, followed by hardware/input event order,
and finally rendering or PCM presentation. A config or frame-label error can
make every later comparison misleading.

Use binary/landmark narrowing: bound the last known-good and first bad interval,
capture a smaller region and inspect the causal predecessor. Keep the smallest
failing case and a baseline produced by the same policy.

```console
python -m amiga_decomp --project your-game run diff_slow_ram build/before/slow.bin build/after/slow.bin --region slow
python -m amiga_decomp --project your-game run check_replay_breakpoint --help
python -m amiga_decomp --project your-game run sample_runtime_memory_region --help
```

The RAM diff reports exact changed ranges using the selected region base/size.
For a different region, change both input files and `--region`. It locates
changes, not their meaning. Use stepped writes or normal custom logs to recover
causal ordering, with each tool's timing qualification preserved.

## Compare images at the correct layer

Use GUI-host replay for sealed visual evidence. Preserve frame counts, order,
selected camera/viewport, dimensions and palette policy. Full-frame comparison
and bounded viewport comparison establish different scopes. Check geometry,
indices and RGB values separately where the format permits it.

For disappearing geometry, distinguish placement/culling, clipping, raster
construction, buffer ownership, draw order and presentation. Compare source
input packets and per-stage outputs at the first missing line; a visual guess
about distant detail is not evidence of an original culling rule.

Before accepting a draw-order change, trace the actual selector/initialization
path under startup, menu entry, normal gameplay and restart. An unvisited path
is uncertainty, not a confirmed visible fault.

## Keep sound proof distinct

Event/request parity, selected samples, consumed bytes, DMA words, mixer
intervals, filter behavior and complete waveform equality are separate levels.
Working playback proves none of the later levels by itself. Declare onset and
handoff boundaries, preserve complete output and retain negative controls that
reject dropped samples, altered boundaries and wrong filter state.
See [chapter 7](07-hardware-os-and-audio.md).

## Measure visible work and presentation

A hidden or headless run measures its actual work only. For visible qualification
include audio, combat/activity, different cameras and representative modes,
and ensure every rendered frame reaches the intended presentation path.

```console
python -m amiga_decomp --project your-game run report_frame_times build/frame-times.csv --budget-ms 20 --require-budget --require-presentation --output build/frame-times.json
```

The CSV is produced by a runtime with the documented phase schema, not generated
by this command. Phases include input/game/audio/conversion/presentation/wait,
work/total duration and start interval, plus frame and presentation markers.
The command checks sequential rows, summarizes percentiles/maxima and can reject
work exceeding the selected budget or missing recorded SDL presentation.

Choose the budget from the game's target, not the example's 20 ms value.
Intentional pacing wait is separate from work. SDL submission/presentation
markers do not prove compositor visibility; state window visibility and the
measurement limit. Warm up initialization/caches before attributing allocations
or latency to steady gameplay, while preserving startup qualification separately.

For a no-allocation gameplay requirement, initialize buffers, pools, audio
conversion and device caches before the first update. Measure heap requests and
pool growth through the actual input/update/draw/audio/present path, including
callbacks and camera/mode transitions. A zero count in an offline helper does
not cover driver or runtime work outside that instrumented boundary. Record
fixed capacities and exhaustion behavior, and retain the tested worst cases.

Keep update/event ordering stable where the original establishes it. Record
random-source state and original spawn decisions when comparing a replay;
repeat independent runs to investigate naturally varying encounters. Making
the original deterministic by inventing a seed or changing spawn rules would
change the behavior being qualified.

## Report an achieved result precisely

Name the code/build, scenario, input/state/config hashes, compared outputs,
counts, first failure if any and remaining dependencies. Explain when a policy
waives a larger run and what smaller checks cover its intended risks.
Keep estimates scoped; function counts, commits and elapsed effort cannot
support an invented completion percentage.

Run checks appropriate to the change. Once they pass, repeat/broaden only for
new changes, failures or unresolved questions. Commit the validated result and
retain its report, rather than rebuilding every unrelated historical harness.

Acceptance: each claim has a stated boundary/policy and an appropriate oracle;
divergences are either resolved causally or retained as explicit open evidence.
