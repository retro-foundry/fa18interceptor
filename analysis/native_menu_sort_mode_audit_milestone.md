# Native menu sort mode audit - 2026-10-09

Actual Demonstration entry now sorts all three display lists, matching its
original caller. Qualification entry also passes the caller comparison. Initial
startup before frontend observers attach remains unverified.

## Connected change and original evidence

The active Release/Debug build uses `port/native/CMakeLists.txt` under
`build/native-cmake`. The playable call chain is `native_frontend_tick` ->
`native_flight_tick` -> delayed menu transition -> `transition_child`'s
`MENU_REFRESH` -> `refresh_native_context_sort` -> retained display-list sorting.
The change removes an incorrect request-only sorting decision on Demonstration's
C0FECE path; it introduces no emulator dependency or new physics rule.

The preceding test selected key 5 for the scenario labelled mode 127. That key
actually starts qualification (mode 9). The fixture now selects key 1 for
Demonstration, adds explicit qualification coverage, records the selected mode,
and rejects a scenario which fails to reach its requested mode.

With actual mode 127, the original C0F3C4/C0F5F8 caller comparison initially found
22 RAM differences, although the retained sort word was zero on both sides.
Original tracing showed saved A4 `$C47584`, whose byte `$75` supplies C1E48C's
nonzero all-list decision. Adding mode 127 to the existing caller-specific
all-list decision fixes those differences. The ordinary frame's request-derived
decision is unchanged. No RAM exclusion or comparison mask was added.

## Validation

Ten requested modes (2, 3 through 9, 125 and 127) pass in Release and Debug:
12 input/stage intervals, 30 sorted lists and 20 original-only incoming-factor
probes. Both builds agree on every interval record and before/after RAM hash.
The one-bit wrong retained-word negative control remains rejected. Qualification
and Demonstration each reach three lists with no cached-depth or fixed-far
entries. A retained-output check alone would have missed the original fault;
the existing complete compared-RAM check detects it.

Release frontend, demo, artifact-policy and cleanup CTests pass (50.11 seconds).
Debug frontend, demo and cleanup CTests pass (92.94 seconds). Both focused
context-sort oracle runs pass. The canonical Release executable is refreshed.
Passing RAM remains temporary; the failed mode-127 pair is retained for diagnosis.
The existing capture and 4 GiB build-cache limits remain enabled.

These oracle runs start original instructions from observed native before-states.
They validate the reached caller contracts and the connected runtime change;
they do not establish independently started original whole-flight parity.
Initial startup and rare callers retain the unknown-factor TODO. Full-flight
comparisons, uninterrupted campaign qualification, original recorded sound,
visible performance and remaining named-state ownership are still outstanding.
The complete-port goal remains active.

Evidence: `analysis/figures/native_menu_sort_mode_audit_checkpoint.json`.

```powershell
cmake --build build/native-cmake --config Release --target fa18_native fa18_native_context_sort_test
python tools/native/check_context_sort.py --out build/native-flight/context-sort-mode-fixed
cmake --build build/native-cmake --config Debug --target fa18_native fa18_native_context_sort_test
python tools/native/check_context_sort.py --test build/native-cmake/native/Debug/fa18_native_context_sort_test.exe --out build/native-flight/context-sort-mode-debug
```
