# `$C35932` to `$C355D8`: bridge face-family candidate

Classification: **trace-proven static vertex-transform and renderer-face path; partial, scenario-labelled Golden Gate bridge candidate**.

At the Golden Gate checkpoint, `build/run031_frame12000_c35932_following_trace_v2/trace.jsonl` starts the alternate matrix path at `$C1F4AC` with `A1=$C35932`. Its vertex loop at `$C1F524` feeds the transformed workspace. The same bounded trace subsequently selects the static face/control context `$C355D8` at the renderer walker and reaches clipping (`$C2469E`), perspective projection (`$C24CFE`), and final polygon consumption (`$C2FF48`).

The independent 48-frame Golden Gate polygon collector records 15 finalized `$C355D8` polygons containing 53 renderer-observed edges. [The tight-fit orthographic sheet](../plots/golden_gate_c35932_c355d8_model_sheet.png) draws exactly those submitted outlines in X-Y, X-Z, and Y-Z; it adds neither inferred links nor missing faces.

The checkpoint's scenario label and the plotted structure make this a bridge **face-family** candidate, not a complete landmark reconstruction. The checkpoint contains additional independently transformed source blocks and renderer contexts. It is therefore incorrect to treat this sheet as comparable in completeness to the `$C3515E -> $C34A9A` aircraft sheet; it proves one source-to-face path only.

## Reused bridge-face evidence

The frame-12,000 matrix capture records `$C35932`, `$C361E4`, `$C3B9B2`, `$C3B0CE`, `$C3AD0E`, and `$C3A96E` entering `$C1F4AC` with the same matrix pointer (`$C45BD8`) and the same reusable destination (`$C48390`). Bounded follow-on traces from each source reach final `$C2FF48` submission with `A5=$C355D8`.

This is evidence for a composed/reused bridge face family: each transform pass replaces the shared workspace before the `$C355D8` renderer stream consumes it. It is not evidence that all of those vertex blocks are accumulated into one workspace simultaneously. `$C3B720` uses the same matrix/destination pattern but reaches `$C3B6B0`, making it a separate traced component that must not yet be merged into the `$C355D8` sheet.

## Assembled transform-batch view

The bounded-batch collector captures renderer output between consecutive `$C1F4AC` entries. Four `$C35932` batches and four `$C3B720` batches provide an evidence-bounded composite: `$C35932`'s batches include the `$C355D8/$C355D6` family; `$C3B720`'s batches include `$C3B6B0`. [The assembled sheet](../plots/golden_gate_assembled_transform_batches_model_sheet.png) contains 25 observed filled-polygon outlines (86 polygon edges) plus 36 observed `$C212B0` line segments.

This is the current broadest bridge draw. It is still explicitly batch-scoped: it excludes contexts with no source-to-batch evidence and cannot claim unseen/back-facing faces.

## Bridge-family pre-clip view

The `$C2469E` pre-clip capture contains 14 prepared `$C355D8` faces across the replay. Deduplicating on observed record context yields 12 `$C355D8/$C355D6` face records, including entries not reaching `$C2FF48`; [the pre-clip face-and-line sheet](../plots/golden_gate_c355_preclip_complete_combined_sheet.png) combines these 42 polygon edges with 39 `$C212B0` line segments. This is a renderer-family completeness improvement, not evidence for any additional landmark component.

Regenerate the sheet with:

```powershell
python scripts/render_external_aircraft_model_sheet.py --input build/run031_frame12000_golden_gate_polygon_submissions_48f/polygon_submissions.json --context '$C355D8' --fit --title 'Golden Gate bridge-model candidate: $C35932 -> $C355D8' --output analysis/plots/golden_gate_c35932_c355d8_model_sheet.png
```
