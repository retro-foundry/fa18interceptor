# `$C2AD00` map/control record detail gate

Classification: **byte-exact conditional detail-selection machinery; primitive mapping pending**.

The control walker reads a signed mode byte from `(A0)+`.  `0xFF` terminates
the list; a negative non-terminator byte sets the frame flag at `-$44(A6)` and
uses its low seven bits as the mode.  It clears the three per-record fields at
`-$24(A6)`, `-$22(A6)`, and `-$20(A6)` before any conditional selection.

When `-$3E(A6)` is zero, it proceeds directly to `$C2AD80`.  When that word is
non-zero and `$C457AD` is clear, it applies the following bands to the signed
longword at `-$28(A6)`:

| Record mode | metric condition | fields set before `$C2AD80` |
| --- | --- | --- |
| `4` | `<= $400` | `-$20(A6) = 2`; byte `$C4589C` also copied to `-$24(A6)` |
| `4` | `$400 < metric <= $C80` | `-$20(A6) = 1`; byte `$C4589C` also copied to `-$24(A6)` |
| `4` | `> $C80` | neither word field is set; byte `$C4589C` is still copied |
| not `4` | `<= $C80` | `-$20(A6) = 1`, `-$22(A6) = 1` |
| not `4` | `> $C80` | neither word field is set |

This is source-level evidence for a coarse, three-state detail gate.  The
metric's physical meaning has not been established as camera distance, and
the downstream consumers have not yet been tied to line versus polygon
emission.  It therefore does **not** prove LOD in the user's proposed sense.

The bounded M-map traces exercise both alternatives: after `$C2ADBE`, the
zero branch reaches `$C2ADC0` and the non-zero branch reaches `$C2ADC6`.
Both destinations occur in run003 and run035 M-map traces.  That proves the
mode split is live in this renderer family, but not which values of the
metric were present at each capture point.

Authority: byte-exact reconstruction in
[`map_control_record_detail_gate.asm`](../../source_amiga/observed/map_control_record_detail_gate.asm),
decoded from the verified runtime image; trace PCs `$C2AD00`, `$C2AD2A`, and
`$C2AD80` in the sealed run003/run035 M-map traces.
