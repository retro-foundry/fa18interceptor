# `$C0A2F0`: selected-record scheduler-state preparation

Classification: **sealed replay instruction-level dataflow**.

The byte-exact dispatcher entries `$C0A2F0-$C0A3C5` are reconstructed in
`source_amiga/observed/prepare_selected_record_scheduler_state.asm`.

In the aligned global-frame-9,200 run060 checkpoint replay, the parent update
at restored frame 9 (sampled global frame 9,210) reaches `$C0A2F0`.  It sees
the selector clear, chooses `$C46184` as `A1`, observes bit 6 set at `+1`,
observes `(word(A1+$02) & $C080) == $C080`, and observes word `A1+$6E` zero.
With phase not already three, it takes the `$C0A324` route:

```text
$C0A324  $C45798 := $FF
$C0A32C  D0 := 0
$C0A3A6  $C458C0 := D0
$C0A3AC  $C4582A := 3
$C0A3B4  $C4582C := 4
$C0A3BC  $C458AD := 1
```

The direct stepped writes are retained in
`build/run060_frame09200_c45798_writer_trace/` and
`build/run060_frame09200_scheduler_field_writer_trace/`.  This closes the
record-gate-to-scheduler dataflow that later reaches `$C11078` and `$C110A4`.
It does not identify the physical meaning of the record flags, `$6E`, or the
qualification predicate.
