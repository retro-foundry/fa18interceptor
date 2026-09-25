# `$C1D0B6`: descriptor-indexed component accumulation

Classification: **behavioral dataflow helper**. The normal `$C1CCBC` descriptor
route enters this helper with descriptor word `D7` and component values in
`D2/D4`.

The high byte of `D7` selects a `$C46184` record at 512-byte stride; its low
nibble is a variable arithmetic-shift count. The helper masks record longs at
`+$14/+1C` to 20 bits, shifts them by that count, shifts `D2/D4` left eight,
and adds the resulting components. It separately shifts record `+$18`, adds
`$C45A66` before its second shift, writes the result to `$C45B3C`, and sets
`$C458BB=1`.

The caller then publishes the three resulting components at `$C45B30` and
their `>>8` words at `$C45B2A`. This proves descriptor-indexed component
preparation for the renderer/projection pipeline, without assigning a world
object or coordinate-system name.

Evidence: byte-exact `accumulate_alternate_record_components.asm`, its
`$C1CECC` caller continuation, and the complete normal run060 `$C1CCBC`
trace at frame 304.
