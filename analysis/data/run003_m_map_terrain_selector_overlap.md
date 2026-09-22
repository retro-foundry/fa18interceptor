# `M` map appearance and terrain-selector overlap

Classification: **scenario-backed concurrent pipeline activity**.

The sealed `run003` `M` replay first presents its stable coastline map at
continuation frame 13.  The deterministic, instruction-stepped transition
trace reaches that same hardware frame and executes the established terrain
template selector `$C1D3F4`, followed by the static-stream entry `$C1D442`.
This is the first direct observation that the visible map-transition window
and the world terrain paging pipeline overlap in one replay.

The bounded frame-13 window has 14 selector calls. Six calls reach these
immutable static template streams:

| selector group / row key | workspace band | selected stream |
| --- | --- | --- |
| `$000F / $000F` | `$C48990` | `$C42ADA` |
| `$000F / $000E` | `$C48F90` | `$C42956` |
| `$0010 / $0010` | `$C4A190` | `$C42BD4` |
| `$0010 / $000F` | `$C4A790` | `$C42706` |
| `$0011 / $0010` | `$C4B390` | `$C42646` |
| `$0011 / $000F` | `$C4D190` | `$C42B66` |

The rows are mechanically exported in the companion
[selector inventory](static_template_selector_groups_run003_m_appearance.md).
The selector's static-to-workspace copy reads 71 exact six-byte template
records from those streams; their source addresses and bytes are retained in
the companion [template-record inventory](active_terrain_template_stream_records_run003_m_appearance.md).
Three streams (`$C42ADA`, `$C42BD4`, `$C42706`) are also independently
observed in a live flight page-refresh window.  This supports their being
active world-page inputs, rather than an `M`-only bitmap asset.

An extended, video-hash-matched map-mode trace follows the copied records to
103 descriptor-qualified X/0/Z placement outputs. See the
[template-placement handoff](run003_m_map_template_placement_handoff.md).

This does **not** identify the coastline pixels' producer or prove that any
of the six streams rasterizes the `M` display.  The transition trace also
executes ordinary update/render work, and its display-plane writes are still
only connected to generic mutable renderer scratch and blitter helpers.
Consequently, the safe conclusion is shared timing/pipeline activity, not a
map-data extraction or a static-coastline source claim.

Authority: sealed `captures/run003`, normal-frame visibility timing recorded
in [the M visual probe](run003_m_map_visual_probe.md), and the deterministic
`build/run003_m_map_appearance_trace/trace.jsonl` with its paired slow-RAM
snapshot.  Reproduce the selector export:

```text
python scripts/inventory_static_template_selector_groups.py \
  --trace-directory build/run003_m_map_appearance_trace \
  --output-suffix _run003_m_appearance
```
