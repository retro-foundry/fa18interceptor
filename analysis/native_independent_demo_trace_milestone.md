# Independent complete recorded demo trace

2026-10-09. Both actual runners now produce bounded read-only JSONL diagnostics
at their C0EFD4 pre-input boundary. This replaces one MiB of retained RAM per
update without supplying native state, replaying clocks or masking HUD pixels.
`port/native/flight_trace.c` reads direct host spans and hashes all eight complete
320x200 drawing planes in each run's draw/display role order. Every byte in all
sixteen A4-byte record cores, named camera/control fields and explicit HUD/clock
fields is retained. Original dimensions come from its ViewPort; native dimensions
come from the real host bitmap. Native has no Amiga ViewPort allocation.

Default budget: 512 MiB, with native RAM captures reserving their portion of the
same budget. Reference tracing requires translated boundaries and ports off.
Successful footers count every row. Truncated/over-budget evidence is rejected.
Passing raw RAM is temporary; retained traces are compressed.

Release/Debug tracing preserves each runner's final RAM and runtime counters.
Live source2452/native2415 RAM independently verifies every core, field and full
plane hash. The prior 363-update result is reproduced: 5,808 matching cores,
all camera/control states matching, and 83 complete drawing matches. Debug's
trace CTest and cleanup pass; frontend, frame-time integration and artifact
checks pass in both builds. Release trace validation was run directly.
Truncation, joint-budget and invalid-reference-mode rejection checks pass.

The whole sealed demo recording is now traced independently: original loads
its sealed menu state and ROM; native starts from ADF and ordinary keys.
Neither receives the other's runtime state. Original consumes exactly the two
sealed selection edges and completes 4,892 iterations / 20,833 PAL frames.

The first flight, from C10D8A through the following C0F946 wait, covers **2,046
consecutive boundaries**: source2180..4225/native2143..4188. All **32,736 complete
record cores** and all named camera/control states match. **1,235/2,046** complete
drawing boundaries match. First HUD difference remains source2452/native2415,
tick273. This is whole first-flight state evidence, not strict picture acceptance.

Fixed loop alignment breaks later in the viewport wait. Source C0F946 executes
185 iterations; native executes31. Both enter C0F974 **33 PAL ticks** after wait
entry. Gameplay state remains unchanged throughout each wait. Ordinal actual
callback runs account for the complete flight and automatic restart: 2,701 source
and 2,487 native boundaries, twelve callback runs and 2,062 consecutive distinct
gameplay states. Every state sequence agrees, including every core byte. Repeated
identical states are counted explicitly. A mutation inside a frozen wait is
rejected. All thirteen callback-entry pairs through the second active-flight stage
match cores/camera/controls; eleven match complete pages. The first thirteen
second-flight boundaries match all208 cores, camera/controls and complete pages.

The complete fixed-offset 2,713-boundary diagnostic still reports failures:
41,592/43,408 matching cores, 2,216 complete core boundaries, 2,211 camera/control
boundaries and 1,273 complete drawing matches. Its first later stage difference
is source4257/native4220: different positions in the documented viewport wait.
This is not evidence of an incorrect aircraft reset.

Reports/compressed traces are in `build/native-flight/flight-trace-check/Release/`,
`build/native-cmake/native/flight-trace-check-Debug/` and
`build/native-flight/recorded-demo-trace/Release/`. Reproduce with
`tools/native/check_flight_trace.py`, `check_recorded_demo_trace.py` and
`compare_flight_traces.py`. `--assess-existing` checks retained traces without
another game run. Runtime CLI: `--flight-trace PATH`; reference environment:
`FA18_LOOP_TRACE=PATH`.

The recording ends during a second active flight. All-mission independent whole
flights and equivalent elapsed-time HUD assessment remain open. Restart evidence
covers named gameplay fields, not every arena byte or strict pictures. Recorded
audio/filter, visible performance and typed-state cleanup remain unfinished.

Latest user report: plain Escape from Free Flight returns briefly to menu then
reselects option two. The earlier return check used Shift+Escape. Investigation
has reproduced the plain-Escape reentry through C0F992/C0FCB4 with mode one.
