# `$C33258` postflight transform table

Classification: **confirmed data within an executed CODE hunk**. `$C33258-$C332B3`
is a 92-byte, 46-word table immediately before executable entry `$C332B4`; it
must not be disassembled as instructions.

The words, in ascending address order, are:

```
0000 7000 0000 B000 0002 1000 0000 C000
0002 0000 0002 4000 0000 C000 0002 0000
0002 4000 0002 A000 0002 E000 0004 2000
0004 6000 0000 E000 0002 2000 0000 C000
0002 2000 0002 6000 0002 A000 0002 E000
0000 6000 0000 A000 0000 E000
```

`$C33F8A` passes the table base in `A1`, with count/configuration `$1A/2/2`,
to `$C32AA6`; `$C33F70` passes `$C33264`, 12 bytes into the same table, with
`$A/2/2`. `$C32B00` consumes pairs from `A1`, while its `A2` byte selects a
glyph through `$C3D790` and `$C330FE` composites that glyph. This proves the
table supplies font-render coordinate pairs, but not its coordinate convention,
visual field, or caller-visible effect.

The enclosing segment is correctly still classified as executed code because
its later `$C332B4` entry runs; current coverage accounting is segment-grained,
so this inline data does not alter the global data denominator.
