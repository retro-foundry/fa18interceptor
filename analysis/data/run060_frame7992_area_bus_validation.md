# run060 frame 7992 area bus validation

The DMA capture was widened to include the complete active page range
`$012BC0-$01A8BF`. For each of the four `$C30F5A` area submissions, the
captured A/B source rows, C/D destination addresses, `$FCE` minterm, and
modulo tuple were replayed by the native semantic executor.

The Engine9000 DMA records expose the destination writes interleaved with
source reads. The exposed words from the first submission agree with
`~A & (B | C)` and the 37-byte source / 41-byte destination strides. The
remaining differences in a full frame comparison are later renderer writes to
the same page ranges, as shown by their later DMA indices. A full frame 7992
gate therefore needs the complete ordered renderer stream, rather than a
single area-job comparison.

This validates the area operation itself and keeps the native model at the
semantic page boundary. It does not yet claim frame 7992 pixel parity.
