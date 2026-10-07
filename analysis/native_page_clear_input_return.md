# Native page-clear return consumed by recorder input — 2026-10-07

The periodic C2F582 page-top clear now returns its cleared longword pattern.
The native flight clock owner publishes that pattern for the following first
pending command. C2F59A constructs zero and C2F5A2 explicitly places the same
pattern in the value inherited by pending flare/chaff selection; this is a
reconstructed producer, not a general zero substitute for depleted commands.

The runtime path is native entry -> frontend -> flight -> finish_frame_clock
-> clear_page_plane_tops, followed by the next C0F3C4 pending-input parent.
The clear occurs only at the original `(saved_tick & 31) == 8` cadence.
All ten cleared longs per plane, four planes and both pages remain unchanged.
Existing CPU adapters can ignore the new return and retain their original ABI.

`NativeInputReturn` identifies either a final message return or page-clear
pattern. It is discarded before a new body and after the first command.
C2B3C2's three initial memory comparisons preserve the preceding value when
they skip labels; the shared owner now reports whether that pass was selected.
Selected label or debug-overlay passes invalidate the page-clear result because
their complete return contracts remain unfinished. C32CEE replaces the result
when it assigns a message/glyph byte and preserves it on no-assignment exits.
Other frame producers remain UNKNOWN and fail explicitly if consumed.

`python tools/native/check_countermeasures.py` reaches twelve page clears after
ordinary disk/key Free Flight startup. UPDATE_TICK is never seeded: each case
waits for the normal counter to reach its clear cadence. Controlled depletion
settings follow each completed clear. The original body oracle independently
computes and checks the return; the original input oracle then consumes that
original result. Native gameplay does not read the reference value.

| Scope | Result |
| --- | --- |
| Complete actual flight bodies | 31 match compared RAM and every drawing-page byte: four keyboard bodies, fifteen message bodies and twelve periodic-clear bodies |
| Actual recorder input parents | 48 match compared RAM: 24 claimed/chained, twelve message-derived first-depleted and twelve page-clear first-depleted parents |
| New variants | Recorder modes 1/2/3, flare/chaff, depleted stocks 1/$80 and raw/translated queue wrap positions |
| Existing input/control contracts | 2,512 source input parents, 256 control-effect cases, two $FD parents and four actual collision parents pass |

Native Release/Debug and both reference MSVC runners build. Affected host-key,
scene-exit, complete-frame-body, frontend, queued-input, consumed-game-input
and automatic artifact cleanup checks pass. Passing comparison RAM remains
temporary; logs and capture metadata remain under
`build/native-flight/countermeasure-check/`. No display/HUD exclusions changed.

HUD fill/text returns, active markers/labels and debug-tail returns, returns
after intervening commands, complete scenario acceptance, typed-state migration,
audio fidelity and measured 20 ms performance remain unfinished. The goal is
active. This batch adds one connected drawing producer and does not establish
general depleted-recorder support.
