# run037 M-map terrain-template placement handoff

Classification: **scenario-backed static-template-to-placement dataflow during
a second stable M-map flight position**. This is a bounded active cache export,
not a complete world terrain mesh, global coordinate map, or coastline-pixel
mapping.

The sealed run037 enters its visible `M` map after frame 5,414. A three-frame
instruction trace over stable map frames 5,717--5,719 executes 37,681
instructions. It observes 110 immutable template copies at `$C1D488`; 100 of
those same workspace cells are subsequently read at `$C1DD36` and emitted by
the established placement builder.

All 100 emitted placements have a zero middle word. Their signed X/Z output
ranges are `-448..472` and `-768..256`, respectively. This is an independent
map-mode confirmation of the sampled flat placement plane. It remains a
runtime placement cache: the source words are transformed before output and
must not be reinterpreted as global vertex coordinates or an elevation table.

The joined [copy-to-placement inventory](workspace_template_copies_run037_m_map_template_to_placement_3f.md)
preserves every static source, mutable cell, descriptor, descriptor `+8`
control field, and emitted triple. Its [X/Z inspection plot](../plots/workspace_template_placements_xz_run037_m_map_template_to_placement_3f.svg)
visualizes the bounded flat outputs. The [target catalog](run037_m_map_template_target_catalog.md)
groups the 100 placements into 82 descriptor-field candidates; those fields
are generic-route projection inputs, not asserted raster submissions.

This run therefore independently establishes the chain:

```text
immutable segment-66/67 template record
  -> $C1D488/$C1D4BC mutable cell copy
  -> $C1DD36 placement-builder read
  -> descriptor-qualified X/0/Z placement
```

It does not associate one placement with a coastline pixel or an M-map packet,
and it cannot distinguish map-mode selection from physical-distance LOD.

Authority: sealed `captures/run037` and
`build/run037_m_map_template_to_placement_3f_trace/trace.jsonl`, which ends
at chipset frame 5,719.
