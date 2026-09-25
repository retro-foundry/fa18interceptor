# Run037 `$C3B6A6`: flat placement target to 3D face context

Classification: **bounded placement-control-to-face-context join**. This
connects one sampled flat map-placement descriptor to the already proven
`$C3B6B0` 3D component face family. It does not establish a positioned world
mesh, complete object ownership, or LOD.

The placement inventory and control trace are separate sealed run037 captures.
Their matching template/descriptor target makes this a replay-correlated
instance-class join, not yet an uninterrupted trace from the exact runtime
placement-record address to its individual primitives.

In the sealed run037 stable-map checkpoint, immutable template `$C42789`
contains header `$2A` and source pair `($0E00,$0E00)`. The placement builder
emits its descriptor-qualified cache tuple as `(368, 0, -144)`, with
descriptor `$C224D0` and descriptor `+8` target `$C3B6A6`.

The control-trace execution proves the cursor handoff rather than inferring it from
the nearby walker: `$C1CC70` stores `$C3B6A6` to `$C45A36`; `$C1EF10` then
publishes `$C3B6AE`; and `$C1F70E` loads `$C3B6AE` into `A5` for the live
`$C1F6F8` walker. (`A1=$C3B73E` at walker entry is a separate cursor.) A
6,000-instruction bounded trace from that entry reaches four
`$C2FF48` submissions at trace indices 513, 1,205, 2,171, and 3,077, each
with `A5=$C3B6B0`; it also reaches `$C2FA7E` at index 554 with the same
context. No further `$C2FF48`, `$C212B0`, or `$C2FA7E` entry occurs before
the 6,000-instruction bound. This is the complete observed polygon/line batch
for this control-stream pass.
The established `$C3B720 -> $C3B6B0` component boundary provides the separate
immutable local-triple and face-topology authority for that context.

Thus this trace makes the layer distinction concrete:

```text
static template pair -> placement (X, 0, Z) -> $C3B6A6 descriptor field
                       -> $C1EE14 cursor -> $C3B6AE in walker A5
                       -> $C3B6B0 3D face/line family
```

The trace begins after the earlier matrix-source selection, so it does not by
itself prove that this individual placement transformed `$C3B720` in this
exact draw. It proves the descriptor target's renderer context; the local 3D
component association remains grounded in its separate bounded authority.

Authority: `analysis/data/workspace_template_copies_run037_m_map_template_to_placement_3f.md`,
`analysis/data/run037_m_map_descriptor_control_handoff.md`,
`build/run037_c3b6ae_control_trace_6000/selected_stream_trace.jsonl`, and
`analysis/data/c3b720_c3b6b0_static_component_boundary.md`.
