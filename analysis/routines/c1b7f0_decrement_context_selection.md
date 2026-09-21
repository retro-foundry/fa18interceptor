# `$C1B7F0` context selection decrement

Classification: **structural with byte-exact source**. The route handles a
zero/nonzero context-selection byte at `$C45785`. In one state it subtracts
`$02000000` from `$C45C42` with a `$01000000` lower clamp; otherwise it
decrements the byte and wraps its nonpositive result to `$FF`. It latches
`$FF` at observed request bytes before joining the queue.

The exact raw-key and gameplay association remains pending a bounded trace.
