# Run060/run062 sampled postflight field comparison

Classification: **checkpoint comparison only**. This comparison rules out a
claim that the first sampled bytes of the common postflight fields are the
immediate success/failure discriminator at these two later checkpoints.

Inputs are the existing native Slow-RAM exports:

- `build/run060_frame09284_state_view/slow.bin`
- `build/run062_frame2000_state_view/slow.bin`

| Address | run060 frame 9,284 | run062 frame 2,000 |
| --- | --- | --- |
| `$C46184` (first four bytes) | `11 C8 C0 82` | `11 C8 20 8A` |
| `$C46200` (first four bytes) | `00 53 14 00` | `00 53 14 00` |
| `$C4578C` (first four bytes) | `01 01 01 01` | `01 01 01 01` |
| `$C45785` (first four bytes) | `00 00 01 00` | `00 00 01 00` |
| `$C458A6` (first four bytes) | `09 00 00 00` | `09 00 00 00` |
| `$C1820C` callback slot | `$C110A4` | `$C10DAE` |
| `$C45AD6` (first four bytes) | `00 00 00 00` | `FF CC 00 00` |

The differing callback/countdown states are expected later presentation
progression. The matching first words/bytes do not prove identical full record
state, nor do they identify the landing predicate, result-state writer, or
persistence behavior. They only exclude treating these sampled leading fields
as a sufficient direct success/failure test.

## Later run062 transition

The run062 frame-2,000 checkpoint is not the instant at which its callback
executes. A no-input, frame-by-frame sample from that same native checkpoint
records the leading `$C46184` word as `$11C8` through replay frame 124, then
as `$93C8` on frame 125. It remains `$93C8` until the `$C10DAE` callback is
entered on frame 132, where the bounded trace observes bit 9 set; the
gate-positive callback then clears it to `$91C8`.

The sample is retained at
`build/run062_frame2000_postflight_word_samples.json`; the callback trace is
`build/run062_frame2000_c10dae_handoff_trace/trace.jsonl`. A CPU and an
all-source write watch on `$C46184` both miss this sampled mutation, so neither
identifies a writer PC. The update is therefore evidence of a timed
postflight-record transition, not yet of its producer or a qualification
predicate.
