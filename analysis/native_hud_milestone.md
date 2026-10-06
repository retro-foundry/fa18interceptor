# Native cockpit/HUD milestone

The connected caller is `fa18_native` -> frontend tick -> native flight tick ->
scene drawing -> `native_hud_draw`. Nineteen instrument/panel owners now run
directly from the game sources, under C0EFD4's view gates and saved-tick cadence.
The executable executes their HUD marks, tapes, numeric readouts, threat lights,
panel frame/image, compass, panel mark, mode bar and indicator bars.

`hud_bars.c` now has explicit native branches for its panel/image/bar operations.
These call direct host plane copy, mask, fill/clear and compass shift operations.
There is no register file, hardware-register capture, minterm interpreter or
scheduled blitter in the native path. Reference builds retain the original
hardware branches. Unexpected image operation kinds abort.

`check_hud.py` exercises native frames 5300, 6100 and 6500, observing 780, 1307
and 1707 connected HUD/panel frame calls. Its oracle compares nineteen owners
across centered, left/right panned and fully clipped views: 95 cases at each
checkpoint. Every non-stack RAM byte, including planes, matches original
instructions. The oracle routes the source WaitBlit wrapper's library call to
the exact Kickstart 1.3 ROM entry because a native data fixture has no OS vector
table; the original ROM instructions and chipset still execute in validation.
None of those dependencies enters the native executable.

Validation passes: native MSVC build and link omission, frontend settled-screen
and save/load checks, Free Flight setup/pause/resume, all seven view/record
oracle checkpoints, all three HUD oracle checkpoints, and the reference MSVC
build with twelve passing CTests. The GNU validation build also passes.

This is component correctness plus demonstrated runtime integration. Whole
frame parity remains open: C12950 control/audio actions, C31226 postflight
dispatch, stores, message-line drawing, end-of-frame owners and complete
message/timer/counter ordering are not yet connected. Source tick cadence gates
are present, but the complete source frame clock still needs its owner.
The existing `scene-setup` label covers this partial flight loop.

Estimated Free Flight startup wiring is now about 97% (previously 95%), with
cockpit/HUD rendering connected. This excludes takeoff, other modes, full flight
and recorded-run acceptance. Copper fade is excluded; no full sealed replay
was repeated.
