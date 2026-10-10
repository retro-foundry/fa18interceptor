# Complete native Demo recordings at a common output rate

The current qualified native Release executable records the complete 4892-update
Demonstration at both 44100 and 48000 Hz from independently started games, using
ordinary recorded controls and no reference RAM. Both runs retain all 12680 audio
boundaries, 8248 sample requests and 554 stops. Final RAM, non-PCM counters,
voice records, playing state and all 8802 per-channel request/stop ordinals match.
Active byte phases match exactly in physical time; request payload, length,
period and volume agree on every ordinal. Event timestamps describe enclosing
host output samples and agree within one sample at the lower rate. The complete
48000 Hz WAV, trace, RAM and all counters are byte-identical to the preceding
retained native Demonstration. All gameplay heap/pool violations remain zero.

The first checker rejected 430 silent-stream phase differences. Actual
`native_audio_render` retains the remaining portion of the last enclosing host
sample after a buffer ends; `service` resets this value before a future start.
This unplayed tail depends on the output sample grid. The checker now preserves
both recorded tails and bounds each to one interval, requires zero idle volume,
and continues to compare every other field exactly. All 23986 idle-channel rows
are checked; 89 later active rows after a differing idle tail have exact physical
phase. Production audio state and playback are unchanged. The original rejection
is retained. Seven mutations reject active phase, event time, payload, channel,
omission, excessive idle tail and nonzero idle volume.

`check_music_handoffs.py` now verifies the complete actual WAV hash, stereo
16-bit format, header rate, recorded frame count and integral 20 ms duration
before interpreting native trace times. It uses that actual 44100/48000 Hz rate
for trace validation and refill rounding. Four metadata mutations at each rate
reject a wrong declaration, WAV identity, output length or duration.

At the actual common 44100 Hz rate, all 48 original initial-music requests match
native bytes, order, period and volume. Original first nonzero output remains
at stereo frames 3370056 and 3370123; native output begins at 17 on both channels.
Absolute boot timelines are different; the observed original 67-frame relative
stereo gap also remains unresolved. No offset, trimming, gain adjustment or
clock change is supplied to native playback.

Every one of the 8281 complete original Demonstration requests resolves to a
retained native payload on its actual channel. This checks payload coverage,
not ordered handoffs, fetched-sample lifetime, level, pitch or waveform equality.
An additional parameter-coverage diagnostic finds 165/216/2 original requests
on channels 0/1/2 whose complete payload/period/volume tuple is absent from the
native recording; channel 3 has none. These recordings have different runtime
contexts and field cadences, so this diagnostic does not establish a native
behavior fault. It demonstrates why payload coverage alone cannot close complete
sound acceptance. The actual requests and diagnostic remain retained.

Release and Debug each pass the updated connected output-rate CTest and cleanup.
Their intro, Free Flight and final-combat comparisons retain 2086, 5890 and
55357 ordered sound events, complete game outputs and all seven negative
controls. All 48 original audio protocol tests pass. Complete compressed
startup/payload reports recheck identically; complete native rate comparisons
and metadata/physical-sound controls also pass from compressed artifacts.

The [checkpoint](figures/native_common_rate_demo_checkpoint.json) binds current
sources, executable identities, terminal test logs, complete reports, retained
rejection and parameter diagnostic. Evidence is under
`build/native-audio/common-rate-complete-demo`. WAV, trace and final RAM are
compressed only after complete identity and decoded-byte verification; passing
raw copies are removed. Original/native onset, complete handoffs and flight/combat
waveforms remain unaccepted. Native sound behavior is unchanged by this batch.
