# Original output clock and startup channel timing

The independent original startup recording preserves all 5,000 ordinary calls,
4,410,000 stereo PCM frames and complete game execution. Its actual initial
output countdown and source clock rules predict every one of the **1,059,282
output boundaries** observed across calls 3,800–5,000, including the exact
countdown after each call. A restored 94-call recording also preserves its
entire execution and predicts all **29,988 boundaries** in calls 60–93.
Native gameplay and its audio clock are unchanged.

The reference-only export reads the original audio service cycle, last audio
cycle, output countdown and display/audio clock inputs into a fixed 56-byte
record. The caller remains ordinary `retro_run` through original `update_audio`.
It does not update audio, deliver events, allocate storage or alter save-state
serialization. The existing mixer observer remains byte-identical before the
appended export. All 227 dependency object hashes remain identical. The strict
validator reconstructs the exact known generated source, including the new
export; an unknown appended export remains unacceptable.

`custom.c:compute_vsynctime` and `retrodep/sounddep/sound.c:update_sound` derive
this recording's output cadence from the observed display configuration:
312 nominal lines, long field enabled, 227 clocks per line, 50 Hz refresh,
44100 Hz output and a sync multiplier of one. Their float32 operations give
**3,552,550 Hz** and an output interval of **41,245.0234375 raw units**.
Original `update_audio` rounds the remaining countdown to the nearest integer,
subtracts each actual service interval in float32 and adds the output interval
when emitting. Starting with the observed countdown predicts all boundaries
and final countdowns exactly. There is no phase search or inferred clock.

The complete startup window contains 1,034,454 output gaps of 41,245 raw units
and 24,827 gaps of 41,246. The shorter window contains 29,285 and 702,
respectively. Both recordings reject six mutations of actual observations:
initial countdown, output interval, display geometry, final countdown,
output timestamp and a dropped output. All 48 audio protocol tests pass,
including ABI, finite values, coverage, rounding and shared capture budget.

The native renderer's existing 3,546,895 Hz rational clock differs from this
reference frontend's display-derived cadence. These observations establish
the reference clock's owner. They do not authorize copying a frontend clock,
fitting a frequency or accepting the native waveform.

## Startup channel phase

The actual cold-start resident handler is relocated by 0x1E0 from the canonical
source image. Its empty-buffer path clears volume and DMA, sets period 124,
then executes the original 400-iteration wait. The retained disassembly and
register-write events bind this finding to the actual recorded RAM.

Channel 0's first DMA enable occurs at beam position (v=284, h=168); channel 1's
at (v=308, h=158), **5,438 chip clocks** later. Their first consumed bytes follow
those enables by **304** and **316** clocks. Thus the observed stereo byte gap
is **5,450 clocks = 5,438 + 316 - 304**. The gap already has source scheduling
and DMA owners before filtering. The wait's isolated CPU/bus charge is not
qualified, and these measured gaps are not native delay constants.

## Retention and limits

The observer is built in its separate `mixer-clock-probe-engine` directory;
the accepted clock-free reference and native executables are unchanged.
Compressed source, object, DLL and manifest witnesses have encoded and decoded
hash checks. Complete mixer, PCM, sample, RAM and execution preservation
checks pass again from compressed recording witnesses, producing identical
reports.

The pruner removed large passing raw files after their complete comparisons.
Their streams had already matched the accepted clock-free recordings exactly;
the retained compressed witnesses were reused only after checking their encoded
hashes, decoded hashes and current capture declarations. Unique clock metadata
and reports remain intact. A fresh shorter recording exercises the final
metadata budget guard and independently verifies gzip encoding and decoding
before sharing storage with the old exact witnesses. No reference RAM or clock
state enters the native game.

See the [checkpoint](figures/native_original_audio_clock_checkpoint.json) for
source identities, both complete comparison reports, mutation results, actual
startup phase events and retention hashes. Evidence is under
`build/native-audio/mixer-clock-startup/complete-capture` and
`build/native-audio/ram-mixer-clock-budget-warm`.

This batch removes an evidence gap in the original output clock. It removes
no native runtime dependency. Native sound onset, scheduling and complete
flight/combat waveform comparisons remain open. Named-state cleanup remains
outside this goal, and uninterrupted campaign completion remains waived.
