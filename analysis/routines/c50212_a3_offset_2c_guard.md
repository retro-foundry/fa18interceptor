# `$C50212-$C5027A`: delayed record-update command interpreter

Classification: **dataflow-backed bounded command interpreter**. The routine
is shared by the four-record iterator and occurs in 15 independent P-code
capture packets. It proves a record-local delayed-update mechanism, not a UI,
flight, object, or gameplay identity for those records.

## Contract

`A3+$2C` is a record-local countdown. A zero value returns immediately. A
nonzero value is decremented; execution returns until it reaches zero. On that
expiry, the routine resumes a cursor at `A3+$34`, relative to command base
`A3+$30`, and consumes eight-byte command pairs:

```text
pair = (first_longword, second_longword)

first < $2C:  store second to A3 + first
first = $2C:  save second as the next countdown and save the command cursor
first > $2C:  use first - $40 to select a record-local subcount at A3+$24;
              a nonzero subcount is decremented, then second is used as a
              cursor relative to the command base unless that decrement
              reaches zero
```

On a `$2C` command with zero second longword, it additionally clears `(A2)`
and calls `$C4FFB4` with `D0=D3`. The purpose of that callback is unassigned;
this is a precise control-flow and side-effect statement, not a completion
notification claim.

The byte-exact implementation is
[`apply_delayed_record_updates`](../../source_amiga/observed/branch_if_a3_offset_2c_zero.asm).
The two encoded `CMPI.L #0` instructions are retained literally because their
condition-code behavior is part of the original contract.

## Evidence boundary

The immediate zero-return route is directly observed in 15 P-code exports
covering menu, attract, human flight, and run024 crash-result presentation.
The nonzero path is statically reconstructed from the identical runtime image
used by those captures; no capture currently observes an expiry or an encoded
command pair. Consequently, the countdown/command interpretation is supported
by exact dataflow, but individual command streams, record ownership, and the
callback's game role remain open.
