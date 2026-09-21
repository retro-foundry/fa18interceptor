# `$C362A2` to `$C36298`: later bridge-model candidate

Classification: **trace-proven alternate-transform to finalized-polygon path; model identity still user-facing**.

At the frame-14,500 bridge checkpoint, `build/run031_frame14500_matrix_transform_inputs.json` records `$C1F4AC` with `A1=$C362A2`, followed by its `$C1F524` vertex-loop iteration at `$C362A8`. The bounded follow-on trace beginning at that source (`build/run031_frame14500_c362a2_following_trace/trace.jsonl`) reaches clipping (`$C2469E`), perspective projection (`$C24CFE`), and polygon submission (`$C2FF48`) with `A5=$C36298`.

`build/run031_frame14500_bridge_polygon_submissions_64f/polygon_submissions.json` captures 16 finalized polygons for `$C36298`, with 55 renderer-observed edges. [The tight-fit X-Y, X-Z, and Y-Z sheet](../plots/later_bridge_c36298_model_sheet.png) connects only vertices submitted together by the renderer.

The same checkpoint's `$C2469E` pre-clip capture has six distinct observed `$C36298` record contexts (19 edges). [The pre-clip sheet](../plots/later_bridge_c36298_preclip_face_sheet.png) is the topology-completeness reference; the final-submission sheet remains the more legible repeated-instance silhouette.

The visual structure is bridge-like and occurs at the bridge checkpoint, but no landmark name is promoted here. The evidence proves the `$C362A2` source to `$C36298` renderer-family path, not a complete scene/model boundary.
