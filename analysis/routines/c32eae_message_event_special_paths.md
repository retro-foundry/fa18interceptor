# `$C32EAE-$C32EF5`: message-event special paths

Classification: **static message-event dataflow**.

Two byte-exact slices extend the event dispatcher:

- `$C32EAE-$C32EE3` compares `$C457E0` with 3.  Equality calls `$C25246` and
  sets bit 7 of that byte; the other path clears bit 1, calls `$C1643A`, and
  writes `$FF` to `$C457D5`.  Both return through `$C32CEC`.
- `$C32EE4-$C32EF5` tests `$C457DE`, returns through `$C32CEC` when
  nonpositive, otherwise decrements it and enters `$C32F54` static-text setup.

The second path is a second static consumer of the byte that changes from 1 to
0 across run060's native 9,285--9,290 text-onset checkpoint boundary.  That
state comparison cannot decide whether this path or `$C32E7C` executed, and
does not establish either helper's role, the event's origin, or the success
predicate.
