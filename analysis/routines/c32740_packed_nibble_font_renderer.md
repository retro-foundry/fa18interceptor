# `$C32740-$C328A5`: packed-nibble font-rendering path

Classification: **runtime-backed graphics dataflow** for the observed run029
path; the caller's field semantics remain unassigned.

## Observed path

In the chipset-frame-993 trace, `$C31B56` establishes a two-character render
request and branches to `$C32740` with `D0=1`, `A0=$C457FC`,
`A1=$C31A1C`, `A2=$C457FA`, `A4=$1CDE`, and `A5=$1E`.

`$C32740` reads the packed value at `$C45B22`. Its loop takes a low nibble,
adds `$30`, conditionally has a non-decimal adjustment path, stores the byte
backward through `A0`, shifts the packed value right four bits, and repeats
under `DBRA`. The observed invocation emits two character bytes into the
`$C457FA` scratch area, with a leading-zero suppression check before drawing.

For each retained byte, the path at `$C327A6` reads a coordinate pair through
`A1`, takes a character through `A2`, bounds the adjusted coordinate against
`$28`, and derives a destination from the renderer pointer block `$C456B6`.
It subtracts `$20` from the character and uses the doubled result to index the
word-offset table at `$C3D790`. That resolves `A0` to the glyph byte stream.
The routine then calls `$C32806`, whose observed body delegates to the
byte-stream/strided-long merge loop at `$C32858`.

The first observed glyph stream starts at `$C3D847`; its destination starts
at `$0186F6` and advances by `$28` for six scanlines. The second starts at
`$C3D842` and writes the adjacent destination sequence. These addresses and
the merge loop establish a concrete font-glyph-to-framebuffer bridge.

## Scope

This path does not yet prove that `$C45B22` is speed, altitude, or another
cockpit readout. It does prove that the game has a packed-nibble formatter
feeding a glyph table and CPU compositor. To name a cockpit value, replay a
readable numeric transition and establish that the transition invokes this
path with a changed `$C45B22` value or changed glyph selection.
