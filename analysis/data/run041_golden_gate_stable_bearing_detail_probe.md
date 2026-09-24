# Run041 Golden Gate stable-bearing detail probe

Classification: **scenario-backed, landmark-associated visual-detail
selection; not distance-threshold LOD proof**.

Run041 is a sealed hands-off cockpit approach. The exact red landmark scan is
limited to world viewport `x=40..679, y=18..159`, excluding cockpit/HUD red
pixels. Its centre remains close to the viewport centre while the landmark
grows:

| Frame | Red pixels | Bounds, inclusive | Horizontal centre |
| ---: | ---: | --- | ---: |
| 4,500 | 30 | `356,108`--`387,108` | 371.5 |
| 4,750 | 68 | `322,108`--`391,108` | 356.5 |
| 5,000 | 72 | `316,108`--`395,108` | 355.5 |
| 5,250 | 78 | `310,108`--`401,109` | 355.5 |
| 5,500 | 100 | `300,109`--`399,109` | 349.5 |
| 5,750 | 166 | `286,106`--`403,110` | 344.5 |
| 6,000 | 296 | `272,106`--`439,112` | 355.5 |
| 6,250 | 674 | `148,106`--`489,120` | 318.5 |

Two independent 12-frame no-input traces bracket that growth. At frame 5,000
the `$C1F4AC` alternate matrix transform has 18 entries from ten immutable
sources: `$C35BAA`, `$C35BC2`, `$C363EC`, `$C36954`, `$C3A9A8`, `$C3AD0E`,
`$C3AF62`, `$C3B0CE`, `$C3B720`, and `$C3B9B2`. At frame 6,250 it has six
entries from only three distinct sources: `$C35A98`, `$C35ADE`, and `$C3B588`.
The sets are disjoint in these bounded samples.

The terrain-origin policy endpoint is exactly identical across those windows:
`$C457B6=6`, `$C458AE=0`, and
`$C45C3E/$C45C42/$C45C46=(264187402,2928,286094979)`. Thus this transform
source switch does not coincide with a change in the reconstructed
terrain-origin selector state.

## Landmark screen-space association

The replay-preserved `$C2FA7E` collection armed at frame 6,250 observes a
screen-space edge from `$C4BFA6` at `(123,90)->(177,91)`, with the normal
four-plane mask `$0F` and return `$C302BA`. The corresponding screenshot's
red Golden Gate bounds convert from screen `x=148..489, y=106..120` to the
renderer bitmap rectangle `x=54..224, y=90..104`; the complete edge is inside
that rectangle.

The source-bounded collector independently records `$C4BFA6` as a three-point
polygon workspace produced while processing `$C35A98`. This joins one member
of the close-window `$C1F4AC` source set to the measured Golden Gate raster.
It is an edge emitted from a polygon workspace, not proof that the complete
filled polygon is visible or a reconstruction of an immutable bridge mesh.
The complete immutable four-triple `$C35A98` and eight-triple `$C35ADE`
direct-input runs are exported separately in the
[close-landmark component report](c35a98_c35ade_close_landmark_component.md).

At the small-window checkpoint, `$C35BAA` and `$C35BC2` independently submit
the already red-raster-correlated `$C3559A/$C355D2` Golden Gate line contexts.
Together with the disjoint source sets, this confirms a landmark-associated
change from the distant line family to a close-window primitive family as the
same, nearly centred landmark grows.

## Result and limit

The stable bearing, large measured landmark growth, unchanged selector-origin
state, disjoint transform-input sets, and screen-space association provide
confirmed **landmark-associated visual-detail selection**. It does not prove
that a physical-distance threshold selects the family: camera state and
visibility may still participate, and no source-level selector threshold has
been tied to a measured distance. It is therefore not a confirmed LOD level,
nor an extracted bridge or terrain mesh.

## Source-bounded primitive comparison

No-input source-bounded collectors from the two saved trace states confirm
that the changing inputs feed different renderer primitive contexts rather
than merely being unused transforms:

| View checkpoint / source | Polygons | Lines | Observed static line context |
| --- | ---: | ---: | --- |
| frame 5,000 `$C35BAA` | 0 | 1 | `$C3559A` |
| frame 5,000 `$C35BC2` | 10 | 2 | `$C355D2` (one other mutable/zero context) |
| frame 5,000 `$C3B720` | 2 | 4 | mutable/zero contexts only |
| frame 6,250 `$C35A98` | 7 | 2 | `$C355C2` |
| frame 6,250 `$C35ADE` | 0 | 1 | `$C35590` |
| frame 6,250 `$C3B588` | 2 | 0 | -- |

This proves a source-bounded primitive-family change at the stable-bearing
landmark growth. The `$C4BFA6` polygon context is now projected onto the exact
red viewport pixels as documented above. The `$C35590/$C355C2` contexts remain
unassigned individually, so they are not claimed as additional Golden Gate
segments.

Authority: sealed `captures/run041`; ignored artifacts
`build/run041_bridge_keyframes/`, `build/run041_bridge_red.json`,
`build/run041_frame05000_12f_trace/`, and
`build/run041_frame06250_12f_trace_retry/`,
`build/run041_{small,large}_view_matrix_instances/`, and
`build/run041_red_{5000,6250}_line_entries/`.
