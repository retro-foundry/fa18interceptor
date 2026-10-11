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

## Native final-mission return contract - 2026-10-08

The ordinary-input final-mission objective exposed a missing native child
return: C230B0 retains the incoming selection in D0.w, and C09E30/C0A370 only
replace its low byte. The generic completion C0A3A6 therefore publishes FF04
for selection FFFF and quota four. The native callback previously seeded only
the mode byte and published 0004. The dispatcher now consumes the selection
return and `native/records.c` carries it into the generic mode. Original whole
body 8,938 and the landing/save/menu/wrap comparisons agree after the correction;
see `../native_final_mission_sequence_milestone.md`. No predicates, masks or
original countdowns were changed.
