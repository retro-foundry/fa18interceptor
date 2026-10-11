# Cockpit message timing prerequisite

The live update sequence calls C11BFC from C0F12C and resumes at C0F132.
Its fixed 3,000-cycle adapter independently changed 64 selected pixels at
one-based demo frame 316. The registry now reaches the finite source timing
bridge in `port/game/glue/glue_message_step.c`; readable message behavior
remains `port/game/messages.c`. This removes the fixed charge and retains
original stack, register, memory and event boundaries. It is temporary CPU
integration scaffolding, not CPU independence.

The original-opcode oracle matches all 256 source instructions / 8,192 cases
with DMA bus contention. Isolated 800-frame OFF/ON drawing and final RAM match
on demo, carrier and crash recordings with Copper fade excluded. All shadow
and sandbox calls match: 42 / 15 / 74, with zero incomplete or mismatched calls.
GNU and MSVC build both runners; all twelve CTests pass; GNU profiling is
invisible in OFF/ON/interpreter modes.

Combined ON still first differs at frame 316 in a 600-frame probe. A fresh
320-frame subdivision isolates C30EAA as another independent reproducer.
The JSON stores bounded evidence and hashes. No full replay was rerun.
Last measured full-suite raw CPU share: **38.4011%** (cached); this batch's
full-suite delta is unmeasured. Accepted CPU share remains unset; memory,
chipset and boot cutover are **0%**, deletion gate **0/4**. No phase is complete.

Reproduce: `python tools/recomp/check_active_planes_step.py --group message_update --bus`
and `python scripts/probe_recomp_timing.py C11BFC ALL --frames 600`.
