# Native menu/context retained sorting - 2026-10-08

The reached menu-entry and scene-restart callers now have direct retained-sort
comparisons. No playable game behavior changes. The new gate checks the renderer
owner outside the RAM arena as well as the existing compared game/drawing RAM.

## Connected path and source result

The fixture links the playable `fa18_native_runtime` and starts its frontend
from the ADF. Ordinary keys reach `native_frontend_tick` -> `native_flight_tick`
-> delayed menu transition or scene bootstrap -> `refresh_native_context_sort`
-> `sort_display_list_retained` -> `native_model_retain_result`.

The external oracle executes complete original input/stage callers C0F3C4 and
C0F5F8 from each observed native before-state. C1E4A6 supplies the retained word;
the comparison explicitly checks it against the native renderer owner after
the input/stage interval. When sorting is disabled, the owner stays unchanged.
The prior bootstrap scratch exclusions and stack comparison boundary remain
unchanged. No original output is supplied to native gameplay.

Nine scenarios cover internal modes 2, 3 through 8, Free Flight setup 125 and
Demonstration 127. Modes 5 through 8 load actual earned predecessor pilot saves
through the normal file loader. The gate observes eleven intervals: ten
C0FECE transitions (including the one after restart) and a mode-two C0F992 restart.
They reach 27 sorted lists, with zero cached-depth or fixed-far entries. Free
Flight setup and the mode-two transition after restart have sorting disabled.

The no-refresh menu paths replace their incoming factor with uncached distance
outputs. The restart and Demonstration refresh templates before sorting.
Tracing observes the preceding source writes, including C28DE2 at mode-two
menu entry, C1DCC8 after template refresh, and C1D994 in restart distance work.
Each of the nine sorting intervals also runs twice with original-only incoming
factors `$A9F01357` and `$FFFF2468` at C1C860. All eighteen dependency probes
preserve compared RAM and retained output. A one-bit wrong expected retained
word is rejected while compared RAM still agrees.

## Validation and limits

Release and Debug each pass the context-sort, artifact-policy and cleanup
checks. Both configurations agree on all eleven interval records and RAM
hashes. Release takes 53.01 seconds; Debug takes 114.83 seconds for this gate.

`fa18_native_context_sort` is serial because reference builds share GNU objects.
Passing captures use temporary storage and are deleted; only reports, source
traces and hashes remain. The largest native export contains three pairs,
6 MiB total. Existing capture and 4 GiB build-cache limits remain unchanged;
the final pruner reports 2.10 GiB, with no passing raw RAM remaining.

The comparison is against original instructions starting from native
before-states. It does not establish independent original whole flights.
Initial frontend startup before observers attach, mode nine and other rare
callers remain outside this gate. The startup/menu TODO still applies if one
of those callers reaches cached-only sorting without a producer. No guessed
factor replaces that unknown contract.

The playable binaries and source reconstruction census remain unchanged.
Independent whole flights, uninterrupted single-process tour acceptance,
remaining caller contracts, typed state, original audio fidelity and wider
performance remain open. The complete-port goal stays active.

Evidence: `analysis/figures/native_menu_context_sort_checkpoint.json`.

```powershell
cmake --build build/native-cmake --config Release --target fa18_native_context_sort_test
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_(context_sort|artifact_policy)$' --output-on-failure
cmake --build build/native-cmake --config Debug --target fa18_native_context_sort_test
ctest --test-dir build/native-cmake -C Debug -R '^fa18_native_(context_sort|artifact_policy)$' --output-on-failure
```
