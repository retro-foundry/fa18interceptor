# `$C2B93E`: three-record marker line-list entry

Classification: **scenario-backed renderer dataflow**.

The byte-exact entry in
[`submit_three_record_marker_line_list.asm`](../../source_amiga/observed/submit_three_record_marker_line_list.asm)
has a nonnegative-status path that sets `$C45954` to three, selects the signed
offset list at `$C2B91E`, and joins the common clipped-line iterator at
`$C2BAA8`.  The map trace then emits the three `$C4C598` line segments that
form the black flight-object marker candidate.

The entry is a line-list selector, not proof of the marker's game ownership.
Its `A1=$C45BEA` producer context remains the separate evidence for calling
the marker a flight-object candidate.
