# `$C327A0-$C32805`: packed-font glyph compositor

Classification: **scenario-backed glyph-stream-to-buffer dataflow**.

`source_amiga/observed/compose_packed_font_glyphs.asm` is byte exact for
`$C327A0-$C32805`. For each `D0 + 1` glyphs it consumes an `(x,y)` word pair
from `A1` and one character byte from `A2`, bounds the adjusted coordinate to
the observed `$28` limit, subtracts ASCII space, and uses the doubled code to
select a word offset from `$C3D790`.

It combines that glyph-stream pointer, geometry base, and selected renderer
pointer into `D1/D4/D2`, rejects odd destinations through `$C06C02` with code
`$46`, and otherwise calls `$C32806`. The latter has independent runtime
evidence for merging the glyph byte stream into a 40-byte-stride buffer.

This is the byte-exact implementation beneath the existing run029
glyph-to-framebuffer observation. The postflight entries at `$C3278C/$C32794`
join this same loop, but no postflight screen/payload identity is inferred.
