# Native qualification result milestone

Scope: carrier qualification success messages, pilot-log save and viewport /
scene restart in `fa18_native`. This functional scenario milestone is 100%
complete with original consumed keyboard input. Two of three sealed scenarios
now have demonstrated native functional outcomes (about 67% of that specific
three-scenario checklist). Zero of three has full native frame-parity acceptance.
Neither measure estimates whole-game emulation-removal completeness.

The runtime path is native frontend -> flight tick -> post-input tick -> stage
callback. It now directly runs C11078/C110A4 for results, C0F946/C0F974 for
viewport readiness and C0F992/C08F26 for restart. The original mode-9 success
branch sets the pilot record's qualification word to 1, queues $4A/$8053 and
publishes the original phase/sequence changes. Its C1643A config-write boundary
reuses the existing native 78-byte save overlay. C0F992 calls existing native
voice release, bootstrap and long-table loading. Returning to C0FCB4 activates
the existing menu composition while preserving the source's selected mode.

Validation:

- `tools/native/check_qualification_result.py` reuses the original consumed-key
  export and end RAM. It runs native cold startup through all 8038 recording
  updates / 554 consumed keys, demonstrates success, reaches restarted carrier
  flight at C10DAE/mode 9, and leaves no queued keys or crash reset.
- The actual saved config is exactly 78 bytes, equals native pilot RAM, contains
  qualification word 1 and reloads byte-for-byte in a fresh native launch. The
  original reference has the same success word. Other pilot-log fields are not
  claimed equal: the sealed start state and supplied ADF have different logs,
  and flight clocks/cadence remain distinct.
- Stable end aircraft pose/contact, kind, orientation, speed and both matrices
  match the original end checkpoint. Complete end state does not: record +$26,
  +$38, +$4C, +$74 and +$76 differ after the result/restart. In the observed run,
  native's game tick is 204 versus the original's 12. These timing/state debts
  remain open rather than being overwritten with reference values.
- Fifteen focused C11078/C110A4/C0F946/C0F974/C0F992 cases compare original
  non-stack RAM, countdown gates, viewport mismatch, qualification phases and
  restart. Both paths share the external config-write boundary in this fixture;
  the fixture does not claim to prove the original OS/file-service internals.
  Actual host file persistence is independently exercised above.
- Original crash-to-menu/reselection, qualification startup/short takeoff,
  frontend/save/reload/SDL and native link omission regressions pass.

No new reference replay was needed for result wiring; native checks reuse the
carrier evidence generated while resolving input alignment. Copper fade remains
excluded. Next: demo runtime/playback, complete frame ownership and result /
restart timing alignment. Other mission result children, audio and physical
gameport acquisition remain open.
