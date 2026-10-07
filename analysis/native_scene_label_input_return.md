# Native scene-label and context HUD returns - 2026-10-07

Scene-position labels now return their actual final row component, matrix
product or number-text selection to the next first depleted recorder flare/chaff
command. The connected context speed, altitude and heading owners also return
their actual text selection when labels skip. This removes the unfinished input
dependency for these paths without supplying reference values to gameplay.

The playable caller is native entry -> frontend -> native_flight_tick ->
native_frame_scene_labels -> draw_scene_position_labels -> label_child ->
native_draw_position_number -> draw_text. Shared traversal remains in
port/game/flight_markers.c; native composition remains in port/game/native.
Context HUD composition calls the existing port/game/hud_readouts.c owners.

## Original contracts

C2B460 loads the signed row-Z word even for a terminator row. That load
supersedes the preceding number renderer's return. C2B514's final matrix
multiplication publishes its Z product; projection rejection preserves it.
Accepted projected points invoke C32A44, whose actual character/glyph result
supersedes the product. The alternate C2B45E skipped-row exit preserves the
latest preceding output, including the caller's result if no row was loaded.
Initial gates also preserve the caller's result.

SceneLabelResult exposes these domain outputs separately from MarkerState.
An optional host callback retrieves the actual number renderer result; the
reference adapter retains its existing observation path. Native composition
selects the low byte only at the input boundary. No CPU register file or captured
return enters the playable runtime.

The context HUD branch composes speed -> altitude -> heading -> optional
message in original order. The small-text renderer's second context-plane draw
provides each readout's final result. Source cache/context gates that perform
no text draw preserve the preceding result. The flight loop then composes page
clear/redraw, labels, debug overlay and final messages in their existing order.

## Validation

The real countermeasure fixture starts normal disk/key Free Flight. Twelve new
completed bodies select detail modes 5/6/7 through controlled validation inputs.
Observer and control-record positions derive from the actual start_position
owner; the normal frame computes POSITION_BIAS. The source detail hold counter
keeps these views selected. No UPDATE_TICK or computed bias is seeded.
Two threshold-skipped label frames preserve the context HUD output; the other
ten publish scene-label results, including alternate skipped-row exits.

Each body is followed by controlled first-depleted recorder input. The original
body independently computes/checks its return, and that original value supplies
only the original input oracle. Native input consumes its own owner result.

| Scope | Result |
| --- | --- |
| Actual complete flight bodies | 67 match compared RAM and every drawing-page byte |
| Actual recorder input parents | 84 match compared RAM, including twelve new label/HUD-derived parents |
| Scene-label component | 256 gated cases match original returned input and non-stack RAM; 41 draw visible pixels and 41 publish numbers |
| HUD components | Three runtime snapshots each match 150 original non-stack RAM cases and 69 defined returns: 450 RAM cases and 207 returns total |
| Existing input/control contracts | 2,512 input parents, 256 control-effect cases, two $FD parents and four actual collision parents pass |
| Affected CTests | Host keys, scene exit, frame body, frontend, HUD, frame tail, input, game input, qualification and artifact cleanup pass |
| Builds | Native Release/Debug and both reference MSVC runners pass |

The label oracle independently initializes inherited input to $51AB12E7 and
covers inactive gates, detail modes, selected rows, blink skips, negative rows,
terminators and visible number draws. These bounded component checks are
separate from the actual full-body/input comparisons. Passing RAM remains
temporary; no comparison masks or drawing exclusions change.

The full-port goal remains active. Active grid/marker and remaining HUD or
intervening-command returns still need their source contracts. Complete scenario
acceptance, typed-state migration, audio fidelity and measured 20 ms performance
remain unfinished.
