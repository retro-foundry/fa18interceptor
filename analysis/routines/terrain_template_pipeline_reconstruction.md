# Terrain template pipeline reconstruction

Classification: **source-backed static-template-to-flat-placement pipeline**.
This is the current bounded answer to where the game assembles terrain/scene
content. It is not a complete global terrain mesh, a physical-world coordinate
decode, or a flight-distance LOD result.

## Reconstructed control path

```text
active control record + matrix state
  `$C29042-$C291D3`
  -> derived selector origin `$C45C3E/$C45C42/$C45C46`
  -> `$C1D330-$C1D3F3` band walker
       immutable segment-65 control bytes + live bounded terms
  -> `$C1D3F4-$C1D51D` static group/row selector
       signed-relative group -> bit gate -> binary row-key search -> stream
  -> immutable segment-66/67 header + two-word template records
  -> `$C1D442` mutable workspace cell expansion
  -> `$C1DC1C-$C1E0B0` placement builder
  -> descriptor-qualified X/0/Z placement cache records
```

The byte-exact reconstructed parts are:

| Range | Source | Proven responsibility |
| --- | --- | --- |
| `$C29042-$C291D3` | [`publish_terrain_selector_origin.asm`](../../source_amiga/observed/publish_terrain_selector_origin.asm) | derives and stores the live three-component selector origin; its first/third components causally change selected template pages in controlled probes |
| `$C1D330-$C1D3F3` | [`walk_static_template_bands.asm`](../../source_amiga/observed/walk_static_template_bands.asm) | translates immutable control bytes, bounds the two live terms, calls the static selector, and advances workspace bands by `$600` |
| `$C1D3F4-$C1D51D` | [`select_static_template_stream.asm`](../../source_amiga/observed/select_static_template_stream.asm) | signed-relative group lookup, bitset gate, binary row-key lookup, static stream selection, and static-to-workspace item copy |
| `$C1D520-$C1D5D7` | [`append_template_workspace_matches.asm`](../../source_amiga/observed/append_template_workspace_matches.asm) | conditionally appends auxiliary mutable marker/index records from two bounded record regions |
| `$C1D722-$C1D763` | [`mark_workspace_cell_starts.asm`](../../source_amiga/observed/mark_workspace_cell_starts.asm) | sixteen generic strided marker stores; its caller uses `$FFFF` at `$60` stride before template repopulation |

## Established data boundaries

- The static selector layout is decoded as a 32×32 bounded lattice for the
  sampled update packet. It selects 4--18 immutable streams per cell; see the
  [selector lattice](../data/terrain_template_selector_lattice.md).
- The selected immutable source records are a header plus two words. They are
  not raw global coordinates: the downstream builder combines them with live
  state before emitting placements; see the [template payload export](../data/terrain_lattice_stream_templates.md).
- The sampled emitted placement cache has a zero middle word (X/0/Z). This is
  positive flat-placement evidence, not proof that every reusable local 3D
  component has zero local height.
- The visible M-map renderer separately reuses a compact local 3D component
  with nonzero local middle coordinate. That local geometry does not change
  the flat placement contract.

## Explicit limits

The selector-origin producer's common adjustment/publish tail
`$C29548-$C295D0` and component-delta mode selector `$C291D4-$C29225` are
reconstructed; the downstream dispatch/threshold-policy range
`$C29226-$C29547` is not. Neither the reconstructed smoothing tail nor
the static threshold branches must be called a terrain LOD scheme without a
controlled same-instance distance trace. The 32×32
selector lattice wraps in the observed bounded inputs, but no evidence maps
those bins to absolute global coordinates or total physical flight-map size.
