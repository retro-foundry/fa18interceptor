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

| static source | frames 404--426 output | frames 5253--5255 output |
| --- | --- | --- |
| `$C4264D` | `(76, 0, 428)` | `(11008, 0, 6400)` |
| `$C4265F` | `(-64, 0, 448)` | `(8864, 0, 9216)` |
| `$C42665` | `(-112, 0, 432)` | `(13120, 0, 8192)` |
| `$C42683` | `(-148, 0, 416)` | `(2048, 0, 10240)` |
| `$C426A1` | `(-64, 0, 320)` | `(11024, 0, 5376)` |
| `$C426A7` | `(-68, 0, 348)` | `(12288, 0, 2048)` |
| `$C426C5` | `(-64, 0, 192)` | `(12288, 0, -4096)` |

Therefore the source entries in segments 66--67 are reusable templates, not
fixed global terrain-position records. The placement builder combines each
template with mutable context before writing its runtime tuple. This is
consistent with a tiled or paged world representation, but does not by itself
prove a grid, identify map-cell coordinates, or establish LOD.

For visual checks, the two diagnostics are
[frames 404--426 X/Z plot](../plots/workspace_template_placements_xz_404_426.svg)
and
[frames 5253--5255 X/Z plot](../plots/workspace_template_placements_xz_5253_5255.svg).
