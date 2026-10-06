# Signed-byte timer timing prerequisite

The live C25312 timer owner calls C25482 and resumes at C253DC/C253E6/C253F0.
Its fixed 30-cycle adapter independently changed 64 selected pixels at demo
frame 316. The active registry now reaches source-timed signed-byte test,
conditional decrement and return boundaries in `glue_timer_tick_step.c`.
Readable timer behavior remains in `port/game/stages.c`. This repairs timing;
CPU registers, guest memory and chipset service are still dependencies.

All four source instructions / 128 independent opcode cases match under DMA
contention. Three isolated 800-frame drawing/RAM comparisons match OFF, with
Copper fade excluded. Shadow and sandbox match every 27/15/36 call, with zero
mismatches or incomplete calls. Both GNU/MSVC runners build; twelve CTests and
GNU profiling invisibility in all three CPU modes pass. The message/image/timer
group matches demo through frame 800.

Combined ON still differs at frame 316. After these three repairs, neither
half of the complete 603-entry registry reproduces the mismatch alone;
minimize the interacting selected paths next. No full replay was rerun.
Cached last full raw CPU share: **38.4011%**, new full-suite delta unmeasured;
accepted CPU share unset; memory/chipset/boot **0%**, deletion **0/4**. No phase
is complete. The JSON stores bounded evidence and hashes.

Reproduce: `python tools/recomp/check_active_planes_step.py --group timer_tick --bus`
and `python scripts/probe_recomp_timing.py C25482 C11BFC,C30EAA,C25482 ALL --frames 800`.
