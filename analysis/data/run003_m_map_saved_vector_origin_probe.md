# Run003 M-map saved-vector origin probe

Classification: **diagnostic display-state causality; direct vector capture**.

The raw `M` command's byte-exact transition at `$C1BFBC` copies `$C45664`
into `$C45C42`.  In the normal run003 M-map trace, every direct `$C2AF00`
packet entry has `A4=$C45C3E`; the packet routine therefore reads the word at
`4(A4)`, namely `$C45C42`, as its live projection-origin term.  The 26 direct
entries all contain the signed long `50331648` (`$03000000`) at that address.

This explains the prior negative `$C45C3E` mutation result: `$C1BFBC`
overwrites the whole live triple as M-map entry begins.  The pre-entry
`$C45664` feeder is instead causal in three isolated emulator instances,
all restored from the same untouched `build/run003_pre_m_2183/state.bin` and
given the same relative M-key stream:

| Pre-entry debugger write | Final video SHA-256 | Observed result |
| --- | --- | --- |
| none | `1828e0c0717c86ddb8ee3d4afa1ce15b2f0c9b028b8a0201cbae65cdc462f9f3` | normal map coastline |
| `$C45664 = $00000000` | `812dcdb481951e0fa1015f7fde8bccc7d98bfa1bf53029347c178334ff1b6bd8` | solid land base; no visible coastline |
| `$C45664 = $10000000` | `b6ea7a22bafe12746befb51d96f165af4b74373daa52274a7c2f795445ab0e66` | distinct coastline view |

The `$10000000` case was captured at `$C2FF48` before area filling.  Its
[SVG](../visuals/run003_m_map_saved_vector_a_projected_polygon_vectors.svg)
and [PNG](../visuals/run003_m_map_saved_vector_a_projected_polygon_vectors.png)
contain 40 direct projected polygons with the normal land/sea palette and no
labels.  The companion JSON records the mutation explicitly.

This is not an ordinary player position, a recovered map-pan control, or a
global-coordinate decode.  It proves only that `$C45664` is an M-entry feeder
for the live projection-origin term and can select a different map-rendered
coastline view.

Reproduction:

```powershell
python scripts/probe_m_map_request_flag.py --restore build/run003_pre_m_2183/state.bin `
  --playback build/run003_m_press_only.e9k --start-frame 2183 --frames 60 `
  --write-memory 0xC45664 0x10000000 4 --output build/run003_m_map_saved_vector_a

python scripts/collect_run003_m_map_projected_polygons.py `
  --restore build/run003_pre_m_2183/state.bin --playback build/run003_m_press_only.e9k `
  --write-memory 0xC45664 0x10000000 4 --frames 100 --max-polygons 96 `
  --output build/run003_m_map_saved_vector_a_polygons
```
