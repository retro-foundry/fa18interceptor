# Run003 M-map renderer vectors

The [SVG](../visuals/run003_m_map_renderer_vectors.svg) is the first map view
drawn from the renderer's captured vector-emission inputs rather than from the
completed map bitplanes.  It contains all 19 `$C2FA7E` entries in the sealed
13-frame transition trace as SVG lines in the renderer's 320 by 180 screen
coordinate space.  The dark-blue field uses the map water palette colour
`#003366` only as context; it is not a claimed reconstruction of polygon fill.

The red **Golden Gate** callout at `(97, 59.5)` is evidence-backed: two emitted
vectors use `$C3559A` and `$C355D2`, the source/control contexts independently
correlated with the user-identified Golden Gate bridge lines.  It is an anchor
in this run003 M-map view, not an absolute world coordinate.

The map land fill is deliberately absent.  `$C304F4` supplies 44 pending-page
line-mode blitter jobs in this transition, but their source-to-polygon and
final-colour contract has not yet been decoded.  Turning the already-finished
bitplanes into SVG land shapes would answer a different question (bitmap
vectorisation), so this artifact does not do that.

Authority: `build/run003_m_map_appearance_trace/trace.jsonl`; generated with:

```powershell
python scripts/render_run003_m_map_renderer_vectors.py
```
