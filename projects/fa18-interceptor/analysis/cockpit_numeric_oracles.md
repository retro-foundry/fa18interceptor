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

Run029 renderer tracing identifies a reusable packed-nibble font path at
`$C32740-$C328A5`: it converts a value from `$C45B22` to character bytes,
indexes glyph offsets at `$C3D790`, and composites glyph bytes through the
`$C32858` strided-long loop. In an authentic frame-991 invocation,
`$C45B22=$00000171` is converted to ASCII `0171`; the screen is still showing
`161 KTS` at that phase and shows `171 KTS` by frame 994. This directly ties
the formatter to buffered cockpit-number production, while also explaining
why endpoint snapshots of `$C45B22=$00000040` at frames 993 and 994 cannot be
used to reject it: the workspace is reused later in the frame and also by
postflight code. It is not yet assigned as the persistent KTS or FT state. See
`analysis/routines/c32740_packed_nibble_font_renderer.md`.

## Next evidence

Trace the frame-991 `$C45B22` writer and the formatter's coordinate/record
consumer, then connect that buffered glyph submission to the changed numeric
rectangle. That producer/state/consumer chain is required before naming the
corresponding cockpit live variable.
