# `$C325A6-$C32678`: postflight message-buffer submission

Classification: **static dataflow in a runtime-observed postflight parent path**.

`source_amiga/observed/submit_postflight_message_buffers.asm` is byte exact for
`$C325A6-$C32678`. It begins with `$C4580A`, coordinate table `$C31998`, fixed
geometry `$1E0C/$0C`, and renderer inputs `$C45986/$C45918`. It rotates
`$C45862` by two and uses the retained two-bit value to make one or two calls
through the final glyph-render entry at `$C32794`, with the observed `D5/D3`
parameter pairs `(0,4)`, `(4,0)`, or `($C,0)`.

After restoring the initial render parameters, nonzero `$C45785` plus
`$C45793` selects a separate `$C3278C` submission geometry. In zero mode,
`$C45861` is a signed redraw delay; each nonnegative pass decrements it and
performs a second `$C32794` submission with the `$F0A` mask adjustment.

The `$C3278C/$C32794` entry setup is independently byte-exact in
`analysis/routines/c3278c_postflight_glyph_renderer_entries.md`; the existing
`$C32740-$C328A5` analysis supplies the scenario-backed glyph-to-framebuffer
bridge. This connects postflight text buffers and style bits to final
glyph-renderer calls. It does not prove the screen location, text-plane
meaning, or any qualification state/result semantics.
