# Native next-mission / mode-6 connection

Digit 7 now selects the original pilot log's next mission (mode 6 on the
original ADF) and proceeds through briefing, context selection and aircraft
setup into native flight. A normal disk/input run reaches 401 scene/HUD frames
by host tick 10000, ending in C10DAE. Mission completion, further variants and
whole-game acceptance remain open.

The real caller is `native_frontend_tick` -> `native_flight_tick` ->
`run_post_input_tick` -> C0FECE. The native gate now admits mode 6. The adapter
connects source C103E4/C10418 viewport setup, C10458's key/countdown handling,
C104C2/C105A6/C105F4's briefing and pause, C10626/C1064C's context callbacks,
and C10900/C10942/C10970/C109AC's message, smoothing and setup return.
Existing game functions own all decisions and source-defined timers.

Reached child calls now compose C24FA4's indexed briefing/outcome messages,
C29368's origin-record selection and its source fallback/fault contract,
C17E4A/C17CF6's setup noise/engine choice, and C09E06 -> C0A15C's mode-6
record scheduler. No CPU, translation or chipset object is added to the
native link. Other reached-but-unimplemented children still fail explicitly;
full scheduler outcomes, cancellation cases and mission variants are not
claimed complete.

The first full transition comparison found 22 differing RAM bytes. Original
instruction tracing showed C1E328 entered with A4=C29662; C1E48C then tested
$96 at A6-$2C, inside the saved pointer, despite a zero request mask. The
mode-6 transition therefore sorts every list, like the previously demonstrated
mode-2 transition. Ordinary frame refresh retains its request-derived choice.
The corrected transition matches. The optional `FA18_MODE_STAGE_TRACE`
oracle trace and `transition-sort-trace.log` retain the evidence.

The shared-runtime integration test uses only the ADF and normal keys: Space
at 1800, digit 7 at 3000, Return at 5000 and 6500, releases two ticks later.
It captures stage changes and flight after 128, 256 and 384 scene frames.
Captured state is used only in separate original-instruction comparisons.

```powershell
python tools/native/check_mode_two.py --mode 6 --out build/native-flight/mission-six/original-check
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mode_six$' --output-on-failure
```

38 actual C0F3C4/C0F5F8 input/stage intervals and 27 C0EFEA/C0F3C0 bodies match
all compared original RAM and display bytes. This establishes sampled
component correctness and runtime integration. Existing oracle exclusions
remain unchanged: original stack, native record locals and documented
scratch/asynchronous voice/blitter polling; no HUD masking or new rendering
exclusion is added. Complete sequence parity, outcomes, audio fidelity and
the 20 ms frame target remain unverified. No full original replay was repeated.

Artifacts: `build/native-flight/mission-six/original-check/` contains
`captures.json`, input/stage and frame-body logs, before/after/source exports
and the sort trace. `runtime.log` records the actual executable's final run.
The delivered `build/native/fa18_native.exe` includes this batch.

Mode-2 source regression passes 32 intervals / 21 bodies, and mode 125 passes
55 / 35. Eight affected native integration tests and current frontend pixel,
save/reload, SDL presentation and link-omission checks pass. The menu test now
requires mode 6's source C105A6 briefing at its former banner-only checkpoint;
it checks that both digit 7 and mission-list F4 enter that connected stage.
