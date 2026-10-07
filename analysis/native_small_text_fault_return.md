# Native small-text release fault return - 2026-10-07

The connected small-text owner now preserves its selected glyph when an odd
destination takes the original release fault path. This removes the unresolved
return for that path. The existing error $46 write and drawing skip are unchanged.

The playable caller is native entry -> frontend -> native_flight_tick ->
native_hud_draw -> existing HUD readout/message owners -> draw_small_text.
The shared renderer remains in port/game/text.c. Its TextDrawResult reaches the
already connected native HUD composition and recorder input boundary.

## Original contract

C327D6 selects the glyph before the destination test. C327F6 writes error $46;
C327FE calls C06C02, whose actual release instruction is RTS. C32804 resumes
the character loop without changing the selected glyph. A later character load
or glyph selection can still replace that result normally. The native renderer
therefore retains TEXT_DRAW_GLYPH rather than publishing an unresolved fault
result. The unused separate fault enum is removed.

## Validation

The HUD oracle starts with three normal native runtime snapshots. Two additional
bounded variants give the active page planes odd destinations and exercise the
actual speed, altitude, heading, scale and message owners, in both cockpit and
context views. Source execution runs the real release fault hook; no fault
bypass is installed. Independent original returns and all non-stack RAM are
compared. Fixture assertions require defined fault returns in both variants.

Each snapshot passes 160 RAM cases and 79 defined returns, including nine
odd-destination fault returns: 480 RAM cases, 237 returns and 27 fault returns
in total. These controlled component cases establish the odd path separately
from full gameplay integration. The unchanged normal integration suite is also
rerun after the renderer change; it matches 67 complete bodies and 84 recorder
input parents, including every compared drawing-page byte. No comparison mask
changes. Passing RAM remains temporary.

Six affected checks pass: frame body, frontend, HUD, frame tail, input and
artifact cleanup. The expanded HUD fixture passes again after adding the
context variant. Native Release/Debug and both reference MSVC runners build.

The full-port goal remains active. Active grid/marker and remaining HUD or
intervening-command contracts, complete scenario acceptance, typed-state
migration, audio fidelity and measured 20 ms performance remain open.
