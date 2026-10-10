# Complete mission-five message timing - 2026-10-10

The independent mission-five flight follows the original message and timer rules
through all **13,775 observations**, including **13,762 real transitions** and
**12 duplicate observations** after the initial boundary. Both original and
native runs pass every elapsed accumulator, sampled-second countdown, target
page, delay/redraw, full text, message-entry and colour-cache check. This accepts
the recorded flight's message timing under the existing cadence policy in
`native_gameplay_acceptance.md`. Complete cockpit drawing and audio acceptance
remain separate.

The connected playable path remains `main -> native_frontend_tick ->
native_flight_tick -> update_message -> native_hud_draw -> draw_message_line`,
followed by `begin_main_loop_timers` and its later clock polls. Original C11B44
owns notification cadence, C11BFC publishes message entries/kind, C322EE formats
or retains the line, and C25312/C25482 decrement information requests on changed
sampled seconds. C3180C initializes an acquired target's information page.
No playable dependency is removed or game behavior changed by this batch.

The shared, existing `FA18_TRACE_MESSAGE_FIELDS=1` diagnostics add fourteen
read-only spans needed for warning and cache ownership. A fresh original replay
preserves **all 75,573 earlier observations**, every core, page hash and common
field, all counters, final RAM and consumed keys. The native replay preserves
**all 45,570 earlier trace observations**, final RAM, every game/voice counter,
actual event-anchor result, saved pilot and third completion. These native rows
include the ordinary qualification/patrol/escort prefix; the complete flight
still maps to the proven original JSR/LINK identities. Reference RAM, clocks,
synthetic progress and fitted offsets never initialize native gameplay.

The capture tool now supports the already-verified ordinary native mission-five
prefix, keeping its mode-three path. This longer original replay uses the
existing 900-second mode-five timeout. The 512 MiB capture budget is unchanged;
the full-prefix optional message trace fits it. Full-prefix drawing-band capture
is rejected before launch because it would exceed that budget; bounded drawing
owner captures remain a separate diagnostic. Both native enlistment and flight
report zero project gameplay heap violations and zero SDL failures.

| Complete checked sequence | Original | Native |
| --- | ---: | ---: |
| Real transitions / notification steps | 13,762 | 13,762 |
| Duplicate observations retained | 12 | 12 |
| Paused HUD/timer cache transitions | 1,129 | 1,129 |
| Active message-kind/cache transitions | 12,633 | 12,633 |
| Fresh target-information lines | 914 | 861 |
| Cached target-information lines | 4,471 | 4,524 |
| Information blanking transitions | 18 | 18 |
| Fresh message entries | 27 | 27 |
| Cached message entries | 7,203 | 7,203 |
| Changed sampled-second events | 1,917 | 855 |

All nine priority-message producer fields match at every flight observation:
notification countdown, code, loaded/shown entries, message countdown/time,
cockpit flags, threat events and player phase. Fresh text is calculated from
the actual complete selected record and hashed original strings, including
original signed/unsigned division and decimal zero policy. Existing cached
text must remain exact until its real refresh branch executes.

Both first information pages appear at observation **61,957**, game tick **161**.
Original first HDG appears at observation **61,969**, tick **173**, while native
first HDG appears at observation **61,977**, tick **181**. Both follow exactly
**two changed sampled-second events before the HUD** on the unchanged selected
target. Both produce `NO SIG   HDG:  081`. Actual accumulated intervals are
**1,622 ms original** and **1,360 ms native**; they remain reported without
normalization. Original refresh decisions depend on changed sampled seconds
and cache state, so equal update counts or a fixed relative delay would not
establish the original rule.

The first diagnostic attempt rejected original observation **62,057** because
the mission-three checker had never covered a selected-record transition from
`1400` to `FFFF`. Original `release_lost_selection` and C3180C's empty-list
return clear selection without acquisition stores to INFO_PAGE/REQUEST/DELAY;
C322EE subsequently blanks and resets the page. The diagnostic now models that
existing source path. All three actual selection-loss events at **62,057**,
**64,264** and **67,770** are checked on both runs. The initial rejection and
preceding assessor source remain retained; this was a missing diagnostic case,
not a playable fault or permission to skip an observation.

Nine negative controls reject wrong notification cadence, a lost sampled-second
event, a premature page, wrong fresh text, changed paused text, wrong message
colour, lost selection page-reset/blanking and a wrong priority producer. The prior complete mission-three message
assessment, including its nine rejection controls, reproduces its earlier
report exactly. On the actual mission-five first-arrest snapshot at observation
74,641, the native C versions of C11B44, C11BFC and C322EE also match original
instructions in all non-stack RAM and defined text return. That is a separate
component check, rather than a claim of full native frame-body integration.

Complete gameplay parity remains **13,775 observations / 220,400 record cores**,
with identical grade, third completion and menu return. Strict complete page
equality remains **8,393/13,775**. This assessment explains message state and
cadence; it does not establish every remaining cockpit pixel's drawing history.
Broader independent mission comparisons and original sound onset/handoff
acceptance remain open. Campaign continuity stays waived and named-state cleanup
stays outside the goal.

Native production SHA-256 remains
`148d58e7d8821b1221ecdf42f1904398d52e32b401480252a78ed39d49b1dac6`.
The [checkpoint](figures/native_mission_five_message_timing_checkpoint.json)
binds capture hashes, source rules, complete results, rejection controls and
actual-owner evidence. Passing raw capture RAM/traces are removed; compressed
reference recordings and rejected cases remain retained. The pruner runs with
the unchanged 4 GiB build budget.

```powershell
python tools/native/check_mission_message_trace.py --runner build/native/fa18_native.exe --source-evidence build/native-flight/original-mission-five-touchdown-approach --native-evidence build/native-flight/independent-mission-five-warm-prefix-v2 --source-updates build/native-flight/original-mission-five-touchdown-updates --native-prefix build/native-flight/recorded-escort-ground-carry --out build/native-flight/mission-five-message-trace
python tools/native/assess_complete_mission_messages.py --traces build/native-flight/mission-five-message-trace --source-evidence build/native-flight/original-mission-five-touchdown-approach --native-evidence build/native-flight/independent-mission-five-warm-prefix-v2 --source-updates build/native-flight/original-mission-five-touchdown-updates --native-prefix build/native-flight/recorded-escort-ground-carry --runner build/native/fa18_native.exe --out build/native-flight/mission-five-message-timing
```

The second command reassesses retained evidence without another game run.
