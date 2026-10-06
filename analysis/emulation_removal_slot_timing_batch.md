# Flagged-slot scan timing

The live frame-update owner calls C265E8 from C0F0E0 and resumes at C0F0E6.
Its fixed 3,500-cycle scan charge interacts with C2D408. The registry now uses
original scan, magnitude-child and return boundaries in `glue_slot_range_step.c`;
readable behavior remains `flagged_slot_in_range()` in `fixed_math.c`.

All 65 original instructions / 2,080 DMA cases pass. Three isolated 800-frame
drawing/RAM probes match OFF with Copper fade excluded; shadow/sandbox report
zero mismatches. Both builds, twelve CTests and GNU profiling checks pass.
Combined carrier moves 374 -> **446**, crashes 213 -> **263**; demo remains 565.
The next failure coincides with the isolated matrix path's timing debt in the
carrier/crash runs. Full replays were not rerun. This is a timing prerequisite,
not removal of a CPU dependency: cached full raw CPU **38.4011%**, new delta
unmeasured, memory/chipset/boot **0%**, deletion gate **0/4**.

Reproduce: `python tools/recomp/check_active_planes_step.py --group slot_range --bus`.
The JSON contains the bounded proofs, comparisons and hashes.
