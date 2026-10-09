# Initial startup retained sorting - 2026-10-09

The requested initial startup and qualification-entry sort checks are complete.
No initial startup sorting discrepancy was found. Release and Debug match the
original caller's compared RAM, list visits and retained output. Gameplay and
sorting behavior are unchanged; only optional read-only startup observation was
added to the connected runtime.

## Actual caller and producer contract

The active build is `port/native/CMakeLists.txt` under `build/native-cmake`.
The playable runner calls `native_frontend_open` ->
`native_frontend_open_observed` with no observer -> `native_flight_initialize`
-> `bootstrap_scene` -> `refresh_native_context_sort` ->
`sort_display_list_retained`. Tests use the same open implementation with an
observer attached before the first scene. Ordinary runtime callers retain the
existing open API and do not receive diagnostic callbacks during startup.

The startup boundary surrounds the actual complete C08F26 bootstrap after the
original C08EE4 defaults and C08EB8 saved-level load. The external oracle starts
at C0F812, executes its real `LINK A6,-14` and complete C08F26 call, and stops at
C0F81C before unrelated text setup. It does not manufacture a standalone sort
frame. Observed A6 is C7FEFC; C1C98A writes the caller's all-list choice as one,
and C1E48C reads one while traversing all three lists.

The initial FF refresh request rebuilds templates. C1DCC8 replaces incoming D3
before C1E328; fresh list depths replace it again before the retained-output
boundary C1E4A6. Original-only incoming factors A9F01357 and FFFF2468 therefore
leave every compared byte and retained output unchanged. Native code receives
neither probe values nor original results. The original templates/distance
producers justify the absent-versus-produced factor distinction; the old startup
unknown-factor TODO is resolved for the requested callers.

## Verification

Ten ordinary-menu scenarios each capture fresh initial startup and their later
menu/mission transition. Earned saves supply five distinct actual startup
before-states. Every startup reaches three lists, zero cached-depth entries,
zero fixed-far entries and retained word zero. Qualification entry separately
reaches three lists with matching RAM and retained output. Demonstration, normal
mission entry, Free Flight setup and the existing restart path remain checked.

| Per configuration | Intervals | Sorted lists | Original-only factor probes |
| --- | ---: | ---: | ---: |
| Initial startup | 10 | 30 | 20 |
| Later menu/mission/restart | 12 | 30 | 20 |
| Total | 22 | 60 | 40 |

Release and Debug agree exactly on all 22 interval records and before/after RAM
hashes. The wrong retained-output negative control is rejected. Coverage checks
also require later mission/menu sorting independently of the added startup
visits: a copy of the actual report with later list visits removed is rejected.
This prevents initial startup from hiding a regression in older coverage.

The compared-RAM exclusions and stack boundary are unchanged. These comparisons
run original instructions from real native before-states; they do not establish
independently started original complete flights. No new decompilation or broader
emulation-removal claim is made.

Four regression CTests pass in each configuration: frontend, disk-backed
Demonstration, artifact policy and cleanup. Those checks exercise the normal
unobserved frontend API. Release takes 36.79 seconds and Debug 54.34 seconds for
that set. Both focused context-sort runs pass; the final coverage assertions
were also revalidated against both captured reports. Canonical Release matches
the tested executable hash.

Passing RAM is temporary and removed; the largest native case has four pairs,
8 MiB. The existing capture and 4 GiB build-cache limits remain enabled. Final
cache use is 2.24 GiB. Evidence is sealed in
`figures/native_initial_startup_sort_checkpoint.json`; traces/reports remain in
`build/native-flight/context-sort-initial-startup-release/` and its Debug sibling.

Independent original whole flights/HUD timing, original recorded sound/filter
fidelity, visible performance/presentation and remaining named C state ownership
remain open. Repeated individual mission qualification already satisfies the
user's revised campaign criterion. The complete-port goal remains active.
