# Native debug text return consumed by recorder input — 2026-10-07

The connected numeric debug overlay now returns the actual final character
or glyph selection of the existing four-plane text renderer. This removes
the unfinished return dependency for first depleted recorder flare/chaff input
following that overlay. Its prior behavior invalidated all active debug output.

The playable caller is native entry -> frontend -> native_flight_tick ->
native_frame_debug_overlay -> draw_stream_numeric_fields -> numeric_child ->
draw_text. The drawing owner remains in port/game/text.c; composition remains
in port/game/native/frame_tail.c. No reference return enters native gameplay.

## Original contracts

C32B12 loads every character, including spaces and characters subsequently
clipped. C32B4C selects the glyph address before C32B52 tests an odd destination.
The four C330FE children preserve that selection. The shared renderer now
returns these domain results; glyph selection moves ahead of the existing odd
window skip to match the source. Its pixels, clipping and raster calls retain
their existing implementation.

The numeric parent always draws its first four-character field and optionally
two further fields at the original C457B3 gate. Each later field replaces the
preceding text result. This final result supersedes the earlier debug page mark.
Both initial overlay gates preserve the incoming domain result when inactive.
The flight loop composes this after labels and before the final message owner.
The first input dispatcher can consume the defined byte and invalidates it
after a command, as in the previous batches.

## Validation

The countermeasure fixture starts normal disk/key Free Flight. Twelve new full
bodies select one or three debug fields through controlled validation gates;
normal pages, counter values and flight ordering are retained. Each completed
body is followed by controlled first-depleted recorder input. Original bodies
independently derive/check the return, and the original input oracle consumes
that original value. Native gameplay never reads reference values.

`python tools/native/check_countermeasures.py` now matches 55 complete bodies
and 72 actual recorder input parents against compared original RAM and every
drawing-page byte. Twelve debug bodies/parents join the prior keyboard,
message, page-clear and HUD/redraw cases. Existing 2,512 source input parents,
256 control-effect cases, two $FD parents and four actual collision parents
remain passing.

The frame-tail component oracle compares 256 original gated overlay returns
and all non-stack RAM. Controlled last-character columns cover normal glyph
selection, an odd window, the last visible byte and left/right clipping.
Cases also cover both pages, optional extra fields, inactive gates and signed
numeric values. The original inherited input is independently initialized to
$51AB12E7 to verify preservation. The existing 64 cleanup comparisons pass.
These bounded components are separate from the actual full-body evidence.

Ten affected CTests pass: host keys, scene exit, frame body, frontend, HUD,
frame tail, input, game input, qualification and artifact cleanup. Native
Release/Debug and both reference MSVC runners build. Qualification's fatal
out-of-scope fixture boundaries now use the current native signatures; they
still abort if reached. Passing RAM stays temporary and no comparison masks
change.

The full-port goal remains active. Scene-label traversal can overwrite a
previous glyph at its next C2B460 MOVEM, including the terminator row; its
complete return must account for the alternate C2B45E skipped-row exit too.
Those contracts, active markers, earlier/clipped HUD producers and intervening
commands remain unfinished. Complete scenario acceptance, typed-state
migration, audio fidelity and measured 20 ms performance remain open.
