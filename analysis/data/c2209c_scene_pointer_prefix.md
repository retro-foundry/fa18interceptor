# `$C2209C-$C220A7`: scene-pointer prefix

Classification: **inline data inside original CODE segment 16**.

Segment 16 payload offsets `$54,$58,$5C` are three consecutive 32-bit Hunk
relocation fields targeting segment 41. Their original addends are `$8,$8,$0`.
At runtime they resolve to two pointers to `$C34A58` followed by one pointer to
`$C34A50`.

There are no intervening 68000 opcode words between the three relocation
longwords. This contiguous linker-pointer run is positive data evidence. The
range is deliberately limited to those 12 bytes; the surrounding segment-16
bytes retain their original CODE classification.
