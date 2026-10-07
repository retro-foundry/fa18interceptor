# Native weapon firing and returned point destination

Normal mode-8 input now runs all three Return/Space firing probes through
18,000 host ticks, reaching 2,458 scene/HUD frames at C10DAE. The eligible
saved-pilot fixture changes only the existing mode availability byte and
reloads through the normal loader. No gameplay records are seeded by the
integration test. This demonstrates these firing/lifetime paths, not successful
hits, complete combat or mission outcomes.

The actual caller is `native_frontend_tick` -> `native_flight_tick` ->
`native_input_process` -> weapon selection/Space publication ->
`native_control_records_update` -> readable primary record action/motion ->
scene placement/model stream. The two-Return probe originally aborted on
missing draw command $98. Its directory entry identifies C21E08. Native model
composition now calls the readable workspace extension function: ten vertices
at +$42..+$78 using original points and word-wrapped displacements. It consumes
no stream operand and returns the source zero. Reusable geometry stays in
`port/game/vertex_tail.c`; game composition stays in `port/game/native/model.c`.

The original C21E08 parent matches all non-stack RAM/display and its zero result
in 64 coordinate-wrap cases. The geometry component regression now passes 256
complete C21FA4/C0D524/C0D61C/C21E08 parents. Earlier overlapping-bank cases are
retained; the new operation uses only WORKSPACES.

The gun path reaches scene -> `native_control_effects` -> C1518C -> C153FC ->
C2CCA0 -> C2ECA8 -> C2F5F4. Complete-body comparison exposed one differing
pixel at $041230 in the body at host ticks 12003..12006. Instruction tracing
showed C2CD1E cleared bit $0200 at the point child's returned plane word +$26,
not at the original control-record address. The point child leaves the final
plane word address in its caller's destination. The native plot API now returns
that address explicitly; the control renderer consumes it for the original
test/clear. A rejected projection retains the control-record address. Other
plot callers ignore the new return value, keeping their existing drawing.

64 complete C2F5F4 cases compare the returned destination and all non-stack
RAM/display, covering eight columns, four rows including row zero, colours
9/13, plane masks and XOR drawing. No generic CPU register state is introduced.
The optional reference-only `FA18_FRAME_TRACE_PIXEL` diagnostic reports original
instructions changing a requested byte; it changes no comparison acceptance.

The shared playable-runtime test supplies Return at host ticks 11000, 11020
and 11040 as needed, then holds Space at 12000..12100 and 13000..13100. It
checks ammunition and player-log changes against their pre-fire baseline,
preserving the disk's nonzero existing counters. Missile captures include
launch, lifetime ranges, lifetime 1 before removal and inactive records.

| Probe | Source stores demonstrated | Actual input/stage intervals | Actual bodies |
| --- | --- | ---: | ---: |
| One Return, weapon high nibble $30 | Low stock nibble loses 2; log +$3E gains 2 | 44 | 38 |
| Two Returns, high nibble $20 | High stock nibble loses 2; log +$42 gains 2 | 46 | 38 |
| Three Returns, high nibble $10 | Gun ammunition loses 57; log +$3A gains 57 | 48 | 36 |

All 138 C0F3C4/C0F5F8 intervals and 112 C0EFEA/C0F3C0 bodies match compared
original RAM/display with zero differences. The native state supplies only
comparison entry inputs; original instructions independently produce expected
output. Existing stack/native scratch/asynchronous voice/blitter exclusions
remain unchanged. No HUD/display mask or new RAM exclusion was added. These
are sampled parent comparisons, not a complete original gameplay replay.

```powershell
python tools/native/check_mode_two.py --mode 8 --weapon 1 --out build/native-flight/weapons/1-fixed
python tools/native/check_mode_two.py --mode 8 --weapon 2 --out build/native-flight/weapons/2-fixed
python tools/native/check_mode_two.py --mode 8 --weapon 3 --out build/native-flight/weapons/three-fixed
build/recomp/native_model_oracle.exe build/native-flight/mission-eight/end.dat --tails-only
build/recomp/native_model_oracle.exe build/native-flight/mission-eight/end.dat --points-only
```

The three default setup-model comparisons pass with both new component scopes.
Countermeasure regression passes its input and control-effect parents plus
four actual complete frame bodies, preserving the other callers of the changed
control renderer. The reference MSVC runner builds and all 12 affected
host/loading tests pass. Native and reference builds remain distinct; the
native link still omits emulation objects.

All 21 affected native integration tests pass in 249.36 seconds: numbered
mission entries, ejection, all three firing probes, countermeasures, scene
exit/expiry, frame-body paths, frontend/menu/input and cockpit assets.

Artifacts are under `build/native-flight/weapons/`: accepted captures and
entry/body logs in `1-fixed`, `2-fixed` and `three-fixed`, `runner-stats.json`,
actual runner end states, plotting trace, setup/countermeasure and regression
logs. The earlier `3` directory retains the failed one-pixel comparison; it
is not accepted evidence. Exact audio fidelity, complete outcomes, typed-state
migration, whole gameplay sequence acceptance and the 20 ms presentation target
remain open. The complete-port goal remains active.
