# `$C1F4AC` Golden Gate bridge-source packet inventory

Classification: **trace-proven packet-boundary inventory for the sampled matrix route**.

The frame-12,000 bounded traces distinguish raw triple blocks from mixed
data/control packets before they overwrite the reusable `$C48390` workspace.
Counts below include the initial `$C1F4AC` triple plus `$C1F528` loop entries.

| Source entry | Direct triples | Boundary after direct run | Classification |
| --- | ---: | --- | --- |
| `$C35932` | 7 | `$C3595C` | mixed: mode word, conditional triple, then control stream |
| `$C361E4` | 1 | `$C361EA` | mixed: mode word, conditional triple, then control stream |
| `$C3AD0E` | 2 | `$C3AD1A` | raw triple block in sampled route |
| `$C3B0CE` | 4 | `$C3B0E6` | raw triple block in sampled route |
| `$C3B720` | 5 | `$C3B73E` | raw triple block; `$C3B6B0` static face/control stream follows |
| `$C3B9B2` | 2 | `$C3B9BE` | raw triple block in sampled route |
| `$C3A96E` | 1 | `$C3A974` | raw triple block, but this trace returns through the early matrix gate before a sampled renderer submission |

This table does not say that a raw block is a complete model. It establishes
only which bytes are direct immutable coordinate input at this matrix entry,
and which adjacent bytes must remain code/control data until separately
decoded. `$C48390` is the overwritten renderer workspace in every row and is
never a source asset.

Authorities: `build/run031_frame12000_{c35932,c361e4,c3ad0e,c3b0ce,c3b720,c3b9b2,c3a96e}_following_trace*/trace.jsonl` and
`build/run031_frame12000_c3b720_mixed_boundary_trace/trace.jsonl`.
