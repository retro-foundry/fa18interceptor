# Map packet inline/alternate geometry comparison

Each row compares the first directly entered transform batch for the same immutable packet header. Pair counts are source-coordinate consumption counts, not face counts or a global terrain complexity measure. The alternate route is dynamically proven, but these captures do not isolate physical distance from other map transition/control state, so the comparison establishes a live geometry variant rather than distance-only LOD.

The [inline/alternate coordinate sheet](../plots/run003_run035_map_packet_variants.png) shows these exact batches at a common local scale per packet. It is a raw source-pair view, not globally placed terrain or reconstructed faces.

| Packet header | Inline pairs | Alternate pairs | Delta | Inline pair range | Alternate pair range |
| --- | ---: | ---: | ---: | --- | --- |
| `$C43BAC` | 7 | 7 | +0 | `$C43BB2`--`$C43BCA` | `$C43C72`--`$C43C8A` |
| `$C43CBE` | 13 | 7 | -6 | `$C43CC4`--`$C43CF4` | `$C43CFC`--`$C43D14` |
| `$C43ECA` | 9 | 8 | -1 | `$C43ED0`--`$C43EF0` | `$C43F3C`--`$C43F58` |
| `$C43F5E` | 6 | 3 | -3 | `$C43F64`--`$C43F78` | `$C43FB0`--`$C43FB8` |
