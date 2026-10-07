# Native HUD marker-line input return - 2026-10-07

The actual line renderer now exposes its final size or defined clipped-start X
delta to both HUD marker callers. Their result reaches first depleted recorder
flare/chaff input through the existing native HUD/frame composition. Previously
a selected marker line invalidated that input even when the source defined it.

The playable caller is native entry -> frontend -> native_flight_tick ->
native_hud_draw -> draw_mode_bar/draw_indicator_bars ->
draw_line_to_row_result. Rendering remains in port/game/render_line.c; the
existing void drawing functions delegate to that same owner. Native HUD
composition remains in port/game/native/hud.c. There is no alternate renderer,
captured return or CPU register file in gameplay.

## Original contract

C2FB42-C2FB46 constructs the actual line blit size before the plane gates. It
therefore defines this result even when no plane is enabled. For an endpoint
order taking C2FABA, C2FAC0-C2FAC2 computes the signed X delta before the C2FAC8
last-row rejection. Other last-row rejection paths return without assigning a
new output. The existing line setup retains this early delta separately from
the accepted line's absolute dx.

LineDrawResult exposes size, X delta or no output. The indicator and mode-bar
owners preserve their preceding result when the line assigns nothing; an actual
line result replaces it. Subsequent bar fills, text and frame-tail owners retain
their original ordering and can supersede it normally. Source horizontal lines
retain their one-row increment and size calculation. Raster pixels, clipping,
colours, plane selection and source timers are unchanged.

## Validation

The countermeasure integration fixture retains its existing 67 full bodies and
84 input parents. It then starts another ordinary disk/key Free Flight through
the actual shared runtime, independent of the preceding controlled map-view and
collision parent fixtures. Twelve controlled indicator redraws leave the later
B/C/E bar gates inactive and avoid the periodic clear/redraw boundaries through
the normal game counter. Each actual complete body is followed by first depleted
recorder input. No game counter, line size or reference return is seeded.

Original body execution independently derives/checks the return and supplies
that original value only to the original input oracle. Native input consumes its
own line output. All 79 full bodies and 96 input parents match compared RAM and
every drawing-page byte. Existing 2,512 input parents, 256 control-effect cases,
two $FD parents and four actual collision parents still pass.

The line component oracle uses two ordinary runtime snapshots. Each checks 128
original line returns and all non-stack RAM: 24 preserve inherited input, eight
publish the clipped X delta and 96 publish size. It covers forward/reversed
horizontal lines, both endpoint orders, all major-axis routes, clipped lines,
degenerate points, last-row rejections, fixed/dynamic last-row entries and zero,
one, mixed or all enabled planes. The inherited input is independently set to
$51AB12E7. Defined size/delta words match the source word; preservation exits
match the full inherited value. These are domain-output checks, not a claim of
complete CPU-register parity. Existing 160 polygon, horizon and complete map
comparisons pass at both snapshots.

The HUD oracle adds actual indicator/mode marker selection and last-row skips.
Three snapshots each match 164 non-stack RAM cases and 84 defined input returns:
492 RAM cases and 252 returns total. Each includes two forced marker-line
results and both corresponding last-row preservation cases, along with the
previous odd-glyph cases. These bounded checks are separate from complete-body
integration. No comparison mask changes; passing RAM stays temporary.

Ten affected native CTests pass: host keys, scene exit, frame body, frontend,
HUD, raster, frame tail, input, game input and artifact cleanup. Native
Release/Debug and both reference MSVC runners build. The previously manual
raster check is now registered in CTest. Its stale C10C08 checkpoint assertion
is corrected to C10DAE, the existing source-backed Free Flight context-update
owner already verified by the HUD checker; the exact stage check is retained.

The full-port goal remains active. Active grid/aircraft markers still invalidate
input until their traversal and projection/line children expose the complete
composition contract. Remaining HUD or intervening-command returns, complete
scenario acceptance, typed-state migration, audio fidelity and measured 20 ms
performance remain unfinished.
