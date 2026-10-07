# Native source configuration owners

2026-10-07. Active build: `port/recomp/CMakeLists.txt` includes
`port/native/CMakeLists.txt`. This advances the complete-port goal; full
gameplay, audio and performance acceptance remain open.

## Runtime connections

The former `native_frontend_save_log` directly wrote 78 bytes and cleared the
changed flag. It skipped C1643A's source decisions. Native startup also left
MENU_FILE_READY and the C162E4 load-result word unset.

Actual startup caller: `native_frontend_key` / menu return ->
`native_frontend_enlist` -> `native_frontend_refresh_log` -> C162E4 -> C0EF08,
C16386 and, when the source load fails, C1631C. Actual result caller:
`native_flight_tick` -> C110A4 -> `native_frontend_save_log` -> C1643A -> C0EF08
and DOS Open/Write/Close. The callsign and pilot-log save callers share this
same connected owner.

`port/game/native/files.c` composes the existing domain owners. Game decisions
remain in `postflight_file_callers.c` and `menu_followup.c`; file/lock/info and
allocation operations reuse `port/amiga/host_compat.c`. The frontend retains its
validated read-only OFS mount and owns the backend for its lifetime. Config
writes use the writable overlay, including the original MODE_OLDFILE=1005
write-in-place contract. Native file-info storage has a separate 64-byte bank
at $4A000, outside display, recorder and audio allocations.

C0EF08, C16386 and C1631C now expose ordinary-local native entries while
retaining their original frame/observer entries for CPU adapters. C1643A's
helper and phases are correctly named Write: C539F4 calls DOS offset 48. The
old `load_menu_mode_file`/read labels were misleading. The source status and
readiness gates, changed-flag timing, failed-open readiness clear, and rejection
of only Write=-1 remain authoritative.

## Evidence

The actual shared-runtime postflight fixtures still match **nine** input/stage
intervals and **25** sampled bodies against original RAM/display. The reference
now executes complete C1643A and C0EF08; **no game routine is bypassed**. Only
actual OS services use the host file backend. Exactly one ready-player interval
reaches DOS Write. Existing RAM/display masks are unchanged.

**77** result/restart/config-parent cases match original RAM and persisted
bytes. The new direct-owner cases cover initial nonzero status, readiness zero,
missing filename/open failure, rejected writes, negative/BAD volume tags,
normal startup refresh and source-defined file creation after a missing load.
The source instructions for C0EF08/C162E4/C1631C/C16386/C1643A execute fully.

The preserved CPU adapter paths pass **512** complete CPU/SR/PC/RAM and
child-entry contract cases for each of those five owners: **2,560** total.
These are component/ABI checks, separate from native runtime integration.

Normal-input mode-four flight still matches **56** intervals and **47** sampled
bodies. Reusing the retained original carrier input and success export, native
qualification completes, persists success, restarts and reloads the log. Its
existing final-state comparison is bounded; whole-sequence state/audio/display
acceptance is not established by this functional result.

All **28** selected native tests pass after the model-probe correction below;
the suite initially passed 27 and failed its qualification probe, then the
affected qualification test passed on rerun. **12** reference host/loading
checks and three default setup-model comparisons pass. Both reference targets
rebuild. The native omission/map check in the frontend test passes.

Evidence directory: `build/native-flight/config-owner/`. Repeat connected
postflight comparisons with `python tools/native/check_postflight_schedule.py`.

## Model-probe correction

The qualification probe had been applying aircraft destruction/repeated-life
fixtures to the carrier's $20 record class. A repeated forced lifetime reached
an invalid stream in both original and native rendering. The previous public
executable fails the same probe. Old/new qualification snapshots differ in only
14 bytes, all file-info/readiness/load-result metadata.

The model oracle retains unmodified carrier descriptor comparisons and excludes
the $20 class from its aircraft expiry fixtures. Aircraft coverage can be
required explicitly. A disk-started demonstration snapshot at iteration 2364,
with validation-only selection of actual aircraft record 10, passes **five**
expiry/selection/repeated-render cases and **11** descriptor comparisons. Its
positions, descriptor and render inputs are retained from the native startup.
No renderer behavior or pixel exclusions were changed to pass this probe.

```powershell
build/recomp/native_model_oracle.exe build/native-flight/config-owner/demo-aircraft.dat --aircraft-record 10
```

This corrects a validation scope error; it does not accept normally played ship
destruction or aircraft destruction across all inputs.

## Published build and remaining work

`build/native/fa18_native.exe` is the validated Release executable, SHA256
`13FDD3C7F1C4F20FFA2296A89A878E368A188B8049D6EFC7C3F1D04D5D5E0D34`.
Protected scripts/allowlist, sealed recordings and `.vscode/` are unchanged.

Normal-input mission successes beyond the existing bounded functional
scenarios, remaining combat/results, complete independent game sequences,
audio fidelity and the 20 ms presentation target remain unfinished. The active
goal stays active.
