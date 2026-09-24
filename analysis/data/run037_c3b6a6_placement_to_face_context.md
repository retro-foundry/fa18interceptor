# Run037 `$C3B6A6`: flat placement target to 3D face context

Classification: **bounded placement-control-to-face-context join**. This
connects one sampled flat map-placement descriptor to the already proven
`$C3B6B0` 3D component face family. It does not establish a positioned world
mesh, complete object ownership, or LOD.

In the sealed run037 stable-map checkpoint, immutable template `$C42789`
contains header `$2A` and source pair `($0E00,$0E00)`. The placement builder
emits its descriptor-qualified cache tuple as `(368, 0, -144)`, with
descriptor `$C224D0` and descriptor `+8` target `$C3B6A6`.

At the live `$C1F6F8` control-stream entry, that target resolves to stream
`$C3B6AE`. A 3,000-instruction bounded trace from that entry reaches three
`$C2FF48` submissions at trace indices 513, 1,205, and 2,171, each with
`A5=$C3B6B0`; it also reaches `$C2FA7E` at index 554 with the same context.
The established `$C3B720 -> $C3B6B0` component boundary provides the separate
immutable local-triple and face-topology authority for that context.

Thus this trace makes the layer distinction concrete:

```text
static template pair -> placement (X, 0, Z) -> $C3B6A6/$C3B6AE control
                       -> $C3B6B0 3D face/line family
```

The trace begins after the earlier matrix-source selection, so it does not by
itself prove that this individual placement transformed `$C3B720` in this
exact draw. It proves the descriptor target's renderer context; the local 3D
component association remains grounded in its separate bounded authority.

Authority: `analysis/data/workspace_template_copies_run037_m_map_template_to_placement_3f.md`,
`build/run037_c3b6ae_control_trace/selected_stream_trace.jsonl`, and
`analysis/data/c3b720_c3b6b0_static_component_boundary.md`.
