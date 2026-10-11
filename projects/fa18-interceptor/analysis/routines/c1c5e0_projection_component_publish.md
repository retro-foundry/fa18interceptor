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

The bounded attract-mode oracle now proves one unmodified handoff: `$C1C636`
stores `D1=-125` at trace frame 4, then `$C2AAD2` consumes `D0=-125` in frame
5, after 19 intervening reads and no intervening `$C45A78` write. See
[`attract_projection_depth_metric_oracle.md`](../data/attract_projection_depth_metric_oracle.md).
The publisher's `A2` is `$C46184` in that invocation, and its direct arm reads
the mutable control-record-bank fields `+$14/+$18/+$1C`; this excludes treating
the publisher input as a direct immutable terrain-template triple, but does
not identify the selected record's physical role.

A second sealed packet in the qualification oracle reaches the same direct arm
at run060 replay frame 8,246, with `A2=$C46184`, and returns to `$C0F036` in
the `$C0EFD4` parent update sequence. It measures the exact root triple and
the published tuple, linking the live root record to this renderer-transform
stage without assigning player/camera ownership. See
[`run060_root_projection_packet.md`](../data/run060_root_projection_packet.md).

Authority: byte-exact reconstruction in
[`publish_projection_depth_component.asm`](../../source_amiga/observed/publish_projection_depth_component.asm),
plus live run035 M-map state samples where `$C1C636` updates `$C45A78` before
the `$C2AB0C` metric producer and `$C2AF40` stream selector.
