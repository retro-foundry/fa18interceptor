# Native viewport and fade cadence

Runtime caller: `fa18_native` -> `native_frontend_tick` ->
`native_viewport_tick` -> `advance_viewport_palette`, C1718E's
C172DE-C1744C viewport/master-fade tail. This runs once per host PAL frame,
before menu waits, display continuations and frame timer polls. It is removed
from `native_flight_tick`, where it previously ran once per game update.

Source authority: C17456 registers callback C1718E through C53B00 with number
5. C53B00 loads ExecBase and calls vector -$A8, AddIntServer. The reference
machine raises interrupt bit 5 at every PAL frame start. The original
[AmigaOS interrupt documentation](https://wiki.amigaos.net/wiki/Exec_Interrupts)
describes the vertical-blank server chain and its AddIntServer contract.
This evidence supersedes the older registration note's unassigned service.

The host palette sink implements C53EC0's sixteen RGB4 colours for first,
second and stable publication. Stable publication was previously skipped.
The shared game owner still performs its source countdowns, mode movement,
view/palette-pair stores, final sixteen-word palette copy and quarter-step
master-volume fade. No interrupt controller, CPU or chipset is recreated.
The separate mouse-counter prefix of C1718E is not implemented by this tail.

Validation:

- 128 source sequences execute 50 original viewport/fade tails each: 6,400
  ticks covering both transition directions, signed countdown boundaries,
  both pages, palette flags and master-volume directions. Only RGB4 service
  publication is shared with the host sink. All non-stack RAM matches apart
  from the corresponding host scratch frame; all 32 host palette entries
  also match. C24FE8's original fade body executes in the reference path.
- Actual PAL frames 3,001/3,002 hold update, record, HUD, glyph and game-tick
  counts while the display wait is pending. Master volume advances from
  $001F0000 to $001EC000 in that interval. This verifies the callback runs
  independently of game progress in the actual runner.
- Native settled frontend/menu digests, callsign/save/reload, mission gates,
  log controls, SDL presentation and link omission pass.
- Both original cockpit assets remain source-byte exact and immutable.
  Four cache/mask cases and 230 populated HUD/panel comparisons pass. The
  native demo has 275/875 HUD passes at updates 2,400/3,000; the old game-rate
  viewport callback made 60 extra HUD passes during transition. Native tick
  259 at update 2,400 still leads the reused source tick 222 by 37.
- All 4,892 native demo updates complete and the final view/control and record
  parent matches source non-stack RAM. Carrier qualification/save/restart/
  reload and its fifteen result cases pass. Crash/menu/qualification re-entry
  and the dirty-bank reset comparisons pass.

The viewport/fade tail is **1/1 connected and verified (100% of this scope)**.
The full physical mouse input callback, asynchronous voice updates/output,
remaining frame-tail rendering and timing differences remain open. Functional
scenario wiring stays 3/3; full recorded frame parity remains **0/3 accepted**.
Copper fade is excluded from acceptance. Tests verify the source callback's
state changes separately; no full original replay suite was repeated.

Run `python tools/native/check_viewport.py` for the source sequences and the
actual runner's display-wait check.
