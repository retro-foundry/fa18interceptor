# Native normal mission 7

Mission-list F5 now enters source mode 7 with an eligible saved pilot and
reaches 2,289 scene/HUD frames by host tick 18000. The route also reaches
C11078/C110A4's result-message event and resumes C10DAE. This is sampled
route evidence; complete mission outcomes, combat and whole-game acceptance
remain open.

The original disk config has qualification word 1 and mode-7 availability
byte zero. C1BC72's existing indexed gate reads MODE_TABLE+$12+mode-1, so
F5 remains locked for that pilot. The standalone normal-input run uses the
disk's 78-byte config extracted without modifying the ADF, changing only
offset $18 to 1. The shared-runtime test creates the same saved-pilot fixture
through the existing save API, closes and reopens via the normal loader before
any game tick or key. The test-only fixture does not enter playable code.
Menu checks cover both locked and eligible F5; qualification rules are intact.

The real entry caller is `native_frontend_tick` -> `native_flight_tick` ->
`run_post_input_tick` -> C0FECE. During flight, the native record loop calls
`schedule_postflight(POSTFLIGHT_DISPATCH)` -> SCHEDULE_SEVEN -> the existing
C0A1E0 implementation. Its reached SCHEDULE_PREPARE_SEVEN child calls readable
C1BEE8 `publish_context_record_command`, preserving record 4's +6 OR +12
word as the event and STREAM_MODE as the index. Native view composition
connects C08324 and the complete C1BA86 redraw/row/queue tail. The existing
view-command owner now shares `finish_view_redraw` rather than copying that
tail's row rules. 256 complete native publication parents match original
non-stack RAM/display across signed/wrapped record offsets, aircraft kinds,
context return, event values and queue gates.

The selected record view exposed command $FC. The original relocated
C1FCE8 directory points to C21FA4. It consumes no operand and completes
five parallelograms in the shown record's +$A4 bank, then WORKSPACES:
origins $00/$12/$2A/$3C/$4E produce points $60/$66/$6C/$72/$78 using
P0+(P2-P1) with word wrapping. `derive_shown_parallelogram_vertices` belongs
to the existing vertex-tail domain and native model composition calls it.
64 complete original parents match result and all non-stack RAM/display,
including wrapped coordinates/indices and overlapping banks.

Original transition tracing records A4=C296DA, yielding $96 at C1E48C's
A6-$2C test. Native C0FECE preserves that all-list sorting choice; ordinary
refresh keeps its request-derived local. The first comparison's 22 RAM
differences disappear with this source choice.

C110A4 exposed a separate four-byte residue from the native private frame at
$48FA..$48FF. Its cursor and saved mode now live in `ResultMessageLocals` for
the native entry, while the original CPU adapter retains frame-backed reloads
through the same result-message implementation. Native composition calls
`prepare_postflight_result`. The qualification-result oracle removes its
older six-byte scratch exclusion and passes 39 complete result/viewport/restart
parents, including mode-4..7 FE/FD/ordinary phase variants and both flag values.
Full success/outcome child coverage still needs additional connected paths.

Normal keys are Space 1800, digit 6 at 3000, F5 at 4500, Return at 6500 and
8000, with releases two ticks later. The shared runtime requires recorder
mode 0, aircraft selector $11, C10DAE and at least 2,000 scene frames. It
samples source stages plus flight after 128, 512 and 2,000 scene frames.
Captures feed only the separate original-instruction comparison process.

```powershell
python tools/native/check_mode_two.py --mode 7 --out build/native-flight/mission-seven/original-check
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mode_seven$' --output-on-failure
```

42 actual C0F3C4/C0F5F8 input/stage intervals and 29 C0EFEA/C0F3C0 bodies
match compared original RAM/display with zero differences. Existing source
stack, native record-local and documented scratch/voice/blitter polling
exclusions remain; none was added and no HUD pixels were masked. The result
oracle's scope became stricter. Audio fidelity and the 20 ms performance target
remain unverified. No full original replay was repeated.

The standard setup model checker passes its three actual runtime checkpoints
and complete descriptor/grid/control/followup comparisons with the new geometry
cases. Direct standalone placement-driver runs on mode-7 end/setup snapshots
fail at draw command $C3 in the controlled repeated-aircraft-expiry variant;
those runs are retained and are not accepted as mode-7 descriptor evidence.
`FA18_MODEL_DESCRIPTOR_TRACE=1` executes and logs the original descriptor first:
the same C22AC0/C39246 variant also fails to return in original code (PC $FA0).
This fixture therefore lacks a valid source rendering context for its repeated
expiry branch. Correcting that driver's setup/coverage remains for follow-up;
no fixture fallback or new renderer behavior was introduced. Actual mode-7 body
comparisons above do cover rendering at
their sampled boundaries; they do not prove every renderer path.

Fifteen affected native integration checks, current frontend, reference build
and twelve host/loading checks pass. The retained carrier qualification input
also completes its 8,038 consumed updates, persists success, restarts and reloads
the log using the new typed result owner. Artifacts reside in `build/native-flight/mission-seven/`:
original/saved config, normal input/end state, `original-check/captures.json`,
entry/body exports and logs, sort trace, publication/model/result checks,
build/frontend/regression logs, qualification regression and the unaccepted
model-driver logs/source diagnostic trace.
The delivered `build/native/fa18_native.exe` includes this batch. Mode 8 is
still gated; source ready-gate, preparation/outcome variants and whole mission
completion remain unfinished.
