# `$C32740-$C328A5`: packed-nibble font-rendering path

Classification: **scenario-backed formatter-to-framebuffer dataflow** for the
observed run029 path.  The workspace is proved reusable; the gameplay-state
producer and final screen placement remain unassigned.

The byte-exact prefix is now
`source_amiga/observed/format_packed_font_text_prefix.asm` for
`$C32740-$C3278B`. It joins the byte-exact `$C3278C/$C32794` entry and the
`$C327A0` glyph loop, then the existing `$C32806` row compositor.

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

The first stepped glyph stream starts at `$C3D847`; its destination starts at
`$0186F6` and advances by `$28` for six scanlines. The second starts at
`$C3D842` and writes the adjacent destination sequence. These addresses and
the merge loop establish a concrete font-glyph-to-framebuffer bridge.

An authentic normal replay gives the stronger four-character case. At
chipset frame 991, a breakpoint at `$C32740` finds `$C45B22=$00000171` and
enters with `D0=3`. The repeated low-nibble conversion writes, after the
backward scratch stores settle, ASCII `$30 $31 $37 $31` (`"0171"`) at
`$C45800-$C45803`. Its screen still shows `161 KTS`; the later frame-994
screen shows `171 KTS`. Thus the formatter is preparing buffered cockpit text
ahead of its presentation phase, rather than necessarily formatting the value
visible in that same screenshot.

## Scope

This does not establish `$C45B22` as a persistent speed, altitude, or other
cockpit-state field. Static-only postflight code (`$C3341A` onward) also uses
the workspace, and endpoint snapshots show it as `$00000040` at frames 993
and 994. The differing contents are evidence of reuse at different times in
the frame, not a contradiction of the normal formatter observation.

`$C25A08` is now separately proven to fill this workspace from the raw-long
input at `$C45B1E`; see
`analysis/routines/c25a08_workspace_packed_bcd_conversion.md`. That conversion
is shared infrastructure, so its caller still determines the field semantics.

The proved contract is therefore a reusable packed-decimal formatting
workspace feeding the glyph table and CPU compositor. The normal `0171`
invocation is a strong KTS correspondence, but tracing its writer and the
coordinate/record consumer is still required before assigning a cockpit live
variable or a fixed screen field name.

## Postflight entry linkage

The static postflight message route now reaches the same compositor through
the distinct `$C3278C/$C32794` entries: see
`analysis/routines/c3278c_postflight_glyph_renderer_entries.md`. Its
`$C325A6` caller supplies the postflight text buffers and style-controlled
parameters. This joins that route to the compositor implementation, but does
not make the run029 glyph trace evidence an oracle for a postflight or
qualification screen.
