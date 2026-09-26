# Engine9000 line blitter pipeline

## Evidence

Source: `tools/engine9000-src/ami9000/sources/src/blitter.c`,
`blitter_line_proc_status` through `actually_do_blit`.

The line path runs one vertical iteration per `vblitsize`. Each iteration:

1. Sets the visible pixel flag from `BLTSING` and `blitonedot`.
2. Updates A's error accumulator using `BLTSIGN`.
3. Reads B and C when the horizontal size is greater than one and evaluates
   the first minterm pipeline stage.
4. Applies the X and Y C pointer steps, including the `BLTCH` gate.
5. Updates A's shift from the overflow latch and recomputes the sign bit.
6. Rotates B for the next line.
7. Writes D only when the visible pixel flag is set, then copies C to D.
8. Runs the final minterm pipeline stage and repeats.

For frame 7993 jobs 6-8, the capture contains 11 C/D pairs per submission.
The pairs occur at the recorded odd display addresses and job boundaries,
including raster wrap between vpos 157 and 158 for jobs 7 and 8. The register
`BLTSIZE=0x02c2` alone does not explain the observed event count, so the
submission start/end records and the engine's effective vertical counter must
be included before this is represented as a native semantic packet.

## Port consequence

`FA18LineBlitJob` currently models the proved frame 7992 path, where every
captured C source word maps to one executor iteration. Frame 7993 needs an
explicit pipeline contract for the captured submission boundary before its
line jobs are added. Do not reduce the job to an assumed 11-row convenience
operation.
