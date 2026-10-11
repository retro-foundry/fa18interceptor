# Native grid/aircraft-marker input return - 2026-10-07

The actual grid and aircraft-marker owners now compose their defined output
into first depleted recorder flare/chaff input. A selected grid previously
invalidated that input unconditionally. Inactive grid gates preserve the
preceding drawing result.

The playable caller is native entry -> frontend -> native_flight_tick ->
native_frame_grid_and_markers -> draw_view_grid_and_markers. Point transforms,
marker heading/shape selection, clipped segments, number text and line drawing
remain in their existing port/game owners. Native composition lives in
port/game/native/frame_markers.c. This removes the selected-grid UNKNOWN
dependency without adding captured returns or CPU register storage to gameplay.

## Original contract

C2AFFA publishes the transformed depth. Each eligible control record can replace
the preceding grid result with that depth; skipped records leave it untouched.
The C2EC90 mode-minus-five projection preserves this output on all exits.
Record marker selection can then publish its cached/refreshed heading kind,
signed doubled shape-table offset, or a defined line size/X delta. Rejected
line starts that assign nothing preserve the preceding result.

C2EE4A loads the current endpoint height at C2EE60. Its four crossing helpers
save and restore D0-D6 at both C2F1AA/C2F1B2 exits, so their intermediate
interpolation arithmetic does not replace that height. An accepted crossing
loads its height at C2F03A. Projection reflects the screen height at C2F074;
the final line result replaces it only when assigned. The actual segment owner
now exposes these three domain outputs alongside its existing drawn flag.
The legacy integer API delegates to the same body. Pixel calculations, clipping,
plane selection, segment exchange and error handling retain their existing code.

Grid labels expose the actual number-text character/glyph result. MarkerDrawOutput
keeps these domain outputs separate from the geometric MarkerState working
values and the legacy validation/glue observers. Native flight composes the
result in its original position, after selection cleanup and before the timers.

## Runtime integration

The existing countermeasure suite retains its 79 complete bodies and 96 input
parents, then selects twelve grid frames in the same fresh ordinary Free Flight
used for the preceding HUD marker cases. Validation selects the existing grid
gates; it does not seed observer position, matrices, depth, game counters or
reference returns. The actual source owners compute those values. Normal counter
progression avoids the periodic clear/redraw boundaries in these twelve frames.

All 91 complete bodies and 108 recorder input parents match compared original
RAM and every drawing-page byte. Each new complete body's original execution
independently derives and checks its output; only the original input oracle
receives that original output. Native input consumes its own composed result.
Existing 2,512 input parents, 256 control-effect cases, two $FD input parents and
four actual collision parents also pass. Passing raw captures remain temporary.

## Component checks

Two runtime snapshots each pass 401 complete grid/marker return and non-stack
RAM cases: gated entry, class/record routes, view matrices, cached/refreshed
headings, blink/visibility skips and actual map/flight state. Sixteen added
empty-record cases expose the last grid segment/number without a later record
transform superseding it. Each snapshot has 240 visibly drawn cases. The
existing ten-byte host grid scratch exclusion is unchanged; no drawing mask
was widened.

Two snapshots each pass 128 clipped-segment return/drawn-flag/non-stack RAM
cases: 32 endpoint heights, 24 screen heights and 72 line results. They cover
both endpoint orders, crossings at each side of both axes, rejected segments,
last-row rejection and zero/one/mixed/all plane gates. Defined words match
independently executed original source. These are domain-output comparisons,
not a claim of complete CPU-register parity. Existing line, polygon, horizon
and complete map comparisons still pass.

Eleven affected native CTests pass: host keys, scene exit, frame body, frontend,
HUD, raster, frame tail, input, game input, qualification and artifact cleanup.
Native Release/Debug and both reference MSVC runners build.

The full-port goal remains active. Selection-cleanup and intervening-command
return contracts, remaining earlier HUD outputs, complete scenario acceptance,
typed-state migration, audio fidelity and measured 20 ms performance remain open.
