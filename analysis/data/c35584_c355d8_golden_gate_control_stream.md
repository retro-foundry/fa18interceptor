# `$C35584` / `$C355D8`: Golden Gate control-stream fragments

Classification: **runtime-backed static bridge-family control data inside original segment 42 CODE**. These are not immutable vertex records.

Segment 42 is verified byte-for-byte outside relocation operands at `$C35568-$C361FF`. Bounded polygon collection reaches the following active stream cursors before `$C24CFE → $C2FF48` submission:

| Cursor | Scene oracle | Polygon result |
| --- | --- | --- |
| `$C355D8` | Frame 12,000 Golden Gate bridge view | Two closed polygons / seven edges |
| `$C35584` | Frame 12,600 external Golden Gate pass | Two closed polygons / seven edges |

The exact six-byte fragments beginning at each cursor are registered as inline data because the renderer walker consumes them as control words. Their matching segment-42 ownership separates the Golden Gate bridge control family from the later-bridge `$C36298` Hunk-43 stream and the external-aircraft `$C3925x` Hunk-52 stream.

The associated plots are [frame 12,000 `$C355D8`](../plots/golden_gate_c355d8_polygons_orthographic.svg) and [frame 12,600 `$C35584`](../plots/golden_gate_external_c35584_polygons_orthographic.svg). A complete bridge source-mesh claim still requires tracing from these control words to the first immutable coordinate producer.

## run035 red-landmark line contexts

The sealed run035 replay supplies a separate class of observed segment-42
contexts: `$C2FA7E` line-emitter entries retaining these static `A5` values
while the user-defined red Golden Gate pixels are in the outside viewport.

| red-image sample | static `A5` context | line entries | observed static `A2` records |
| ---: | --- | ---: | --- |
| 4,250 | `$C3559A`, `$C355D2` | 2 | `$C358B8` |
| 7,000 | `$C355CE` | 4 | `$C358A4,$C358A8,$C358AC,$C358B0` |
| 8,000 | `$C3558A` (17), `$C355BC` (1) | 18 | `$C35836..$C35876`, `$C357EA` |

These are line-control contexts and line-list addresses, not vertex bases.
Their selected groups grow along this replay, making them a range/detail
candidate. The recording changes camera state, so this does not establish a
distance-only selector or a terrain LOD rule. The landmark correlation and
full qualification are in
[`run035_golden_gate_line_detail_candidate.md`](run035_golden_gate_line_detail_candidate.md).
