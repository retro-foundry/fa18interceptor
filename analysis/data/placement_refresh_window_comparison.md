# Placement refresh-window comparison

Classification: **two scenario-backed static-template-to-placement windows**.
This comparison tests whether a copied static entry is itself a fixed global
terrain coordinate. It does not establish the coordinate convention of the
runtime placement cache or identify the authoritative world-cell selector.

## Authority

- Early window: `build/run033_placement_bulk_404_thirtyframe_trace/trace.jsonl`,
  with the joined inventory in
  [workspace_template_copies_404_426](workspace_template_copies_404_426.md).
- Later window: `build/run033_frame05250_placement_thirtyframe_trace/trace.jsonl`,
  with the joined inventory in
  [workspace_template_copies_5253_5255](workspace_template_copies_5253_5255.md).
- In both cases `$C1D488` reads the static entry and `$C1DD36` later reads the
  same workspace cell before the builder emits the three placement words.

## Measured windows

| window | static copies | later builder reads | unique static sources | emitted X range | emitted Z range | zero middle words |
| --- | ---: | ---: | ---: | --- | --- | --- |
| frames 404--426 | 143 | 101 | 106 | -704 to 88 | -352 to 512 | 101 / 101 |
| frames 5253--5255 | 73 | 21 | 73 | 2048 to 15872 | -4096 to 10544 | 21 / 21 |

The later values cover a much larger numeric region, while preserving zero in
the sampled middle placement word. This reinforces a flat placement-layer
observation for these two replay windows, without proving a universal height
axis or an entire-map extent.

## Same static entries, different placements

Seven static sources are joined to emitted placements in both windows. Each
has a different output tuple:

| static source | frames 404--426 descriptor / output | frames 5253--5255 descriptor / output |
| --- | --- | --- |
| `$C4264D` | `$C22408` / `(76, 0, 428)` | `$C22700` / `(11008, 0, 6400)` |
| `$C4265F` | `$C22818` / `(-64, 0, 448)` | `$C22700` / `(8864, 0, 9216)` |
| `$C42665` | `$C22318` / `(-112, 0, 432)` | `$C22764` / `(13120, 0, 8192)` |
| `$C42683` | `$C22458` / `(-148, 0, 416)` | `$C22A34` / `(2048, 0, 10240)` |
| `$C426A1` | `$C22368` / `(-64, 0, 320)` | `$C22778` / `(11024, 0, 5376)` |
| `$C426A7` | `$C22AAC` / `(-68, 0, 348)` | `$C22A84` / `(12288, 0, 2048)` |
| `$C426C5` | `$C22390` / `(-64, 0, 192)` | `$C22A98` / `(12288, 0, -4096)` |

Therefore the source entries in segments 66--67 are reusable templates, not
fixed global terrain-position records. The placement builder combines each
template with mutable context before writing its runtime tuple. The seven
matched sources also select different descriptor records between these widely
separated windows. This is a real context-dependent descriptor substitution,
but not LOD evidence: page selection, flight/control state, and camera
distance all differ together. A valid LOD test must hold the source page and
item context fixed while varying only a measured distance, then connect the
descriptor substitution to a changed static model/face family.

## Descriptor-target comparison

The descriptor records are not aliases with only different record addresses.
In the control snapshot each descriptor repeats a target longword at offsets
`+4`, `+8`, and `+12`; the substitutions above change that repeated target:

| static template | early descriptor target | later descriptor target |
| --- | --- | --- |
| `$C4264D` | `$C22408 -> $C35568` | `$C22700 -> $C3B4F8` |
| `$C4265F` | `$C22818 -> $C447C6` | `$C22700 -> $C3B4F8` |
| `$C42665` | `$C22318 -> $C445A2` | `$C22764 -> $C44500` |
| `$C42683` | `$C22458 -> $C36E6A` | `$C22A34 -> $C45294` |
| `$C426A1` | `$C22368 -> $C4477C` | `$C22778 -> $C4455A` |
| `$C426A7` | `$C22AAC -> $C3B960` | `$C22A84 -> $C454F4` |
| `$C426C5` | `$C22390 -> $C4483C` | `$C22A98 -> $C4558C` |

This proves context-dependent selection of different descriptor targets. Some
targets are already known immutable scene-family candidates (for example
`$C35568`); the semantic role and mutability of every target in this table has
not yet been established. Consequently the result strengthens the need for a
controlled same-page distance experiment, but it does not identify any target
pair as an LOD pair.

The repeated target is also functionally significant. `$C1CB74-$C1CCB6`
consumes descriptor `+4` as an `A0` input, writes descriptor `+8` to
`$C45A36`, and writes descriptor `+12` to `$C45A3A`. The compared descriptors
repeat the same target at all three offsets, so every substitution in the
table changes the `$C45A36` static control-stream pointer consumed by
`$C1F6F8` on the projection path. This is direct dataflow evidence for
context-dependent renderer control-stream selection; it remains insufficient
to call the selection LOD because the controlling context is not isolated to
distance.

### Target stability check

The 24-byte windows beginning at all 13 targets in the table above are
byte-identical across five independent run033 snapshots: the frame-404
placement trace, frame-5250 placement trace, and checkpoints/traces at frames
5500, 6000, and 6250. Thus none of these sampled targets behaves like a
changing terrain workspace over that interval. This promotes them to
**byte-stable descriptor-target candidates** for this run033 scope, not to
decoded meshes or LOD levels: relocation ownership, full record format, and
the target-to-renderer path must still be established individually.

For visual checks, the two diagnostics are
[frames 404--426 X/Z plot](../plots/workspace_template_placements_xz_404_426.svg)
and
[frames 5253--5255 X/Z plot](../plots/workspace_template_placements_xz_5253_5255.svg).
