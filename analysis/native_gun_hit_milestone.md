# Native gun damage hit checkpoint — 2026-10-08

The normal-input mode-eight gun-hit check matches 317 input/stage intervals
and 181 sampled bodies against original compared RAM/drawing, including the
exact first damage body. The pilot-log gun counter advances from zero to one
and enemy aircraft ten's damage nibble advances from zero to one with its
flags unchanged. This batch extends validation of the existing playable game;
it changes no gameplay source, rule or playable executable.

## Source ownership and input scope

The actual playable path is `native_frontend_tick` -> `native_flight_tick` ->
`native_scene_draw` -> control effects -> C1518C/C15688/C159AE -> C266AE.
The source collision owner advances the gun counter at pilot-log +60 and
increments target damage before the third-hit destruction branch. The fixture
checks the actual gun counter in native before/after RAM and independently
executed original after RAM. It identifies the first target by the changed
damage byte, since this path need not publish C4FDD2/HISTORY_RECORD.

The validation-only pilot in `tools/native/gun_discovery.c` reads aircraft
positions and the player matrix, then emits ordinary pitch, rudder and fire
key events through `native_frontend_event`. It anticipates the existing source
control ramp and release behavior. It supplies no aircraft position, flight
control RAM, projectile, damage or outcome. Its aiming thresholds and feedback
are a test input strategy, not game behavior, and it is never linked into
`fa18_native`. Only the existing saved-pilot mission-availability fixture is
prepared before startup through the normal loader.

The first hit occurs in body 6534, ticks 14035–14037. Enemy record ten has
kind $12, flags $70C1 and damage 0 -> 1. The later flight naturally ends and
resets; the gun-hit scenario requires its existing 2,000-frame checkpoints.
Existing sustained combat and missile probes retain their 5,000-frame guard.
Original comparisons execute from each sampled native before-state. This
establishes the first damage body and sampled composition, not an independent
complete original flight, a successful mission or a verified gun shoot-down.
No comparison exclusions or gameplay fallbacks were added.

## Unresolved shoot-down diagnostic

The stricter `--kill --gun` probe observes three native gun hits, damage
0 -> 1 -> 2 -> 2, destruction on the third hit, enemy expiry accounting
0 -> 1 and eventual target inactivation. It requires every body in the
101-body first-damage-to-inactivation interval. Its original comparison
**fails**, so these native observations are not shoot-down acceptance.

The retained failing case is body 6540, ticks 14055–14058, saved tick 1138,
stage C10DAE (`build/native-flight/gun-kill/failure/frame.79.*.dat`). It has
two differing bytes in one gameplay word and zero display differences:
C4F6DE is $00C2 in the original and $FFFF in native. This is the first
alternate-followup placement's return cache for destroyed aircraft ten,
descriptor C22250, routine C22AC0, stream C3849E. Its lifetime advances
13 -> 12 with flags $54C1 in both executions.

Trace the connected path `visit_followup_placements` -> scene `followup` ->
`native_scene_placement` -> `aircraft_descriptor` -> `native_model_draw` ->
`record_vertices` / `record_finish`. Source C1F074 takes early expiry before
C1F712 initializes the normal model accumulator; C1F8DE returns the incoming
word at -$7C(A6), and C1CFBA stores the return cache. The native equivalent
reads `frame - 0x7c`. The isolated model oracle copies native caller scratch
into its independent original stack, which does not prove the producer of
that value in the complete original frame. Next work is tracing that producer
and its native ownership before changing runtime behavior. Do not seed native
state from reference RAM or exclude the differing cache word.

## Reproduction and retention

```powershell
python tools/native/check_mode_two.py --mode 8 --hit --gun --out build/native-flight/gun-hit
python tools/native/check_mode_two.py --mode 8 --kill --gun --out build/native-flight/gun-kill
ctest --test-dir build/native-cmake -C Release --output-on-failure -j 1 -R '^fa18_native_(gun_hit|radar_kill|infrared_kill|frontend|artifact_cleanup)$'
```

The gun-hit test is registered in CTest. The failing gun-kill probe remains
a manual diagnostic until its return contract is fixed. Debug and Release
fixture builds and the five affected CTests pass; compact evidence is in
`analysis/figures/native_gun_hit_checkpoint.json`. Passing RAM is temporary
and deleted after comparison. Only the failed before/after/source case is
retained locally; no raw capture is committed.

After the gun expiry mismatch, remaining work includes successful complete
missions, independent full-flight comparisons, callback contracts, typed game
state, audio fidelity and broader visible/combat performance. The complete-port
goal remains active.
