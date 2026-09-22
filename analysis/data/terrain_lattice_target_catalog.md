# Terrain selector lattice target catalog

Classification: **control-window target aggregation**. Each target is reached
through an exported static template record's descriptor association in the origin
control window. This catalog groups selector-page membership; it is not a stable
world-space instance list, terrain mesh, or LOD assignment.

| Descriptor target | template records | static streams | selector-bin cells |
| --- | ---: | ---: | ---: |
| $C08718 | 3 | 1 | 14 |
| $C08798 | 4 | 1 | 14 |
| $C087AC | 2 | 1 | 14 |
| $C087FE | 1 | 1 | 14 |
| $C0883E | 1 | 1 | 14 |
| $C08892 | 2 | 1 | 14 |
| $C08928 | 1 | 1 | 14 |
| $C35568 | 1 | 1 | 14 |
| $C355A0 | 1 | 1 | 14 |
| $C35BD0 | 1 | 1 | 14 |
| $C35BFC | 1 | 1 | 14 |
| $C35D88 | 1 | 1 | 14 |
| $C35E9E | 1 | 1 | 14 |
| $C36208 | 1 | 1 | 14 |
| $C36212 | 1 | 1 | 14 |
| $C3623E | 4 | 1 | 14 |
| $C366E6 | 1 | 1 | 14 |
| $C368D0 | 1 | 1 | 14 |
| $C36A28 | 1 | 1 | 14 |
| $C36E6A | 1 | 1 | 14 |
| $C37218 | 1 | 1 | 14 |
| $C3723E | 1 | 1 | 14 |
| $C3737C | 1 | 1 | 14 |
| $C373A2 | 1 | 1 | 14 |
| $C37680 | 1 | 1 | 14 |
| $C376AC | 1 | 1 | 14 |
| $C37990 | 1 | 1 | 14 |
| $C379BC | 1 | 1 | 14 |
| $C37B4C | 1 | 1 | 14 |
| $C37B78 | 1 | 1 | 14 |
| $C37CF8 | 1 | 1 | 14 |
| $C37D94 | 1 | 1 | 14 |
| $C37E56 | 1 | 1 | 14 |
| $C37EA6 | 1 | 1 | 14 |
| $C37F80 | 1 | 1 | 14 |
| $C380D8 | 1 | 1 | 14 |
| $C381FC | 1 | 1 | 14 |
| $C38228 | 1 | 1 | 14 |
| $C3A942 | 4 | 4 | 1024 |
| $C3B4F8 | 3 | 2 | 18 |
| $C3B6A6 | 2 | 2 | 1024 |
| $C3B960 | 1 | 1 | 14 |
| $C3BBD0 | 1 | 1 | 1024 |
| $C44500 | 1 | 1 | 14 |
| $C4455A | 1 | 1 | 14 |
| $C445A2 | 1 | 1 | 14 |
| $C445DC | 5 | 4 | 96 |
| $C44612 | 3 | 3 | 90 |
| $C4466E | 1 | 1 | 14 |
| $C44732 | 1 | 1 | 14 |
| $C4477C | 1 | 1 | 14 |
| $C447C6 | 1 | 1 | 14 |
| $C44812 | 1 | 1 | 14 |
| $C4483C | 1 | 1 | 14 |
| $C448B6 | 1 | 1 | 14 |
| $C44970 | 1 | 1 | 14 |
| $C44B78 | 1 | 1 | 14 |
| $C44C10 | 1 | 1 | 14 |
| $C44C7A | 1 | 1 | 14 |
| $C44CE4 | 1 | 1 | 14 |
| $C44D4E | 1 | 1 | 14 |
| $C44DB8 | 1 | 1 | 14 |
| $C44E22 | 1 | 1 | 14 |
| $C44E8C | 1 | 1 | 14 |
| $C44EF6 | 1 | 1 | 14 |
| $C45128 | 1 | 1 | 14 |
| $C45192 | 1 | 1 | 14 |
| $C451FC | 1 | 1 | 14 |
| $C45294 | 1 | 1 | 14 |
| $C4532C | 1 | 1 | 14 |
| $C453C4 | 1 | 1 | 14 |
| $C4545C | 1 | 1 | 14 |
| $C454F4 | 1 | 1 | 14 |
| $C4558C | 1 | 1 | 14 |

A target can occur in many cells because templates and descriptors are reusable.
The separate refresh-window experiment proves the same template source can use a
different descriptor/target in another context, so this aggregation must not be
used as an unconditional map-object or LOD table.
