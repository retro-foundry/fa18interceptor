# Native cold scene startup

2026-10-07. Native `frontend_open -> native_flight_initialize` now composes
original C08EE4 and C08EB8 before the existing C08F26 scene bootstrap. Source
C0F4D8 calls those owners at C0F550/C0F556 before C0F812 invokes bootstrap.
The readable owners belong in `port/game/scene_bootstrap.c`; the native caller
belongs in `port/game/native/flight.c`.

C08EE4 initializes the aircraft selector C45849 to $11, viewport state and
other cold defaults. C08EB8 copies the saved pilot-log level's low byte into
the current/previous scene selectors. Previously native passed the executable
hunk's zero selector to root placement, constructing a different record kind.
Its first update set a sixteen-update counter and region flag. Changing those
fields after construction would conceal the missing startup owner; the fix
executes the original owning sequence instead.

All sixteen initial $A4-byte record cores now match retained original startup
RAM. Independent takeoff ticks 7..229 match 223/223 complete drawing/player/
camera boundaries and 3,568/3,568 record cores. Later ticks 222..584 match
363/363 player/camera boundaries and 5,808/5,808 cores, including all flags and
countdowns. Strict later drawing remains 83/363; its existing seconds/view-hold
expiry assessment is unchanged. No captured state, fitted delay, unconditional
flag clearing or extra comparison exclusion supplies native behavior.

Thirty-two complete original C08EE4/C08EB8 parent cases match all compared
non-stack RAM, including word-to-byte saved levels. The demo check now rejects
the wrong cold aircraft/counter/flag and compares all sixteen cores when a
reference initial state is supplied. Disk-backed demo runs at iterations
2000/2400/3000/4892 pass their existing original record/model/startup contracts.
Eight affected runtime CTests pass: menu, frontend, Free Flight start/return,
both selected aircraft, record expiry and scene exit. Free Flight Delete/
Escape/restart matches 57 input intervals and 37 bodies. Controlled postflight
matches nine intervals and 25 bodies with complete source file owners.
Release and Debug native builds pass; public Release is refreshed.

Evidence is in `build/native-flight/demo-startup-review/`. The two retained
original windows are verified `.dat.gz` files in `demo-takeoff-review/` and
`demo-later-review/`; `check_gameplay_window.py` reads them directly. Reports
remain; passing native captures are temporary under the new retention policy.
Whole independent sequences, recorder carry, normal mission success, readable
typed state, audio fidelity and measured frame performance remain unfinished.
