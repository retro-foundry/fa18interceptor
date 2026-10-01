# `$C31226` postflight renderer-variant dispatcher

Classification: **partially runtime-observed dataflow**. The run024
frame-23000 continuation executes `$C31226-$C31262`, establishing the bounds
setup and the dispatch branch. Its selected variant body is not covered by
that trace.

`source_amiga/observed/dispatch_postflight_renderer_variants.asm` reproduces
`$C31226-$C31289` (100 bytes). It initializes inputs for `$C310E2`, then uses
`$C45837` and masked bits 1-3 of `$C458DA` to select `$C3129A`, `$C31312`, or
`$C31392`. The positive activity path writes `-1` to `$C4E71C/$C4E744` before
executing the first two entries.

The selector's user-visible meaning remains unproven; this records only the
demonstrated dispatch and state-write contract.

## Native timing evidence

Updated 2026-10-01. A current sealed-demo source trace through frame 430 covers
the full `$C31224-$C318F4` function range. Six `$C31226` calls complete at
source frames 311, 341, 358, 401, 411 and 420. Their elapsed costs are 36,120,
336, 344, 12,616, 11,178 and 9,180 CPU cycles. The first call takes an
interrupt at `$C31230` and completes in the next frame. The three long calls
execute 347-348 instructions through the tuple/shared-tail route and service
chipset deadlines repeatedly between `$C3129A` and `$C3170E`.

The former registered C entry charged 15,000 cycles only after its complete
readable call. In a 500-frame comparison, C31226 alone and the complete
registered set both first differed from source at one-based frame 416 with the
same 361 pixels. This followed the frame-401 and frame-411 long calls, whose
source costs differ from the fixed charge by 2,384 and 3,822 cycles. The fixed
charge also could not represent the interrupt-bearing or short-exit calls.

The registered child entries C3129A and C31312 were independently enabled for
500-frame replays; each and both together remained RGB444-identical to source.
A valid source-timed replacement therefore had to cover the direct `$C31392`
route and the complete shared tail through `$C318F4`.

## Resumable C boundary

`port/game/glue/glue_postflight_step.c` now covers every one of the 440 source
instructions from the shared `$C31224` return through `$C318F4`. C31226,
C3129A and C31312 share that bridge. External line and pixel routines remain
ordinary child calls, while the source PC and stack retain continuation state
across child dispatch, chipset service, interrupts and frame ends. The three
registry entries no longer use fixed cycle charges.

The postflight instruction-oracle group matches registers, SR, PC, cycles and
RAM for 14,080 fixtures. With the subsequent C305AA polygon-edge bridge, the
combined bridge oracle matches 1,163 instructions and 37,216 fixtures. Fresh
isolated OFF/ON recordings match every RGB444 frame
across demo01, qual_carrier_success and qual_fail_crashes. The complete
three-recording gate matched 721,752 shadow calls and 1,169,610 sandbox calls
with zero mismatches, sealed RAM intact and identical poison frames. This
resolves the C31226 timing blocker. C305AA has since received its own complete
source-timed bridge and exact isolated live proof.
