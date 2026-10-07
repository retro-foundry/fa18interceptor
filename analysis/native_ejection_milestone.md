# Native ejection and action-record rendering

The normal mode-8 menu/flight route now accepts Shift-E at host tick 12000,
executes its original ejection action and returns to the main menu by tick
18000. It reaches 739 scene/HUD frames, ending in mode 0 at C0FCB4. This verifies
one actual ejection/failure route; successful mission outcomes, combat and
whole-game acceptance remain open.

The runtime caller is `native_frontend_tick` -> `native_flight_tick` ->
`native_input_process` -> native command selection/dispatch -> COMMAND_EJECT.
The initial actual input aborted at FLIGHT_EJECT_TOGGLE. Native composition
now calls existing C1C214 `publish_context_toggle_command` with BAR_E_FLAG and
preserves its queue translation result. The domain API returns that byte;
the original CPU adapter still uses its unchanged publication observer.
The parent continues its existing command stores/publication, including the
source modifier and origin gates. 64 additional complete C0F3C4 parents cover
modifier 0/1/2/$FF, origin gate, flag 0/1/$80/$FF and already-claimed queue.
The full input oracle passes 912 cases, retaining its earlier 848 cases.

The subsequent native record loop reaches C230E8/C23116, exposing C23186's
programmed sound and C236AA's manoeuvre setup. The sound reads the original
nine-word table at C23174, sign extends each argument like MOVEM.W, and calls
`play_programmed_sound`. The selector owns its returned zero. The manoeuvre
calls the existing readable clone/motion implementation with native children
for C1BEE8 view publication, C2DEE0 matrix product/angle extraction and C2D954
record orientation. Game records, source coefficient tables and word arithmetic
remain authoritative; no captured flight state supplies runtime behavior.

Action rendering reaches C22B1A, which fixes its parameter base at C3C6E0 and
selects the primary/auxiliary stream from the signed +$4C lifetime: negative,
zero, 1/2, 3/4 or at least 5. Every arm joins the existing C1ED4C descriptor.
Its draw-directory entries $E0 and $100 identify original C0D524/C0D61C.
Readable vertex-tail functions reproduce their two-bank word arithmetic:
eight reflected points at +$5A..+$84 and six midpoint/displacement points at
+$8A..+$A8. Each completes the shown record's signed/wrapped +$A4 bank before
WORKSPACES, consuming no stream operand and returning the source zero result.

64 complete original parents per new geometry operation match result and all
non-stack RAM/display, including wrapped coordinates, signed/wrapped indices,
identical banks and overlapping banks. Together with the existing C21FA4 cases,
the explicit component invocation passes 192 cases:

```powershell
python scripts/build_recomp.py --output build/recomp/native_model_oracle.exe --main tools/native/native_model_oracle.c
build/recomp/native_model_oracle.exe build/native-flight/mission-eight/end.dat --tails-only
```

`--tails-only` names this component scope explicitly. The default model oracle
still runs complete placement drivers and still rejects its earlier invalid
mode-7 repeated-expiry fixture. No failed placement-driver case was skipped or
reclassified. Actual ejection frame bodies independently exercise the connected
model path.

The shared-runtime test starts from the original ADF and the normal eligible
saved-pilot fixture, then supplies normal menu and Shift-E events. Captures
cover every pre-clone ejection body and the lifetime branches, plus source
C1104C/C118FC/C11934/C0F920's end/menu stages. The test requires all seven
lifetime ranges to be observed and the actual menu return.

```powershell
python tools/native/check_mode_two.py --mode 8 --eject --out build/native-flight/ejection/original-check
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_ejection$' --output-on-failure
```

46 actual C0F3C4/C0F5F8 input/stage intervals and 43 C0EFEA/C0F3C0 bodies match
compared original RAM/display with zero differences. Existing stack/native
record-local/scratch/asynchronous voice/blitter exclusions remain unchanged;
no HUD mask or new display exclusion was added. These are focused original
instruction comparisons, not a full original replay or complete gameplay
sequence proof. Audio fidelity and the 20 ms frame target remain unverified.

All 18 affected native integration tests pass: numbered mission routes,
ejection, countermeasures, scene exit, record expiry, active/crash/map frame
bodies, frontend/menu, queued input and cockpit assets. The default setup-model
checker passes all three original parent comparisons at C1075A/C10AE6/C10DAE,
including the new 192 component cases. Reference MSVC build and all 12 affected
host/loading tests pass. The native link still omits emulation objects. These
checks preserve existing behavior without accepting the separate incomplete
mode-7 placement-driver fixture.

Artifacts reside in `build/native-flight/ejection/` and
`build/native-flight/mission-eight/`: normal/ejection inputs and end states,
`original-check/captures.json`, entry/body exports and comparison logs, component
builds and regression logs. The shared native runtime owns every integration
test call; no parallel gameplay implementation was added.
