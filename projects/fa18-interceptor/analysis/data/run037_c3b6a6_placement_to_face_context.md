# Run037 `$C3B6A6`: flat placement target to 3D face context

Classification: **bounded placement-control-to-face-context join**. This
connects one sampled flat map-placement descriptor to the already proven
`$C3B6B0` 3D component face family. It does not establish a positioned world
mesh, complete object ownership, or LOD.

The placement inventory, descriptor handoff, walker, and submissions below
are all in one uninterrupted sealed run037 trace. Between the `$C3B6AE`
walker and the first four submissions, no later `$C1F6F8` walker or `$C1F4AC`
matrix-source entry occurs. Those four primitives are therefore bounded outputs
of this placement record's control pass; this still does not identify a named
world feature or complete model.

In that trace, immutable template `$C42789`
contains header `$2A` and source pair `($0E00,$0E00)`. The placement builder
emits its descriptor-qualified cache tuple as `(368, 0, -144)`, with
descriptor `$C224D0` and descriptor `+8` target `$C3B6A6`.

The execution proves the cursor handoff rather than inferring it from
the nearby walker: `$C1CC70` stores `$C3B6A6` to `$C45A36`; `$C1EF10` then
publishes `$C3B6AE`; and `$C1F70E` loads `$C3B6AE` into `A5` for the live
`$C1F6F8` walker. (`A1=$C3B73E` at walker entry is a separate cursor.)
The walker begins at trace index 148,233. Its first four following `$C2FF48`
submissions occur at indices 148,747, 150,138, 151,104, and 152,010, each
with `A5=$C3B6B0`. No intervening walker or matrix-source entry occurs. They
are the bounded primitive batch owned by this control pass; the longer capture
contains later submissions, so this does not claim the component's complete
model or all of its later draws.
The established `$C3B720 -> $C3B6B0` component boundary provides the separate
immutable local-triple and face-topology authority for that context.

The upstream placement-vector conversion is byte-exact: `$C1CB74` copies the
record's three words to `$C45B2A` and shifts each left eight bits into
`$C45B30/$C45B34/$C45B38`. This record's `(368,0,-144)` consequently becomes
live `(94208,0,-36864)` before the `$C1F464/$C1F4AC` matrix path.

Thus this trace makes the layer distinction concrete:

```text
static template pair -> placement (X, 0, Z) -> placement << 8 live vector
                       -> $C3B6A6 descriptor field
                       -> $C1EE14 cursor -> $C3B6AE in walker A5
                       -> $C3B6B0 3D face/line family
```

This continuous trace proves that this record's shifted live vector reaches
the `$C3B720` transform and its following bounded `$C3B6B0` primitive batch.
It still does not give the component an in-game name, establish all of its
topology, or turn its view-dependent matrix outputs into static global mesh
vertices.

Authority: `analysis/data/workspace_template_copies_run037_m_map_placement_to_control_20f.md`,
`analysis/data/run037_m_map_placement_to_control_20f_handoff.md`,
`build/run037_m_map_placement_to_control_20f_trace/trace.jsonl`, and
`analysis/data/c3b720_c3b6b0_static_component_boundary.md`.
