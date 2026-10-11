# Complete mission-three message timing — 2026-10-09

The successful independently started mission-three flight now passes the
original message-line rules throughout its recorded flight. At observation
22,309, original shows HDG 150 while native retains ALT 5115. Both advance to
HDG after two changed sampled-second events following acquisition; original
reaches that event at tick 168 and native at tick 185. This difference is
assessed under the existing rendering-cadence policy in
`native_gameplay_acceptance.md`. All strict drawing differences remain reported.

The earlier ground-strip initialization fix remains in place. This batch
changes optional diagnostics and validation, rather than game timers or road
drawing. Broader drawing acceptance remains open.

## Connected evidence

The playable chain is `fa18_native` -> `native_frontend_tick` ->
`native_flight_tick` -> `update_message` -> `native_hud_draw` ->
`draw_message_line`, followed by `begin_main_loop_timers` and its clock polls.
Original C11BFC publishes message state before C322EE; C25312 subsequently
decrements INFO_REQUEST through C25482 on a changed sampled second.

`FA18_TRACE_MESSAGE_FIELDS=1` adds fourteen named read-only spans to the shared
V2 flight trace: shown/code/loaded, message flags/countdown/time/kind/redraws,
notification countdown, threat event/current bits, radar phase, text-always and
player phase. The default trace schema and output are unchanged. Both callers
read host spans directly, without performing emulated bus reads or game writes.
This removes the evidence gap for warning text and colour-cache ownership;
it removes no runtime emulation dependency.

The original replays all 46,452 frames and 27,110 observations from its existing
qualified-pilot start using the independently recorded ordinary controls. Native
enlists its own pilot, earns qualification and reopens its actual saved pilot
before the mission. Original RAM, clocks and pages never initialize native.

Every previous trace field, full record core, page hash and buffer role remains
unchanged across **27,110 original** and **16,062 native** observations. Each
runner's entire final RAM, counters, grade, menu return and saved pilot reproduce
its earlier accepted recording. Release and Debug agree on the entire native
trace and final RAM. Debug reuses the hashed original recording, rather than
repeating the original capture.

## Complete message contract

All 4,967 flight observations at 22,142..27,108 remain compared at verified
original JSR/LINK identities. Two instruction-proven interrupt resumptions are
unchanged duplicate observations; the sequence contains 4,964 transitions after
its initial boundary. No observation is silently omitted.

| Checked full-flight evidence | Original | Native |
| --- | ---: | ---: |
| Real transitions | 4,964 | 4,964 |
| Notification countdown steps | 4,964 | 4,964 |
| Paused HUD/timer cache transitions | 1,362 | 1,362 |
| Active message-kind/cache transitions | 3,602 | 3,602 |
| Newly formatted target-information lines | 590 | 541 |
| Retained target-information lines | 2,773 | 2,822 |
| Information blanking transitions | 6 | 6 |
| Newly copied message entries | 14 | 14 |
| Retained message entries | 219 | 219 |
| Changed sampled-second events | 584 | 246 |

Every elapsed accumulator, request/page, delay, redraw count, drawn entry and
all 26 text bytes follow the original rules. Target numbers are formatted from
the complete selected record and hashed original asset strings, including DIVU
overflow and decimal zero policy. C31722 resets vicinity delay to 24 at the two
actual threat acquisitions; subsequent HUD decrements, blanking and resumption
are checked. C12098's cockpit slide, view redraws and C0F350's periodic redraw
are included. Both C110A4 callback periods and long POST_INPUT_AUX=0 intervals
retain the entire message cache and timer state.

Message-kind publication precedes target-information colouring. All kind,
flags and kind-redraw results are predicted separately, including the brief
independent colour-cache differences around 25,698. The selected record,
threat events and cockpit flags are observed game inputs; their equality is
also established by the complete independent comparison.

Nine priority-message producer fields match exactly at **all 4,967** original
observation identities: notification countdown, code, loaded/shown entries,
message countdown/time, cockpit flags, threat events and player phase. The
initial uninterrupted page interval agrees at five equivalent countdown events:
ALT/0, HDG/2, SPD/4, ALT/6 and HDG/8. Actual timestamps and text remain retained;
no clock multiplier, phase adjustment, later-image search or pixel mask is used.

Nine rejection probes detect a premature page, lost second event, wrong fresh
text, wrong paused text, lost vicinity delay, wrong notification step, wrong
message colour, wrong warning text and wrong priority-message code.

## Validation and reproduction

Release/Debug builds and the complete independent runs pass. The existing live
flight-trace checker passes with the optional fields both disabled and enabled,
including direct original/native RAM cross-checks, unchanged traced/untraced
RAM and counters, truncation rejection and the shared capture-budget limit.
The protected legacy `scripts/check_native_build.py` cannot run because its
retired `port/CMakeLists.txt` dependency was deleted; it remains unchanged.

```powershell
python tools/native/check_mission_message_trace.py --runner build/native-cmake/native/Release/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-release --source-updates build/native-flight/patrol-entry-review --out build/native-flight/mission-three-message-trace/Release
python tools/native/check_mission_message_trace.py --runner build/native-cmake/native/Debug/fa18_native.exe --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-debug --source-updates build/native-flight/patrol-entry-review --source-reuse build/native-flight/mission-three-message-trace/Release --out build/native-flight/mission-three-message-trace/Debug
python tools/native/assess_mission_message_timing.py --traces build/native-flight/mission-three-message-trace/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-release --source-updates build/native-flight/patrol-entry-review --out build/native-flight/mission-three-message-timing/Release
```

Use the corresponding Debug trace, baseline and output directories for its
assessment. `figures/native_mission_message_timing_checkpoint.json` preserves
the hashes, event identities, counts and rejection results. Passing RAM is
temporary; reusable traces are compressed and the build pruner remains enabled.

Complete gameplay-state parity remains 4,967 observations / 79,472 cores.
Strict complete drawing remains **287/4,967**. This assessment accepts this
recorded mission's message timing and cache rules, without accepting other
drawing or remaining mission flights. Recorded audio/filter fidelity and
visible gameplay performance also remain open. State cleanup is deferred to
the user's separate follow-up; the uninterrupted campaign requirement remains
waived. Canonical Release is refreshed after validation.
