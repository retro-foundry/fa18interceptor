# `$C1C54E`: projection seed transform

Classification: **scenario-backed port contract**.

The source selects a literal signed seed from the active record type, applies
the nine signed words at `+$92..+$A2` as a 2.14 fixed-point 3x3 matrix, shifts
each dot product right by six, and adds the three results to the record base
tuple at `+$14/+$18/+$1C`.

Seed selection is byte-exact for the observed branches:

```text
record type $30 -> (0, 1, -5)
record type $11 -> (0, 5, 20)
other          -> (0, 4, 18)
```

The run060 identity-matrix packet uses type `$11`, base
`(11982C00, 00007708, 1059A000)`, and matrix diagonal `$4000`. Its output is:

```text
(11982C00, 00007C08, 1059B400)
```

`port/projection_seed.c` exposes this through named matrix, base, and result
structures. It does not reproduce the active record layout or original
workspace addresses. Authority: `analysis/data/run060_root_projection_packet.md`
and the observed `$C1C54E` source slice.
