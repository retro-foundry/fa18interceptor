# `$C32EF6-$C32F5B`: message-event layout preparation

Classification: **static message-event/layout dataflow**.

`source_amiga/observed/prepare_message_event_layout.asm` is byte exact for
`$C32EF6-$C32F5B`.  It copies `$C457DB` into the word at `$C45748`, initializes
`$C457DE` to one, and normally joins `$C32F54`.  When incoming `D4` equals
`$41` and `$C457F6` is positive, it decrements that cursor, increments
`$C457F5`, clears one byte through pointer `$C1AB74 + $1E + D5`, clears two
adjacent bytes starting at `$C457EB + D5`, restores `(A1,A2,A4)` from
`$C456FE`, subtracts four from `A1`, and joins `$C32F5C`.

## Run060 native checkpoint observation

`build/run060_frame09285_c32ef6_short_trace/trace.jsonl` is a bounded,
no-input direct-core instruction trace from the fresh native GUI-frame-9,285
checkpoint. The breakpoint at `$C32EF6` hits on the second following frame.
The first six instructions prove this path's concrete values:

- record byte `$C457DB` is read as 5 and copied to `$C45748`;
- `$C457DE` is written as 1;
- incoming `D4` has low byte `$4D`, so the `$41` branch is not taken;
- the path enters `$C32F54`, restores the layout triplet, and reaches the
  existing `$C32FCE` glyph-compositor entry.

No recorded input occurs between run060 GUI frames 9,285 and 9,290. This
proves a real no-input message-layout pass inside the visible-text onset
interval. It still does not identify the original producer of `D4`, the
meaning of record subtype 5, the qualifying condition, or whether this pass
is the first instruction that causes a visible glyph pixel. The trace is not
used as a visual oracle during active character drawing.
