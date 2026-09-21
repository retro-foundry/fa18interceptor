# `$C3515E-$C351DC`: external-aircraft local-vertex candidate

Classification: **trace-proven static local-coordinate input set; topology not yet joined**.

During a bounded no-future-input step from the external-camera frame-7,500 update entry `$C0F090`, `$C1F100/$C1F21C` read 22 consecutive three-word values from `$C3515E` through `$C351DC`. They write their transformed results consecutively to `$C46228-$C462A6`. The source range is in byte-stable Hunk 41 and is separate from the mutable `$C48390` projection workspace.

The triples form a symmetric, low-height local-coordinate cloud. The selected-source trace now joins them to face topology: after the `$C3515E` transform, `$C1F6F8` enters `$C34A9A`, which repeatedly reaches `$C2469E`, `$C24CFE`, and `$C2FF48`. The frame-7,500 collector records eleven resulting polygons with `A5=$C34A9A`. This is a trace-proven model candidate in the external-camera scenario; the remaining uncertainty is the user-facing identity, not whether the vertex transform and polygon stream are connected.

[Orthographic static-vertex plot](../plots/external_aircraft_c3515e_static_vertices.svg) shows all 22 source points in X-Y, X-Z, and Y-Z. It intentionally draws points only: source order is not proof of an edge or polygon, and the renderer's real topology must be recovered from its face records rather than guessed from nearest neighbours.

Reproduce from the saved transform capture:

```powershell
python scripts\plot_static_model_vertices.py
python scripts\plot_runtime_geometry.py
```

[Renderer-topology orthographic plot](../plots/external_aircraft_c3515e_c34a9a_polygons_orthographic.svg) draws the eleven traced polygon outlines. Unlike the vertex plot, its connectivity comes directly from finalized renderer submissions.

[PNG model sheet](../plots/external_aircraft_c3515e_c34a9a_model_sheet.png) is the same 45-edge topology rendered for quick visual inspection; colours distinguish submitted polygons only and do not encode materials.

[Combined polygon-and-line sheet](../plots/external_aircraft_c3515e_c34a9a_combined_model_sheet.png) additionally overlays 36 `$C212B0` line segments observed with the same `$C34A9A` controller family. White strokes are line submissions; coloured outlines remain final polygons.

The final-submission sheet omits faces rejected by orientation or clipping. `$C2005C` face preparation captures those records before the gates; 21 distinct face records are observed for each `$C34A9A` and `$C34A9C` family. [The complete pre-cull sheet](../plots/external_aircraft_c34a9_preclip_complete_face_sheet.png) draws all 42 observed face records and restores the forward fuselage/nose outline without inferred geometry.
