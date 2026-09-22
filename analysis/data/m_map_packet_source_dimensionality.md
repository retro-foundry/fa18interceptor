# M-map packet source dimensionality

Classification: **byte-exact two-dimensional immutable source format**.

The static map-packet loop at `$C2AF9C/$C2AF9E` consumes exactly two signed
words per source item, advancing `A3` by four bytes: the first word is added
to `D2`, the second to `D4`.  No third immutable coordinate word is read for
that item.

The loop emits three mutable workspace words, but the third is computed at
`$C2AFCA-$C2AFD8` from those same two values through the live transform matrix
at `$C45BD8` and a live offset in `A4`; it is not a stored packet-height word.
The first two emitted workspace words are likewise transformed from the same
two source inputs.  Therefore the segment-68 M-map packet payload is planar
2-D source data, with renderer-space depth introduced during transformation.

This directly supports a flat **M-map source layer**.  It does not prove that
every flight-world object, placement record, or separate terrain system lacks
elevation; those use different source paths and coordinate conventions.

The corresponding live rendered map page is available in the
[run003 M-map display](../visuals/run003_m_map_display.png).  That image is a
separate renderer-output observation; it is not used to infer source-pair
ownership beyond the traced packet path.

Authority: the byte-exact
[`transform_map_packet_pairs.asm`](../../source_amiga/observed/transform_map_packet_pairs.asm)
slice, verified against runtime `$C2AF92-$C2AFF7`, and the exact pair
inventories in [segment-68 trace coverage](m_map_segment68_trace_coverage.md).
