# `$C30306-$C3040A`: renderer lane-state builder

Classification: **bounded renderer-workspace dataflow**. This is not a static
graphics-resource claim.

Authority: `build/attract_cockpit_1800_tenframe_trace/trace.jsonl`. The frame-3
invocation runs from `$C30306` through `$C3040A` after consuming records from
`$C4B390`. It writes a transient lane state block at `$C45960-$C45982` and
starts a blit at `$C30404`.

At `$C30394-$C303C4`, the routine derives a longword from the current lane
calculation plus `$C456E2`, then publishes it to `$C45960` and `$C45964`; it
also stores the calculated offset at `$C45968` and size word at `$C4596E`.
For the frame-3 observed invocation, the published pointer is `$007B6A` and
the `BLTSIZE` value is `$0302`. Later invocations publish different values,
including `$013C76`, `$00719E`, and `$007B6A`.

The hardware writes at `$C303EC-$C30404` place the same current longword in
`BLTAPT` and `BLTDPT` before triggering `BLTSIZE`; therefore the state is a
per-lane blitter workspace. It cannot be exported as an immutable graphics
asset. The later `$C304B2` helper reuses `$C45960` in A/B/D channels, and the
active-plane `$C306AE` jobs inherit only parts of that Custom-register state.

The exact A/B/C/D state at each trigger is in
`analysis/attract_cockpit_blitter_jobs.json`. A future graphics-source claim
requires a job whose enabled input channel points to a stable, separately
proven resource range.
