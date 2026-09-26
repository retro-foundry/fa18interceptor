# run060 frame 7992 `$C30668` preparation trace

A breakpoint at `$C30668` was first replayed without arming it at the target
frame. That capture stopped 36 times in earlier replay frames and was
mistakenly associated with frame 7992. A corrected probe arms the breakpoint
only while executing frame 7992 and records zero hits. This file is retained
as a correction record; it is not a frame-7992 packet source.

## Corrected result

The 36 ordered jobs in frame 7992 are therefore not attributed to this leaf.
The DMA inventory remains authoritative for their final register writes and
completed hardware submissions.

## Port implication

The native renderer still needs a frame-aligned producer trace for the 32
later jobs. No screen endpoints or source assets are promoted from this
discarded capture. Frame 7992 remains held until all 36 outputs are reproduced
in submission order and the final chunky page matches the emulator.
