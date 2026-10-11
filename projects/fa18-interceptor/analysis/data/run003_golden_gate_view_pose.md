# Run003 Golden Gate view-space pose

Classification: **trace-observed view-space pose, not static world geometry**.

Matrix `$C45BD8`: `[168, 0, 0, 0, 0, 252, 0, -128, 0]`.

## `$C3559A` from `$C35BAA`

Trace index `85163`; local shift `8`; live terms `[461, -1536, 273]`.

| local triple | pre-matrix triple | matrix output |
| --- | --- | --- |
| [0, 0, 3744] | [461, -1536, 287] | [302, 282, 768] |
| [0, 416, -3168] | [461, -1535, 260] | [302, 255, 767] |

## `$C355D2` from `$C35BC2`

Trace index `85861`; local shift `8`; live terms `[461, -1536, 249]`.

| local triple | pre-matrix triple | matrix output |
| --- | --- | --- |
| [0, 0, -3744] | [461, -1536, 234] | [302, 230, 768] |
| [0, 416, 3168] | [461, -1535, 261] | [302, 256, 767] |

Each pose applies the byte-exact C1F4AC local-shift, live-term addition, and signed matrix product. The output is view-dependent renderer workspace geometry, not static global bridge vertices.
