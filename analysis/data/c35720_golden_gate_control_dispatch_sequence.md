# `$C35720`: Golden Gate control-block dispatch sequence

Classification: **runtime-backed scene-control data**.

This sequence was captured from the run031 frame-12,000 no-input trace after
the record walker selected the active control stream at `$C355A0`. The stream
control word `$A1B8` resolves `A2` to `$C35720`; selector words are then
dispatched by `$C1F942`.

| Selector word | Handler | Payload address | Observed role |
| --- | --- | --- | --- |
| `$8024` | `$C21490` | `$C35722` | four-point projection preparation |
| `$801C` | `$C2159E` | `$C35728` | conditional three-point preparation |
| `$802C` | `$C211DC` | `$C35734` | counted projection-record submission (13 records) |
| `$8034` | `$C212B0` | `$C3573A` | offset-pair segment submission |
| `$803C` | `$C2139E` | `$C35756` | unresolved projection variant |
| `$8040` | `$C21412` | `$C35762` | unresolved projection variant |
| `$8028` | `$C21500` | `$C3576C` | unresolved projection variant |
| `$8020` | `$C2168A` | `$C35772` | unresolved projection variant |
| `$8030` | `$C2122A` | `$C35780` | unresolved projection variant |
| `$8038` | `$C2131C` | `$C35788` | unresolved projection variant |

All selector words have bit 15 set and use the `$C1FCE8` dispatch table after
`$C1F910` masks them to their low 14 bits. The trace establishes that the
table is an active scene-control packet, but it does not establish a one-to-one
mapping from an entry to a particular visible bridge member.

## Evidence

`build/run031_frame12000_golden_gate_c1f6f8_probe/trace.jsonl` records the
`$C1F942` indirect dispatches at trace indices 125, 148, 212, 527, 689, 1085,
1138, 1161, 1256, and 1681. The `$C211DC` entry at index 212 produces the
13 `$C2EE4A` projection submissions documented separately in
`c483ba_golden_gate_projection_records.md`.
