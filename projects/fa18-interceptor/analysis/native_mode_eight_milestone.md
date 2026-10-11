# Native normal mission 8

Mission-list F6 now runs source mode 8 through briefing/context/aircraft setup
and sustained flight: 2,458 scene/HUD frames by host tick 18000, ending at
C10DAE with recorder mode 0 and source aircraft selector $11. Complete mission
outcomes, combat and whole-game acceptance remain unfinished.

The actual caller is `native_frontend_tick` -> `native_flight_tick` ->
`run_post_input_tick` -> C0FECE for setup. During flight the native record loop
calls `schedule_postflight(POSTFLIGHT_DISPATCH)` -> SCHEDULE_OTHER -> the existing
readable C0A364 `POSTFLIGHT_MODE_OTHER`. The first actual eligible input run
aborted at that missing native child. Native composition now calls the domain
implementation with the source selected-mode event and recursive scheduler
hooks. No CPU adapter, gameplay fallback or new mission rule was added.

The original 78-byte disk config has qualification word 1 but availability byte
$19 zero. C1BC72's source gate reads MODE_TABLE+$12+mode-1, so the original pilot
still rejects F6. A standalone fixture extracted from the original ADF changes
only byte 25 from 0 to 1. The shared-runtime test creates the equivalent fixture
through the existing save API, closes and reopens through the normal loader
before any gameplay input. This fixture exists only in validation. Menu checks
exercise locked/eligible F5 and F6 and retain unqualified F1 rejection.

Source transition tracing records A4=C29702 before C1E328. Its saved byte yields
$97 at C1E48C's A6-$2C test, forcing all-list sorting in C0FECE regardless of
requests. Native composition preserves that source choice. Ordinary frame
refresh retains its request-derived local. The original first comparison's
differences disappear with this connected source sorting path.

Normal keys are Space at 1800, digit 6 at 3000, F6 at 4500 and Return at 6500
and 8000, with releases two ticks later. The shared runtime captures stage
changes and flight after 128, 512 and 2,000 scene frames. Snapshots feed only
the original-instruction comparison tool; native behavior still starts from
the disk and inputs.

```powershell
python tools/native/check_mode_two.py --mode 8 --out build/native-flight/mission-eight/original-check
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mode_eight$' --output-on-failure
```

38 actual C0F3C4/C0F5F8 input/stage intervals and 27 C0EFEA/C0F3C0 bodies match
compared original RAM/display with zero differences. Existing oracle exclusions
remain unchanged: original stack, documented native record locals, scratch and
asynchronous voice/blitter polling. No HUD mask or drawing exclusion was added.
This demonstrates sampled source behavior and actual runtime integration, not
complete mode-8 outcome or whole-sequence parity. Audio fidelity and the 20 ms
frame target remain unverified. No full original replay was repeated.

Eleven affected native integration tests pass: modes 2/3 (both aircraft)/4/5/6/
7/8/125, current frontend and menu checks. The frontend confirms original static
pixels, save/reload, SDL presentation and native link omission. The menu checks
include both locked and eligible F6 without changing the ADF. Shared original
domain code was not changed, so no reference rebuild was required for this
native composition batch.

Retained artifacts are in `build/native-flight/mission-eight/`: extracted config,
normal input/end state, original transition trace, `original-check/captures.json`,
input/stage and frame-body comparisons, and regression logs. The delivered
`build/native/fa18_native.exe` includes this batch. All numbered mission entries
now have connected sampled startup/flight routes; mission completion, combat,
remaining explicit native child boundaries and typed-state migration remain.
