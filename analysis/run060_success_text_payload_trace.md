# Run060 success-text payload-to-compositor trace

Authority: the sealed `captures/run060` native checkpoint at GUI frame 9,285
and the bounded no-input trace
`build/run060_frame09285_c32ef6_short_trace/trace.jsonl`. No recorded input
occurs through GUI frame 9,290; the trace is used for control/dataflow, not as
a pixel-equivalence oracle during active character drawing.

## Direct selected payload evidence

The breakpoint at `$C32EF6` hits on the second frame after native checkpoint
9,285. At entry its register state includes:

```text
A2 = $C3FC46
D4 low byte = $4D
```

The literal success record begins at `$C3FC3C`; its header bytes are followed
by five spaces and `LANDING SUCCESSFUL` beginning at `$C3FC41`. Thus `$C3FC46`
is the `N` in that literal word. The trace follows:

```text
$C32EF6  copy subtype / initialize message auxiliary state
  -> $C32F54  restore selected layout
  -> $C32FCE  read byte from (A2)
  -> $C33002  normalize glyph character
  -> $C33058  submit four glyph mask-update lanes
```

This proves that the active run060 onset path has selected the static
`LANDING SUCCESSFUL` payload and feeds bytes from its record into the glyph
compositor. It supersedes the earlier run029-only negative probe as evidence
for this particular payload route.

## Limits

The trace begins after the producer that established `A2=$C3FC46`; it does not
identify the selector, qualification result test, landing/collision logic,
status writer, pilot-log persistence, or the separate `YOU ARE NOW QUALIFIED
FOR MISSIONS` record. It also does not establish the exact frame at which a
particular glyph becomes visible.
