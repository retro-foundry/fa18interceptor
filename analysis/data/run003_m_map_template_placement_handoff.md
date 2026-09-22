# `M` map-mode terrain-template placement handoff

Classification: **scenario-backed static-template-to-placement dataflow during
the stable map display**.

Starting from the sealed five-frame post-`M` checkpoint, a 25-frame no-input,
instruction-stepped trace reaches the normal stable map frame exactly: its
frame-25 video SHA-256 is
`1828e0c0717c86ddb8ee3d4afa1ce15b2f0c9b028b8a0201cbae65cdc462f9f3`,
the same hash recorded by the ordinary full-frame replay. This makes the
trace a valid map-mode observation despite its instruction-stepping caveat.

Within that window, the established template copier `$C1D488` reads 107 exact
records from immutable segments 66--67 and writes their transformed headers
and two source words into mutable cell workspace. The placement builder later
reads 103 of those cells at `$C1DD36`, emitting a descriptor-qualified runtime
placement record for each. Every one of those 103 emitted triples has a zero
middle coordinate word.

The export also preserves the selected descriptor's immutable `+8` field for
every emitted record. On the established generic route this is the value
copied to `$C45A36`, the later projection control-stream input. In this
map-mode set those fields include `$C0DD30`, `$C0DB50`, `$C08758`, `$C37854`,
`$C3A858`, and scene-family addresses such as `$C3723E`/`$C37218`. They are
therefore concrete descriptor-to-control candidates tied to active flat map
placements. A descriptor type gate can bypass the generic route, so the table
does not assert that each candidate was projected or rasterized.

The machine-readable [copy-to-placement inventory](workspace_template_copies_run003_m_appearance_25f.json)
retains every source address, source byte/words, workspace cell, descriptor,
and emitted triple. Its [readable table](workspace_template_copies_run003_m_appearance_25f.md)
and [X/Z plot](../plots/workspace_template_placements_xz_run003_m_appearance_25f.svg)
are a visual/auditable slice of the active flat placement layer.
The compact [target catalog](run003_m_map_template_target_catalog.md) groups
the 103 emitted placements into 83 descriptor `+8` field candidates with
their source membership and observed X/Z bounds.

This is the strongest direct dataflow connection yet obtained in the `M`
display scenario:

```text
immutable template record
  -> $C1D488/$C1D4BC mutable cell copy
  -> $C1DD36 placement-builder read
  -> descriptor-qualified X/0/Z placement
```

It does not prove that a particular placement writes a particular coastline
pixel. The normal map display interval still performs other update/render
work, and no trace has yet followed an individual placement through its
descriptor/control stream to the map bitplanes. It also does not turn the
bounded active set into a complete global map, assign the two stored source
words a global-coordinate meaning, or prove a universal no-elevation rule.

A separate no-input map-mode collector does reach the `$C1F6F8` projection
walker with static segment-42/43 control streams. That establishes map-mode
3D control activity, but not ownership of one of the placements above; see
[the control-stream entries](run003_m_map_control_streams.md). One map-mode
segment-43 stream is followed through its transformed-triple and
projected-segment route in [the `$C36214` control trace](run003_m_map_c36214_control_trace.md).

Authority: sealed `captures/run003`; `build/run003_m_visual_5/state.bin`; the
deterministic `build/run003_m_map_appearance_25f_trace` trace/snapshots; and
the independently normal-frame `build/run003_m5_noinput_25` snapshot. The
trace uses no input after restoring the post-`M` checkpoint.

Reproduce:

```text
python scripts/engine9000_bridge.py \
  --restore build/run003_m_visual_5/state.bin \
  --config captures/run003/config.uae \
  --frames 0 --trace-frames 25 \
  --output build/run003_m_map_appearance_25f_trace

python scripts/inventory_workspace_template_copies.py \
  --trace-directory build/run003_m_map_appearance_25f_trace \
  --output-suffix _run003_m_appearance_25f
```
