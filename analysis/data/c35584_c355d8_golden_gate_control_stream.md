# `$C35584` / `$C355D8`: Golden Gate control-stream fragments

Classification: **runtime-backed static bridge-family control data inside original segment 42 CODE**. These are not immutable vertex records.

Segment 42 is verified byte-for-byte outside relocation operands at `$C35568-$C361FF`. Bounded polygon collection reaches the following active stream cursors before `$C24CFE → $C2FF48` submission:

| Cursor | Scene oracle | Polygon result |
| --- | --- | --- |
| `$C355D8` | Frame 12,000 Golden Gate bridge view | Two closed polygons / seven edges |
| `$C35584` | Frame 12,600 external Golden Gate pass | Two closed polygons / seven edges |

The exact six-byte fragments beginning at each cursor are registered as inline data because the renderer walker consumes them as control words. Their matching segment-42 ownership separates the Golden Gate bridge control family from the later-bridge `$C36298` Hunk-43 stream and the external-aircraft `$C3925x` Hunk-52 stream.

The associated plots are [frame 12,000 `$C355D8`](../plots/golden_gate_c355d8_polygons_orthographic.svg) and [frame 12,600 `$C35584`](../plots/golden_gate_external_c35584_polygons_orthographic.svg). A complete bridge source-mesh claim still requires tracing from these control words to the first immutable coordinate producer.
