# `$C32EAE-$C32EF5`: message-event special paths

Classification: **static message-event dataflow**.

Two byte-exact slices extend the event dispatcher:

- `$C32EAE-$C32EE3` compares `$C457E0` with 3.  Equality calls `$C25246` and
  sets bit 7 of that byte; the other path clears bit 1, calls `$C1643A`, and
  writes `$FF` to `$C457D5`.  Both return through `$C32CEC`.
- `$C32EE4-$C32EF5` tests `$C457DE`, returns through `$C32CEC` when
  nonpositive, otherwise decrements it and enters `$C32F54` static-text setup.

## Run060 onset consumer observation

The bounded no-input trace
`build/run060_frame09285_c32ee4_short_trace/trace.jsonl` restores the fresh
native GUI-frame-9,285 checkpoint and hits `$C32EE4` on its first following
frame.  Its positive test path executes `SUBQ.B #1,$C457DE` and then branches
to `$C32F54`.  Since the checkpoint value is one, this identifies `$C32EE4`
as the observed `1 -> 0` consumer across the 9,285--9,290 onset boundary.

A matching five-frame no-input breakpoint test does not reach `$C32E7C`.
This is bounded to this checkpoint window: it does not establish either
helper's role, the event's origin, the broad lifetime of the byte, or the
success predicate.
