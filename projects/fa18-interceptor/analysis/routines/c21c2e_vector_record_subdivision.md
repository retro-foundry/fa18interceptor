# `$C21C2E`: selected vector-record subdivision

Classification: **runtime-backed geometric record preparation**.

`source_amiga/observed/subdivide_two_selected_vector_records.asm` is byte
exact for `$C21C2E-$C21C85`.

## Contract

The entry selects one record at `$C46184 + $C459B6 + $A4`, then a second record
at `$C48390 + (A2)+`. For each, the shared helper reads six words at offset
zero, treats `D0-D2` and `D3-D5` as two three-word values, and stores successive
midpoint-derived three-word values at offsets `$1E` and `$24`. It returns zero.

The exact arithmetic is:

```text
delta = second - first
half_point = first + (delta >> 1)
quarter_point = first + ((delta >> 1) >> 1)
```

These equations describe the register operations only; coordinate axes and
the semantic purpose of the two records remain unassigned.

## Runtime anchor

In `build/run031_frame7500_c1f6f8_probe/`, the external-aircraft frame's
`$C1F942` dispatcher calls `$C21C2E` and receives its zero return at `$C1F944`
before dispatching the `$C38B0A` projected-edge list through `$C212B0`. This
places the helper in the live external-view geometry path without claiming it
constructs a specific aircraft feature.
