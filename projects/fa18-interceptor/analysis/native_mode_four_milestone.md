# Native normal mission 4

Mission-list F2 now enters normal source mode 4 and reaches C10DAE flight,
with 2,507 scene/HUD frames by host tick 18000. Complete outcomes, combat,
other variants and whole-game acceptance remain open.

The real caller is `native_frontend_tick` -> `native_flight_tick` ->
`run_post_input_tick` -> C0FECE for entry, and the native record loop ->
`schedule_postflight(POSTFLIGHT_DISPATCH)` -> SCHEDULE_FOUR during flight.
The mode gate now admits 4 and native record composition calls the existing
readable `schedule_postflight(POSTFLIGHT_MODE_FOUR, 4, record, hooks)`
implementation of C09EC4. No CPU adapter is involved. Reached setup stages
reuse the source briefing/context adapters already connected for mode 6.

The initial C0FECE comparison found 22 RAM differences from its saved-pointer
sort choice. Original tracing records A4=C296E4 before the sort, then $96 at
C1E48C's A6-$2C test. Mode 4 therefore sorts every list during this transition,
even without sort requests. Native composition preserves this source choice;
ordinary frame refresh retains its request-derived local.

The shared playable runtime starts from the ADF and normal keys: Space 1800,
digit 6 at 3000, F2 at 4500, Return at 6500 and 8000. Releases follow two ticks
later. The test requires recorder mode 0, source aircraft selector $11,
C10DAE, and at least 2,000 scene frames. It captures stage changes and flight
after 128, 512 and 2,000 scene frames. Captured state feeds only the separate
original-instruction comparison process, never native gameplay.

```powershell
python tools/native/check_mode_two.py --mode 4 --out build/native-flight/mission-four/original-check
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mode_four$' --output-on-failure
```

39 actual C0F3C4/C0F5F8 input/stage intervals and 27 C0EFEA/C0F3C0 bodies
match compared original RAM/display with zero differences. Existing oracle
exclusions remain unchanged: original stack, native record locals and documented
scratch/asynchronous voices/blitter polling. No HUD mask or rendering exclusion
was added. This is sampled route and runtime evidence, not complete mission
outcome or whole-sequence parity. Audio fidelity and the 20 ms performance
target remain unverified. No full original replay was repeated.

Thirteen affected native integration tests pass, including earlier connected
modes, countermeasures, scene return, record expiry, menu, menu start, samples
and cockpit assets. The broader run exposed a stale menu-start assertion,
which also failed on the preceding committed binary: it expected the initial
voice pointer after sample requests had advanced the voice. That assertion
now verifies membership in the selected source asset chain; the existing
18 complete original menu-parent comparisons still verify exact initial
selection and all non-stack RAM before/after the pause. The focused menu-start
and source sample-request/PCM checks pass after this correction.
Current frontend original pixels, save/reload, SDL presentation and native
link omission checks pass. The shared original game implementation was not
changed in this batch. Native ownership and loader boundaries remain intact.

Artifacts reside in `build/native-flight/mission-four/`: normal input/end state,
`original-check/captures.json`, entry/body comparisons and exports, the original
sort trace, build, frontend, regression and menu/sample correction logs. The delivered
`build/native/fa18_native.exe` includes this batch. Modes 5, 7 and 8 remain
gated; normal mission completion and combat still need connected source paths.
