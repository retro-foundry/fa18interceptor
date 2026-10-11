# Run031 frame-12,000 finalized polygon submissions

Classification: **runtime-backed polygon-workspace inventory**. This is a bridge-frame collection boundary, not a complete bridge-model extraction.

`scripts/collect_polygon_submissions.py` restored the sealed frame-12,000 checkpoint and recorded 27 `$C2FF48` entries over a bounded 12-frame no-input window. Each record captures the finalized `$C4B990` triples immediately before `$C24CFE`'s projected-pair consumer submits them. Raw records are retained in [the machine-readable inventory](../../build/run031_frame12000_polygon_submissions_12f/polygon_submissions.json).

## Golden Gate stream subset

Only submissions 0 and 1 retain `A5=$C355D8`, the active-stream continuation observed directly after the verified Golden Gate `$C355A0/$C35720` dispatcher. They produce two closed polygons:

| Submission | Vertices | Finalized `$C4B990` triples |
| ---: | ---: | --- |
| 0 | 4 | `(-799,793,2444)`, `(-866,786,2421)`, `(-1474,882,3455)`, `(-1407,889,3478)` |
| 1 | 3 | `(-465,1365,1684)`, `(-447,304,1753)`, `(-497,316,1788)` |

The corresponding seven closed-loop edges are plotted in [the `$C355D8` orthographic subset](../plots/golden_gate_c355d8_polygons_orthographic.svg). This is a verified bridge-frame renderer subset, not sufficient evidence to claim it is the entire bridge.

## Excluded submissions

After the first two submissions, `A5` switches to `$C34A9A`, `$C4BFBE`, `$C4BFD0`, `$000005`, `$00000B`, `$C3B6B0`, and `$FFFFFDF4`. These are retained in the raw inventory but deliberately excluded from the Golden Gate subset: a checkpoint can execute scene, cockpit, and renderer work after the original stream advances.

The source and output workspaces remain mutable. Identifying bridge source data still requires tracing the upstream writer/control record for the `$C355D8` entries, or matching a controlled source mutation to the ordinary replay image.
