# run060 root pose integrator

Classification: **runtime writer trace / behavioural dataflow**. These traces
identify the active update paths that commit the moving root pose tuple in the
qualification replay. They do not yet assign physical axis names or derive
the joystick-to-delta formula.

## Vertical component

At the descent boundary (checkpoint frame 925), each observed `$C14D32`
instruction resolves `A0` to `$C46184` and executes:

```text
D0 = long[root + $18] - local_delta[-$14(A6)]
MOVE.L D0,$18(A0)
```

The writes are committed, not immediately restored on this route:

```text
$18: $00072301 -> $0006FD81 -> $0006D501 -> $0006AD41 -> $00068601
```

This is the `update_c13d84_record_offset18` path. Its following guards can
instead restore a bounded value on other conditions; at frame 700 that
alternative did occur, so a single temporary write must not be interpreted as
motion without the paired final state. The frame-925 trace proves the
committed descent path.

## Horizontal pair

The active indexed-update publication at `$C25E6E-$C25E72` writes:

```text
MOVE.L D2,$14(A1)
MOVE.L D4,$1C(A1)
```

In both the frame-700 motion and frame-925 descent traces, `A1` resolves to
`$C46184`. `$C25E72` visibly advances `+$1C`; the paired `+$14` store can be
byte-identical on a given update and therefore absent from a change-only
watch report. At later run060 samples, both components do change.

Together with the projection and cockpit-altitude consumers, these direct
writers establish `$C46184+$14/+18/+1C` as the active **flight pose integrator
state** in this qualification scenario. It remains possible that the cockpit
camera uses that state directly rather than that the record is an aircraft-only
object.

Authority:
`build/run060_frame00700_root_motion_writer/memory_writes.json`,
`build/run060_frame00925_root_descent_writer/memory_writes.json`,
`source_amiga/observed/update_c13d84_record_offset18.asm`, and
`source_amiga/observed/publish_indexed_update_delta_pair.asm`.
