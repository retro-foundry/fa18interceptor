# `$C3278C/$C32794`: postflight glyph-renderer entries

Classification: **static dataflow linked to the postflight message submit path**.

`source_amiga/observed/initialize_postflight_glyph_renderer_entry.asm` is byte
exact for `$C3278C-$C3279F`. The `$C3278C` entry swaps `D6`, clears its low
word, clears `D7`, and falls through to `$C32794`. The shared `$C32794` entry
adds `D7` to `A4`, sets `D7=$142`, and loads `$C456B6` into `A0`.

`$C325A6` reaches `$C3278C` in its nonzero-mode route and `$C32794` for its
ordinary render passes. The surrounding `$C32740-$C328A5` routine has separate
scenario-backed glyph-to-framebuffer evidence; this slice establishes the
postflight-specific entry setup without inferring a screen field or result
meaning.
