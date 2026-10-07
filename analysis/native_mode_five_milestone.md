# Native normal mission 5

Mission-list F3 now enters normal source mode 5 and reaches C10DAE flight,
with 2,312 scene/HUD frames by host tick 18000. Complete outcomes, the
scheduler's record-restoration branches, combat and whole-game acceptance
remain open.

The real caller is `native_frontend_tick` -> `native_flight_tick` ->
`run_post_input_tick` -> C0FECE for entry, and the native record loop ->
`schedule_postflight(POSTFLIGHT_DISPATCH)` -> SCHEDULE_FIVE during flight.
The mode gate now admits 5 and native record composition calls the existing
readable `schedule_postflight(POSTFLIGHT_MODE_FIVE, 5, record, hooks)`
implementation of C0A002. The normal run first exposed this missing native
scheduler child. Connected source briefing/context adapters handle its reached
setup stages. No CPU adapter or new gameplay rules were added.

Original transition tracing records A4=C2968A before the sort, then $96 at
C1E48C's A6-$2C test. Mode 5 therefore sorts every list in C0FECE, even without
sort requests. Preserving that source saved-pointer choice removes the initial
22 compared RAM differences. Ordinary frame refresh keeps its existing
request-derived local.

The shared playable runtime starts from the ADF and normal keys: Space 1800,
digit 6 at 3000, F3 at 4500, Return at 6500 and 8000. Releases follow two ticks
later. The test requires recorder mode 0, source aircraft selector $11,
C10DAE and at least 2,000 scene frames. It captures stage changes and flight
after 128, 512 and 2,000 scene frames. Captured state feeds only the separate
original-instruction comparison process, never native gameplay.

```powershell
python tools/native/check_mode_two.py --mode 5 --out build/native-flight/mission-five/original-check
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mode_five$' --output-on-failure
```

39 actual C0F3C4/C0F5F8 input/stage intervals and 27 C0EFEA/C0F3C0 bodies
match compared original RAM/display with zero differences. Existing oracle
exclusions remain unchanged: original stack, native record locals and documented
scratch/asynchronous voices/blitter polling. No HUD mask or rendering exclusion
was added. This is sampled route and runtime evidence, not complete outcome or
whole-sequence parity. Audio fidelity and the 20 ms performance target remain
unverified. No full original replay was repeated.

Seven affected native integration tests pass: normal mode 3's two aircraft
choices, modes 4/5/6, menu and cockpit assets. Current frontend original pixels,
save/reload, SDL presentation and native link omission checks pass. Shared
original game implementations were not changed in this batch; native ownership
and loader boundaries remain intact.

Artifacts reside in `build/native-flight/mission-five/`: normal input/end state,
`original-check/captures.json`, entry/body comparisons and exports, original
sort trace, build, frontend and regression logs. The delivered
`build/native/fa18_native.exe` includes this batch. Modes 7 and 8 remain gated;
normal mission completion and combat still need connected source paths.
