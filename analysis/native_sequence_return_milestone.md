# Native flight sequence return to the menu

The playable caller is `fa18_native` -> `native_frontend_tick` ->
`native_flight_tick` -> C0F5F8 stage dispatch -> C0F920
`reset_sequence_after_bootstrap`. Its C08F26 child uses the same native player,
pose, recorder, template-gate, record and context owners as cold startup.
C0F920 clears the request, selected mode and recorder mode, then publishes
C0FBE0. After the last flight display returns, the following update enters the
existing native menu composition. This also permits another qualification run.

The bootstrap clear now calls source C2FD22 `clear_render_buffers`, rather than
clearing host display pages. Source renderer work banks must be reset when
returning from a dirty flight scene. Intro/menu host presentation still owns its
ordinary pixel-page clearing.

Validation:

- `check_sequence_return.py` completes the entire sealed crash input file:
  3082 input iterations, 168 events, three postflight callbacks/resets and a
  return to mode 0 / C0FCB4 main menu. The sealed recording's note describes
  the same three-crash/menu outcome. Native execution takes 6610 host PAL ticks,
  so recorded-frame cadence is explicitly not matched.
- A subsequent raw digit-5 selection restarts qualification and reaches the
  C0FBB6 briefing with its carrier scene constructed. This proves that the
  returned menu remains connected to play.
- Four original C0F920/C08F26 cases compare every non-stack RAM byte, including
  dirty renderer work banks, the conditional fifth bank, recorder state and
  final callback. They pass at both the end and restarted qualification
  checkpoints. The fixture supplies C0EFD4's valid inherited frame: C1C860
  writes its -$2C flag there before C1E328 sorting. A6=0 would discard that
  write in unmapped memory and give an invalid one-list reference result.
- Existing qualification startup/briefing/takeoff and source view/record/
  rendering checks pass. Frontend/save/reload, SDL and link omission checks
  pass. The native link still omits CPU, translations, glue, bus and chipset.

This sequence-return milestone is complete (100% of this functional scope).
One of the three sealed input scenarios now runs to its stated outcome in the
native runner; that is scenario coverage, not a whole-game completion estimate.
Full native/reference frame parity is open for all recordings, as are original
menu/flight cadence, remaining HUD/record/frame owners, other modes and sample
output. C0FBE0 still uses the established native intro/menu composition; this
batch does not claim complete original outer-loop ownership. No expensive full
reference replay suite was repeated. Copper fade remains excluded.
