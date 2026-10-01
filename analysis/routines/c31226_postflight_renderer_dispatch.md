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

The registered C entry currently charges 15,000 cycles only after its complete
readable call. In a current 500-frame comparison, C31226 alone and the complete
registered set both first differ from source at one-based frame 416 with the
same 361 pixels. This follows the frame-401 and frame-411 long calls, whose
source costs differ from the fixed charge by 2,384 and 3,822 cycles. The fixed
charge also cannot represent the interrupt-bearing or short-exit calls.

The registered child entries C3129A and C31312 were independently enabled for
500-frame replays; each and both together remain RGB444-identical to source.
The timing blocker is the outer dispatch call. A valid source-timed replacement
must cover the direct `$C31392` route and the complete shared tail through
`$C318F4`. Letting a prefix bridge fall into the generated tail is not a
complete C-port boundary and does not satisfy the sandbox contract.
