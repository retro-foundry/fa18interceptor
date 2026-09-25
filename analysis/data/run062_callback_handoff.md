# Run062 `$C10DAE` to `$C11788` callback handoff

Classification: **sealed-replay callback-state transition with bounded
instruction trace**.

The native GUI-frame-2,000 checkpoint was captured from sealed `run062`:

```text
build/run062_frame2000_checkpoint/frame_02000_state.bin
```

At restored replay frame zero, its Slow-RAM callback slot contains
`$C1820C = $C10DAE`. Sampling that four-byte slot for 240 no-input replay
frames records the first change at replay frame 133:

```text
$C1820C: $C10DAE -> $C11788
```

The full sampler output is retained in the ignored build artifact
`build/run062_frame2000_callback_slot_sampler/memory_region_mutations.json`.
The same sampler later records `$C11788 -> $C118A0` at frame 223 and
`$C118A0 -> $C118E6` at frame 225, consistent with the separately traced
failure-message callback pipeline.

A bounded trace armed immediately before the first transition enters
`$C10DAE` at replay frame 132. It observes bit 9 set in `$C46184`, a zero low
nibble in `$C46200`, and a nonzero `$C4578C`; the exact reconstructed
gate-positive path then clears its documented flag bits and proceeds toward
the literal `$C11788` callback-slot installation. The trace is retained at
`build/run062_frame2000_c10dae_handoff_trace/`.

This joins the run062 failure presentation to the same `$C10DAE` postflight
stage already observed after the run060 success-selector activation. It does
not identify the earlier qualification outcome predicate, classify the tested
fields, or show that this shared progression selects success versus failure.
