# Four-plane image blit timing prerequisite

The HUD image stream reaches C30EAA at C309DA, returning to C309E0. Its fixed
3,000-cycle adapter independently changed 64 selected pixels at demo frame 316.
The live registry now uses `port/game/glue/glue_hud_image_step.c`, including
original bound-span, blitter-wait and plane-submit boundaries. The existing
C30F46 timing bridge supplies the shared tail. Readable behavior remains in
`port/game/hud_bars.c`. This is temporary CPU integration scaffolding.

All 62 source instructions / 1,984 original-opcode oracle cases match under DMA
contention. The three 800-frame isolated recordings match OFF drawing and final
RAM with Copper fade excluded. Shadow matches: 32/12/67, with 1/3/7 incomplete;
sandbox matches: 33/15/75, none incomplete. All have zero mismatches. Both
GNU/MSVC runners build, twelve CTests pass and GNU profiling remains invisible
in OFF/ON/interpreter modes. Message-plus-image probes match through demo 800.

Combined ON still differs at frame 316. Fresh 320-frame subdivisions over all
603 registry entries isolate C25482's timer charge as another reproducer. No
full replay rerun: last full raw CPU share **38.4011%** is cached, new full-suite
delta unmeasured, accepted CPU share unset; memory/chipset/boot **0%**, gate
**0/4**. No phase is complete. JSON stores bounded evidence and output hashes.

Reproduce: `python tools/recomp/check_active_planes_step.py --group image_blit --bus`
and `python scripts/probe_recomp_timing.py C30EAA C11BFC,C30EAA ALL --frames 800`.
