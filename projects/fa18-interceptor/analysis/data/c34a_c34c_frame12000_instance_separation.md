# C34A/C34C: frame-12000 coordinate-frame non-comparability

The C34A and C34C face families use the same `$C351xx` transform pass. Their frame-12000 pre-clip coordinates have different numeric ranges:

| Family | X range | Y range | Z range | Mean Y |
| --- | ---: | ---: | ---: | ---: |
| C34A (20 unique faces) | -168 to 227 | -28 to 107 | 485 to 683 | 12.3 |
| C34C (5 unique faces) | -168 to 227 | -956 to -890 | 564 to 763 | -923.0 |

Their X extents coincide because the shared template is transformed through paired lanes, while C34C is produced through `$C45BC6` and C34A through `$C45BD8`. Those are different transformed coordinate spaces, so the approximately 935-unit Y-centroid difference is **not physical-instance separation evidence**. It cannot establish either a shared continuous mesh or separate world instances. The C34C silhouette remains a flight-object candidate; plane, missile, and reduced-detail readings remain unproven.

[The combined pre-clip sheet](../plots/c351_dual_lane_flight_object_composite.png) is a visual comparison only. It preserves face connectivity exactly as observed and does not place the two coordinate lanes in one shared world space.
