# Complete native menu setup

Runtime caller: `fa18_native` -> `native_frontend_start_menu` ->
`native_menu_begin` / `native_menu_resume` -> C0FBE0's
`begin_top_level_menu` / `finish_top_level_menu`. The reference entry
`start_top_level_menu` calls both halves consecutively, retaining the original
child order. Native source lives in `port/game/native/menu_start.c` and is
linked by the active native CMake target.

The former native entry called only the queue helper. It skipped C17B96 sound
selection, the master-volume target, C2FD22 work-bank clearing, C11312 reset,
C0E78A pause, context reset and C11ACC palette-table load. All of those now
execute through their actual game owners. The frontend's forced TONE_MUTE=1
and VOLUME_FADING=2 stores are removed. Startup tone requests now publish their
original programs; audible output and asynchronous program updates remain open.

C0E78A's $C000 loop converts to 23 nominal PAL ticks, using the same 66-cycle
conversion as the existing splash pause. The host suspends game updates and
replay delivery until the deadline. Keyboard events arriving during that pause
enter the existing native input queue and are processed afterward, so later
menu publication cannot erase a selection made during the pause. This is a
native scheduling contract; it does not recreate CPU/DMA timing.

Validation:

- 18 source cases execute original C0FBE0, C17B96, C2FD22, C11312 and C11ACC
  instructions with only the busy pause supplied by the host. Cases vary the
  intro/menu sound flags, fade state and fifth-buffer usage, with dirty work
  banks and context/volume state. Every non-stack RAM byte matches the actual
  native module both before and after the pause. Early and repeated resume
  calls leave the game state unchanged.
- The actual runner's menu pause freezes update, record, glyph and game-tick
  counts. A digit press/release during the pause stays queued, then selects
  Free Flight through the original input owner. Native menu volume and target
  are $001F0000, fade state is 1, tone mute is 0, both selected menu sound slots
  and the complete palette table agree with their source owners.
- Settled splash/credits/menu and all five menu banners still match source
  pixel digests. Callsign editing, save/reload, mission gates, Escape, pilot-log
  reset and SDL presentation pass. Native link omission remains valid.
- The complete native demo and its 23 sound/31 startup-launch source cases
  and two record parents pass. The measured startup lead remains 37 ticks;
  this batch makes no claim of reducing that gap.
- Native carrier qualification/save/restart/reload and its 15 source result
  cases pass. Crash/menu/qualification re-entry and four source reset cases
  pass. Both MSVC reference runners build and twelve focused CTests pass.

C0FBE0 menu setup is **1/1 source parent connected and verified (100% of that
scope)**. Whole-game timing and rendering remain incomplete. Functional
scenario wiring is 3/3; full recorded frame parity remains **0/3 accepted**.
Copper fade is excluded. No full original replay suite was repeated.
Next work: asynchronous input/audio cadence and missing frame-tail owners,
including C12242, C2B564/C2B3C2, C30A00 and the final C2F49C/C31B76 overlay.

Run `python tools/native/check_menu_start.py` for the source comparisons and
the actual runner pause/input checks.
