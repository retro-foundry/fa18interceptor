# `$C0D7E0-$C0D871`: display-record pair selector

Authority: the return-bounded attract cockpit capture which returns from
`$C2E758` to `$C0D7E0`, plus canonical slow-RAM bytes. This prefix is directly
upstream of the already reconstructed `$C0DA70` success/rejection returns and
the record-write leaves.

The prefix reads four workspace words at `$C4E854 + {0,2,4,6}`. Its nested
zero/nonzero tests either select one of six pairs from offsets `{8,10,12,14}`
into `D0/D1`, or branch to one of the rejection/alternate setup paths. The
selected pair and later state fields have no assigned display or gameplay
meaning; this is a verified dataflow and branch-selection contract only.

The first selected-pair branch, `$C0D872-$C0D8BD`, is reconstructed in
`source_amiga/observed/emit_first_display_record_pair.asm`. It writes record
count four at `$C4B390`, then dispatches an ordered sequence of the existing
record-write leaves. `$C45785` and bit 1 of `$C458CA` select the shorter or
extended sequence; both continue to the known `$C0DA70` success return.

The second branch, `$C0D8BE-$C0D90B`, is reconstructed in
`source_amiga/observed/emit_second_display_record_pair.asm`. It initializes
record count five and selects an ordered set of the same leaf calls from bit 1
of `$C458CA`; the extended form initializes a second count-three record at an
eight-byte offset before its final leaf.

The third branch, `$C0D90C-$C0D959`, is reconstructed in
`source_amiga/observed/emit_third_display_record_pair.asm`. It has the same
count-five setup and flag test, but a different ordered leaf sequence; only its
extended form appends the second count-three record and final leaf.

The fourth branch, `$C0D95A-$C0D9A7`, is reconstructed in
`source_amiga/observed/emit_fourth_display_record_pair.asm`. It follows the
same flag split, calling the fixed leaves in another order and appending a
count-three, eight-byte-offset record only on its extended path.

The fifth branch, `$C0D9A8-$C0D9E9`, is reconstructed in
`source_amiga/observed/emit_threshold_display_record_pair.asm`. It initializes
count four and compares `$C45A92` with `$3840`; the lower path adds two leaf
calls through `$C4B39A`, while the greater-or-equal path uses the shorter
sequence. The threshold's domain is unassigned.
