# Compass and selection timing interaction

The live cached HUD parent C30F78 calls C310AA; the frame-update owner C0EFD4
calls C12242 and resumes at C0F2EA. The fixed 300/700-cycle adapters each match
alone through demo frame 320, but their pair changes 64 selected pixels at 316.
The active registry now reaches source-timed boundaries in
`port/game/glue/glue_selection_compass_step.c`, including operand-dependent
multiply/divide/shift timing and the selection cleanup's view-key child.
Readable behavior remains in `control_records.c` and `player_input.c`.
This repairs temporary CPU integration; it is not a native frame cutover.

All 39 source instructions / 1,248 original-opcode cases match under DMA
contention. The paired 800-frame demo/carrier/crash probes match OFF drawing
and final RAM with Copper fade excluded. Every shadow/sandbox call completes
and matches: compass 33/15/74, selection 42/15/74. GNU and MSVC build both
runners, twelve CTests pass, GNU profiling is invisible in all three CPU modes,
and the MSVC/GNU paired demo's 800 RGB/index frames and RAM are identical.

Combined demo's first non-fade difference moves from 316 to **565** (683 pixels):
564 consecutive frames now match, versus 315 before these timing prerequisites.
Combined carrier/crash still differ at 374/213 (55,901/2,145 pixels). These are
bounded comparisons, not full replay or mission acceptance. The JSON stores
all comparisons, proofs and hashes.

Reusing existing 800-frame isolated C2D408 captures against the unchanged OFF
streams gives first differences at demo 584, carrier 446, crash 263. A fresh
220-frame crash subdivision/minimization finds the pair **C265E8,C2D408** first
changes indices at 213; neither member alone differs by 220. This interaction
is the next concrete target; do not replace the matrix charge with an average
or restore retired CPU child adapters as game behavior.

No full replay rerun. Last full raw CPU share **38.4011%** is cached; new full
suite delta unmeasured, accepted CPU share unset. Memory/chipset/boot **0%**,
deletion **0/4**, no phase complete.

Reproduce: `python tools/recomp/check_active_planes_step.py --group selection_compass --bus`
and `python scripts/probe_recomp_timing.py C310AA,C12242 ALL --frames 800`.
