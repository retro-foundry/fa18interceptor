# M-map pair-transform workspace outputs

Classification: **trace-state arithmetic calculation from a byte-exact renderer transform**.

The immutable segment-68 inputs remain signed two-word pairs. For each observed
`$C2AF9C` read, this report reproduces the three signed words that the exact
`$C2AF9C-$C2AFF7` loop calculates for writes through `A5`, using the instruction
register state and the paired snapshot's matrix words. It demonstrates renderer-space triple
formation, not stored elevation or global map coordinates.

Observed pair transforms: 353; display-stage entries: 55.
All 298 adjacent same-batch output addresses advance by six bytes,
as required by three 16-bit workspace writes per pair.

| property | observed range |
| --- | --- |
| source X | -144..4352 |
| source Y | -256..4352 |
| output word 0 | -5209..6141 |
| output word 1 | -8330..8654 |
| output word 2 (computed) | 384..6144 |

The JSON companion retains every input/calculated-output row, including the live pair translation,
snapshot matrix words, detail shift, and target workspace address. The workspace itself is
mutable and not sampled at each write, so these calculated triples must not be exported as
immutable terrain vertices.

