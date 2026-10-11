# Run031 external-camera polygon candidates

Classification: **cross-frame external-view candidate family**. These packets are not yet named player-aircraft model data.

Two independent external-camera anchors converge on the `$C3925x` control-stream family:

| Frame / capture | User-visible fact | Observed control evidence |
| --- | --- | --- |
| 7,500 external-aircraft checkpoint | Player aircraft is visible in external view | Completed-frame `$C45A36=$C3925A`; bounded polygon collection reaches `$C3925C` three times and `$C3925E` three times. |
| 12,600 external Golden Gate pass | External camera shows the player aircraft beneath the bridge | Bounded `$C2FF48` collection reaches `$C3925C` three times and `$C3925E` four times. |

The repeated family across external views makes its polygon submissions strong **player-aircraft/external-view candidates**. It is deliberately not proof of aircraft ownership: camera-specific overlay, another nearby dynamic object, or shared external rendering work remain possible.

## Captured polygon subsets

- `$C3925C`: three closed polygons, 17 edges — [orthographic plot](../plots/external_view_c3925c_polygons_orthographic.svg).
- `$C3925E`: four closed polygons, 16 edges — [orthographic plot](../plots/external_view_c3925e_polygons_orthographic.svg).

The independent frame-7,500 external-aircraft checkpoint reaches the same contexts: [its `$C3925C` plot](../plots/external_aircraft_c3925c_frame7500_orthographic.svg) and [its `$C3925E` plot](../plots/external_aircraft_c3925e_frame7500_orthographic.svg). The differing coordinates and edge counts are expected for mutable, camera-relative renderer work; the repeated static control context is the evidence, not coordinate equality.

The complete external-pass collector output is [the raw inventory](../../build/run031_frame12600_external_polygon_submissions_v2/polygon_submissions.json). Its `$C35584` records are excluded from this family because they are in the Golden Gate `$C355xx` scene-control family instead.
