# `$C3515E-$C35234`: external-aircraft local-vertex candidate

Classification: **trace-proven static local-coordinate input set with `$C34C` face topology**.

During a bounded no-future-input step from the external-camera frame-7,500 update entry `$C0F090`, `$C1F100/$C1F21C` read 22 consecutive three-word values from `$C3515E` through `$C351DC`. They write their transformed results consecutively to `$C46228-$C462A6`. The `$C34C` faces additionally use 14 workspace slots derived by `$C0D384-$C0D521` from that transformed block. The adjacent `$C351E2-$C35234` triples have a related reference pattern but are not the observed source of the derived tail. The static 22-triple run is in byte-stable Hunk 41 and is separate from the mutable `$C48390` projection workspace.

The triples form a symmetric, low-height local-coordinate cloud. A bounded
frame-12,000 trace now supplies the source-to-face join: `$C1F100` enters with
`A1=$C3515E` and `A0=$C46228`; its 22 transforms end at `$C462AC`. The same
no-future-input trace reaches `$C2035A` with `A4=$C46228`, then submits the
five `$C34C06-$C34C48` records via `$C203C4 -> $C2469E`. This is a
trace-proven model candidate; the remaining uncertainty is its user-facing
identity, not whether the static vertices and those five faces are connected.

That path directly traces only the first 22 of 36 vertex slots used by the five
faces. The remaining 14 slots in `$C462AC-$C46301` first initialize together
at replay frame 1966, but their direct producer is now located: the
[`$C0D384` derived-vertex tail](../routines/c0d384_derived_vertex_tail.md)
calculates them from earlier `$C46228` workspace triples. `$C351E2-$C35234`
has a numerically close static continuation, but it is not the observed input
of that producer. This is therefore a 22-static-vertex plus 14-derived-vertex
renderer component, not a complete static-model export.

[Orthographic static-vertex plot](../plots/external_aircraft_c3515e_static_vertices.svg) shows all 36 contiguous source-order points in X-Y, X-Z, and Y-Z. Indices 0-21 are transform-traced; 22-35 are the untraced continuation candidate. It intentionally draws points only: source order is not proof of an edge or polygon, and the renderer's real topology must be recovered from its face records rather than guessed from nearest neighbours.

Reproduce from the saved transform capture:

```powershell
python scripts\plot_static_model_vertices.py
python scripts\plot_runtime_geometry.py
```

The `$C34A9A/$C34A9C` renderer family remains a useful aircraft-like
orthographic plot, but its source linkage must be kept separate: the
frame-12,000 trace uses `$C48390` for that family, not the `$C46228` output
proved above. Its 42-face sheet is catalogued separately.
