# `$C3B720 -> $C3B6B0`: five-triple static bridge component

Classification: **trace-proven compact immutable source block plus static face/control stream**.

The frame-12,000 bounded trace begins at `$C1F4AC` with `A1=$C3B720` and
transforms exactly five consecutive triples:

| Source triples | Destination workspace |
| --- | --- |
| `$C3B720-$C3B73D` (5 × 3 words) | `$C48390-$C483AD` (slots 0--4) |

After the fifth store, `A1=$C3B73E`; `$C1F578` immediately takes the
`$C1F6F8` route.  Unlike `$C35932`, there is no intervening mode word or
conditional coordinate/control packet in this path.  This exact 30-byte
source range is therefore safe to classify as immutable geometry input.

The renderer then loads `A5=$C3B6B0`; its static control stream dispatches
`$C2005C`, which resolves faces against the just-written `$C48390` slots.  The
source triple block and `$C3B6B0` control stream must remain separate assets:
the former is raw geometry input, while the latter encodes face/control
selection.  Neither `$C48390` nor `$C45BEA` is source data.

Authority: `build/run031_frame12000_c3b720_mixed_boundary_trace/trace.jsonl`,
instructions 0--260.
