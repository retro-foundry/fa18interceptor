# Run060 input-control stage selects and updates root record

Classification: **scenario-backed input/control dataflow**, not a spatial or
flight-model identification.

Authority is the sealed deterministic run060 replay and the bounded trace:

```text
python scripts/trace_from_breakpoint.py --restore captures/run060/restored-state.bin --playback captures/run060/playback.e9k --address 0xC13E10 --arm-frame 8241 --return-pc 0xC25D84 --frames 8250 --max-instructions 3000 --ignore-future-input --output build/run060_frame8241_c13e10_input_trace
```

The run's recorded input is delivered in ordinary replay before the breakpoint;
the breakpoint is reached at GUI replay frame 8,245 and the bounded path
returns to `$C25D84` after 277 instructions.  No later input is delivered
while the interval is stepped.

## Accepted facts

- At `$C13E10`, `$C459B4 = $0000` and `$C18210 = $00C46184`.
  Thus this real run060 input-consumer invocation selects the root entry of
  the 512-byte `$C46184 + (index << 9)` record table.
- `$C45778 = $0160` and `$C4577C = $0160` at both interval endpoints.
  `$C13E10` reads `$C45778`; it is an input/control-stage input in this
  scenario, but its physical axis is still unassigned.
- Comparing the full root record before and after the interval finds only two
  changed bytes:

  | Record offset | Before | After | Proven writer |
  | --- | ---: | ---: | --- |
  | `+$39` | `$02` | `$01` | `$C1406A`: increment after a byte store through the stage's `-$0C(A6)` pointer, resolved live as `$C461BD` |
  | `+$75` | `$8A` | `$74` | `$C142CA`: `SUB.L D1,$72(A0)`, with `$A0=$C46184`, `D1=$16` |

- The same interval also executes stores to selected-record `+$26`, `+$76`,
  `+$78`, and `+$6E`; their before/after values happen to be unchanged in
  this particular path.

## Rejected interpretation

This proves that record zero participates in a real run060 input-control
update. It does **not** prove that the record is the aircraft position,
orientation, velocity, camera, or complete flight-model state. In particular,
the `+$14/+$18/+$1C` fields sampled late in run060 remain constant across
continued recorded flight input. A spatial/player claim requires a producer to
renderer/camera trace or a controlled relation to visible aircraft motion.

## Next evidence

Capture equivalent bounded intervals for contrasting real control inputs and
then trace the changed record fields into the projection/render or camera
consumer. That can distinguish transient control fields from aircraft state.
