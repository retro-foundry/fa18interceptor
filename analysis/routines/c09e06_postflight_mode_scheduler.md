# `$C09E06`: postflight mode scheduler dispatcher

Classification: **sealed replay caller relation plus byte-exact dispatch**.

`source_amiga/observed/initialize_c09e06_record_update_gate.asm` and
`source_amiga/observed/dispatch_c09e3c_mode_scheduler.asm` reconstruct
`$C09E06-$C09E97`. After clearing three adjacent bytes and calling `$C230B0`,
it requires bit 6 of `$C458CD` and a zero `$C45790` byte before selecting a
subroutine by `$C458A6`.

The aligned run060 global-frame-9,210 trace returns from the observed
`$C0A2F0` scheduler gate through `$C09E82` and `$C09E94`. The static mode-9
arm at `$C09E78` is the direct `BSR.W $C0A2F0`; run060's live mode is nine in
the same scheduler interval. This joins the mode-9 context to the scheduler
writer without treating that menu/context byte as the success predicate.

The final return reaches `$C230AC` in the retained caller probe, which then
returns to `$C1C6BC` in the parent-update chain. The probe is
`build/run060_frame09200_c0a2f0_caller_probe/`.
