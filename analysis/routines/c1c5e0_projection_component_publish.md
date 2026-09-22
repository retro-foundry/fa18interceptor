# `$C1C5E0` projection-component publication

Classification: **byte-exact transformed-component publication**.

The common tail shifts a three-longword transformed coordinate tuple by eight
bits, stores the three low words at `$C45A72-$C45A76`, and stores the complete
shifted `D1` component at `$C45A78`.  `$C1C636` is the sole write in this
tail to `$C45A78`.

In the direct arm, the tuple is assembled by adding signed/masked fields from
`A2+$14`, `+$18`, and `+$1C` to `D0-D2`, negating all three values, and
publishing them through `$C45A62`.  The alternate arm calls `$C1C2C8`, retains
the current `$C45C3E` triple separately, then returns to the same publication
path.

The M-map depth/detail path reads this exact `$C45A78` component at `$C2AAD2`.
Thus its geometry variant selector is driven by one transformed camera-space
component.  This disproves interpreting it as a stored terrain-elevation
field, but does not establish a global game axis or prove that this component
is physical aircraft-to-terrain distance.

Authority: byte-exact reconstruction in
[`publish_projection_depth_component.asm`](../../source_amiga/observed/publish_projection_depth_component.asm),
plus live run035 M-map state samples where `$C1C636` updates `$C45A78` before
the `$C2AB0C` metric producer and `$C2AF40` stream selector.
