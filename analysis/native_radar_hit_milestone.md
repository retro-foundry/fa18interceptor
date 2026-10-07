# Normal-input radar hit and inactive-aircraft placement results

2026-10-07. The active build is `fa18_native`, using the shared native runtime
in `port/native/CMakeLists.txt`. Whole-game acceptance remains open.

## Connected correction

The caller is `native_frontend_tick` -> `native_flight_tick` ->
`native_scene_draw` -> C1CCBC's followup placement traversal ->
`native_scene_placement` (C22AC0). The normal-input mode-eight radar-hit probe
found two differing bytes at C4F6DE/C4F6DF, the first alternate placement's
cached descriptor result. Native wrote $FFFF; original instructions wrote $0001.
At body ticks 19276..19279, the cache header was $0A11 and its descriptor at
C22250 selected C22AC0. The associated record at C47584 was inactive, with
flags $0081, lifetime 20 and phase four. Drawing bytes already matched.

Original C22B04..C22B18 tests phase five and lifetime, optionally sets the
active flag $0040, and returns without assigning a new result. The native
routine previously returned zero. Placement traversal now passes its actual
preceding calculation through a named `prior_result` field, and this inactive
exit preserves it. Primary/alternate placements provide their shift or
refresh-clock phase. Selected followup placements provide their shifted
position level. C1D0A4/C1D0B6 and C1D91A preserve these incoming calculations.
Existing result normalization and cache ownership stay in the parent.

This removes an invented zero from the connected native descriptor contract.
It introduces no physics, damage, hit probability, timer or drawing rule.

## Gameplay and validation scope

The probe starts from the original ADF through intro/menu, selects mode eight,
and uses ordinary throttle, pitch, weapon-selection, target-selection and fire
events. A validation-only saved-pilot availability byte unlocks the mission;
the fixture reopens through the normal loader before flight. No aircraft,
motion, projectile, collision, hit count, outcome or return is seeded.

Original flight-geometry hit accounting increments the pilot log's radar-hit
word at +68 when a player-owned radar missile contacts an enemy record. The
test requires that counter to increase and captures the precise complete body
containing the increment. One 1 MiB in-memory before-state is reused while a
player radar missile is active; only the hit body adds two raw files. Passing
RAM is temporary, with compact logs and a comparison report retained.

The model oracle also checks inactive phase-four/five and zero/negative
lifetime boundaries with eight distinct incoming results, including negative
and signed-word limits. All 32 cases match original return, drawing and
non-stack RAM. Its cold-start fixture uses an actual ship descriptor, which
shares this unconditional inactive gate with aircraft; these cases do not draw
an inactive model. Controlled component cases remain solely in the reference
process. Aircraft runtime evidence comes from the ordinary-input flight.

The historical mode-eight combat probe's 500-tick pull-up now naturally ends
the flight before its long-flight assertion. Its test input uses a 100-tick
pull-up while retaining the 5000-scene-frame, motion and sampling requirements.
Its CTest now runs the original comparisons and requires a reached C06C02
guidance continuation. Playable controls and flight rules are unchanged.

All three weapon probes also now check their consumption at the first natural
C11788 postflight boundary, before source reset replenishes stores. The later
reset remains exercised and must restore the initial missile stock and gun
ammunition. Infrared and radar probes require two shots with the respective
stock decrement; the gun probe consumes 57 of its initial 500 rounds. Their
CTest wrappers compare actual input/stage intervals and sampled bodies against
original instructions, retaining passing RAM only temporarily.

## Results

The hit occurs in the C10DAE body at ticks 17137..17139, saved game tick 2051.
The radar-hit word in the actual pilot log at $2000 increases by one in both
native and original execution. The native run completes 14,856 scene frames;
57 input/stage intervals and 188 sampled bodies match compared original
RAM/drawing, including this precise hit body. Existing scratch, voice and busy
exclusions stay unchanged; all drawing bytes remain compared.

Native Debug/Release builds pass. Thirteen affected Release checks pass across
targeted runs: combat modes five through eight, all three weapons, radar hit,
natural outcome, frame body, frontend, models and artifact cleanup. The
frontend check verifies the native link's CPU/chipset omissions. The compact
checkpoint is `analysis/figures/native_radar_hit_checkpoint.json`.

Source comparisons execute original instructions from sampled native
before-states, including the actual hit body. This proves a native missile hit
and the compared composition, not an independent complete original mission,
a completed kill, or successful mission results.

```powershell
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_(radar_hit|combat_8)$'
python tools/native/check_mode_two.py --mode 8 --hit --out build/native-flight/radar-hit
```

Full gun/infrared/kill sequences, successful missions, independent complete
flights, remaining callback contracts, typed game state, audio fidelity and
broader visible-window performance remain unfinished. The complete-port goal
stays active.
