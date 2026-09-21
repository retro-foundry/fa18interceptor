# Cockpit numeric-output oracles

## Scope

This note separates values visibly rendered by the original cockpit HUD from
unproven live-state names.  Screenshots are valid output oracles; they do not
alone establish the source RAM representation or writer.

## run029 anchors

Independently replayed run029 screenshots provide these clearly readable
speed/altitude pairs:

| Frame | Visible speed | Visible altitude |
| ---: | ---: | ---: |
| 1,000 | `175 KTS` | `140 FT` |
| 5,000 | `598 KTS` | `8185 FT` |
| 14,298 | `254 KTS` | `120 FT` |

The deterministic states are retained in
`build/run029_numeric_frame_<frame>/slow.bin` and their screenshots in the
matching directories.

## Rejected direct-storage hypothesis

A sampled run029 snapshot scan rejected direct matching values for the visible
speed and altitude readings as unsigned 16- or 32-bit values in either byte
order across the exported Slow-RAM image.  This does not prove that the figures have no
RAM-backed state: their input may be scaled, packed, held within a record, or
produced by a Chip-RAM/render record.

Therefore KTS, FT, G-load, fuel, Mach, and weapon/ammunition readouts are
currently **screen-observed only**.  They must not be assigned to a `$C4xxxx`
field just because that field changes in the same interval.

## Font-renderer bridge

Run029 renderer tracing identifies a general packed-nibble font path at
`$C32740-$C328A5`: it converts a packed value from `$C45B22` to character
bytes, indexes glyph offsets at `$C3D790`, and composites glyph bytes through
the `$C32858` strided-long loop. This is the first direct glyph-source to
framebuffer bridge, but it has not yet been shown to render KTS or FT. See
`analysis/routines/c32740_packed_nibble_font_renderer.md`.

## Next evidence

Capture a bounded renderer trace that connects a changed numeric-glyph
rectangle to its formatter/record writer, then trace that input back to the
state producer.  That producer/state/consumer chain is required before naming
the corresponding cockpit live variable.
