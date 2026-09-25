# Run060 success-text payload-to-compositor trace

Authority: sealed `captures/run060` native checkpoints at GUI frames 9,200 and
9,285, plus bounded no-input traces
`build/run060_frame09200_c32d24_trace/trace.jsonl` and
`build/run060_frame09285_c32ef6_short_trace/trace.jsonl`. No recorded input
occurs from GUI frame 9,023 through 9,290; the traces are used for
control/dataflow, not as pixel-equivalence oracles during active character
drawing.

## Selector-to-payload chain

At native GUI frame 9,285, `$C32D24` receives `D0=$004A` (selector 74), reads
the `$0F2E` relative word at `$C3ED9C`, and resolves descriptor `$C3FC38`.
It publishes payload cursor `A2=$C3FC3C` at `$C32E0E`; the trace continues
straight to `$C32FCE`.  This is the static payload beginning with nine spaces
then `LANDING SUCCESSFUL`.  `$C32CEE` obtains this head word from the live
sequence `$C4574A` with cursor `$C457C6=$00`, clears `$C457F6`, and falls
through into the selector.  The sequence writer remains unknown.

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

This proves that the active run060 onset path uses standard selector 74 to
select the static `LANDING SUCCESSFUL` payload and feeds bytes from its record
into the glyph compositor. It supersedes the earlier run029-only negative
probe as evidence for this particular payload route.

## Limits

The traces begin after the producer that supplied selector 74; they do not
identify the qualification result test, landing/collision logic, status writer,
pilot-log persistence, or the separate `YOU ARE NOW QUALIFIED FOR MISSIONS`
record. They also do not establish the exact frame at which a particular glyph
becomes visible.
