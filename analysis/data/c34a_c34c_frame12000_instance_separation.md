# C34A/C34C: frame-12000 shared-template instance separation

The C34A and C34C face families use the same `$C351xx` transform pass, but the frame-12000 pre-clip coordinates do not occupy one local object space:

| Family | X range | Y range | Z range | Mean Y |
| --- | ---: | ---: | ---: | ---: |
| C34A (20 unique faces) | -168 to 227 | -28 to 107 | 485 to 683 | 12.3 |
| C34C (5 unique faces) | -168 to 227 | -956 to -890 | 564 to 763 | -923.0 |

Their X extents coincide because the shared template is transformed through paired lanes, but the approximately 935-unit Y-centroid separation is too large to call them one continuous captured mesh. The correct reading is **template reuse in spatially separate renderer instances**. The C34C silhouette can still be plane- or missile-like; this evidence rejects treating it as the missing nose of the C34A instance.

[The combined pre-clip sheet](../plots/c351_dual_lane_flight_object_composite.png) is a visual comparison only. It preserves face connectivity exactly as observed and does not add a link between the separated instances.
