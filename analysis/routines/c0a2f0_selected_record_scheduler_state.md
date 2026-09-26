# `$C0A2F0`: selected-record scheduler-state preparation

Classification: **static instruction dataflow cross-checked by native state
checkpoints**.

The byte-exact dispatcher entries `$C0A2F0-$C0A3C5` are reconstructed in
`source_amiga/observed/prepare_selected_record_scheduler_state.asm`.

The byte-exact gate is statically decoded as selecting `$C46184`, requiring
the selector clear, bit 6 at `+1`, `(word(A1+$02) & $C080) == $C080`, zero
word `A1+$6E`, and a phase other than three. Its `$C0A324` route stores:

```text
$C0A324  $C45798 := $FF
$C0A32C  D0 := 0
$C0A3A6  $C458C0 := D0
$C0A3AC  $C4582A := 3
$C0A3B4  $C4582C := 4
$C0A3BC  $C458AD := 1
```

Fresh native run060 checkpoints show the required before/after state: frame
9,205 has `$C45798=0`, `+$6E=$0007`, and phase/countdown/latch `$00/$FF/$00`;
frame 9,210 has `+$6E=0` and exactly the stored `$FF/$03/$04/$01` values. The
direct stepped traces retained in `build/run060_frame09200_c45798_writer_trace/`
and `build/run060_frame09200_scheduler_field_writer_trace/` agree with these
stores as local instruction evidence, but direct-core stepping is not used to
date the native transition. This closes the state-level record-gate-to-
scheduler dataflow that later reaches `$C11078` and `$C110A4`. It does not
identify the physical meaning of the record flags, `$6E`, or the qualification
predicate.
