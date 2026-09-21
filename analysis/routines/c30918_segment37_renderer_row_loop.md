# `$C30918` segment-37 renderer row loop

Classification: **static-only structural/dataflow**. No available replay has
executed this routine.

`source_amiga/observed/run_segment37_renderer_row_loop.asm` reproduces
`$C30918-$C309A1` (138 bytes). It derives a bounded count from `$C458F6`,
conditionally refreshes `$C459A4`, initializes a two-word coordinate pair
from `$C45988/$C458D8`, and repeats two calls to the existing `$C2F60A`
renderer wrapper for each of 22 `DBRA` iterations. It updates `$C45954` with
one of two observed values before each pair.

This records register and memory dataflow only. The meaning of the source,
cache, mode, and destination fields remains unproven.
