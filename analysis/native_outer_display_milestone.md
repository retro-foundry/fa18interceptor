# Native outer display and postflight reset

Validated 2026-10-06 in `fa18_native`, built by `port/native/CMakeLists.txt`.
Runtime: `port/native/main.c` -> `native_frontend_tick` ->
`native_flight_tick` -> final C32CEE text -> `native_display_finish_frame` ->
`advance_outer_display`. Pending display services resume on later host ticks
before another game frame starts.

## Source ownership

Original C15D96-C15DB2 calls C2F558 (select draw page), C0EFD4 (game frame),
then C1612C (publish display). The existing C1612C owner now exposes a readable
continuation, while `synchronize_outer_display` retains its blocking reference
contract. Saved-pair publication, signed activity tests, palette child order,
counter reload/decrement, table-clear gates and final DRAW_PAGE swap remain in
that single source owner.

C53F88 invokes graphics.library WaitBOVP, LVO -402. Native presentation yields
to the next host PAL boundary. C53F44 WaitBlit completes immediately because
host raster submissions are synchronous. Native LoadView selects the completed
host plane page; no Amiga view/Copper object or device service executes.
Static palette uses C084D0; dynamic/clear loads use the current LONG_TABLE,
including all 32 source words. This preserves activity flashes; ignoring Copper
fade does not authorize ignoring other palette changes.

Both host pages now draw through C2F558's selected plane table. Presentation
reads the published page while the other page is being composed. The source
work-buffer tables used by C2FD08 have separate host storage; bank B's fifth
entry shares POLY_MASK_PLANE as in the source layout. Menu/intro composition
retains its earlier scope; complete outer-loop integration there is open.

## Runtime result

The short pullback replay now completes the crash/reset sequence. At tick 7400,
C11788 is queued and activity is 16; at tick 7404 it is 15. During these four
presentation waits, the player record, both plane pages, game counter, records,
HUD, text and control counts do not change. The original activity decrement
therefore runs once per four waits without repeating gameplay.

At tick 8200 the same replay has activity zero, one C11788 callback and one
completed reset; C10DAE and record updates have resumed. It renders the reset
aircraft's runway/cockpit. Counters: 989 record updates, 987 scene/HUD frames,
2761 publications and 2928 display yields. Native takeoff/positive model strips
and the negative-metric terrain regression still pass at their existing
checkpoints.

## Validation

- 128 resumable C1612C cases against original parent instructions: child order,
  publication/palette boundary values and all non-stack RAM, including signed
  activity values and changes at service completion. 2730 children and 5204
  suspended polls, with repeated polls checked for duplicate effects.
- 1024 complete blocking contracts: registers, PC, full SR and all RAM; all
  80 parent instruction boundaries visited. Controlled external child contracts
  are evidence for the owner, not a real Kickstart service timing comparison.
- Updated `check_postflight_entry.py`: actual four-wait native cadence, stable
  game/plane state during waits, completed reset and resumed gameplay; three
  original postflight entry/wait/release comparisons pass.
- Existing model, HUD, clock, action, strip/takeoff and terrain checks pass.
  Free Flight selection/pause/resume, frontend settled pixels, save/reload,
  SDL presentation and native link omission pass. Both reference builds compile;
  twelve reference CTests pass.

No full sealed replay was repeated. Copper fade remains excluded from frame
comparisons. Exact beam-position timing, input callback cadence, complete C0EFD4
ownership, C0DA38 alternate presentation, remaining HUD/record children, other
modes and native sample playback remain open. These results accept the bounded
display/reset milestone, not full recorded-run parity or whole-game completion.

Milestone estimate: display/reset integration 100% for this demonstrated path.
The earlier 99% Free Flight startup estimate remains a startup-only estimate;
it must not be interpreted as 99% of the whole port.
