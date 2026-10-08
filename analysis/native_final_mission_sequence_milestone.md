# Final Carrier Sub mission result and wrap - 2026-10-08

The final mission (F6, internal mode eight) completes through ordinary flight
controls: four counted patrol aircraft destructions, carrier wire landing,
stopped-aircraft admission, saved result, result messages, Escape/menu return,
cold reload and Next Mission wrapping to mode three. The class-20 surface
record (slot 14) remains active at objective admission; no explosion is required by
the original counter scheduler. This verifies the relevant behavior behind
the historical account in `native_final_mission_completion_context.md`.

The initial 78-byte pilot is the established availability-only fixture
`tools/native/fixtures/final-mission-eligible-pilot.json`. Its final-mission
availability is **unearned**. It supplies no flight state, damage, objective,
contact or result. Earned availability and an independently executed original
complete flight remain open; this is not whole-port acceptance.

## Connected original contract correction

The playable caller is `native_frontend_tick` -> native flight -> record update
-> `RECORD_UPDATE_FINISH` -> `schedule_postflight(POSTFLIGHT_DISPATCH)` ->
`SCHEDULE_OTHER` -> C0A364 / `generic_mode`. C230B0 returns the incoming selected
record word, even if it releases that selection. C09E30 replaces only its low
byte with the mission mode; C0A370 replaces only the low byte with the quota.
C0A3A6 publishes the retained word to C458C0 / `PHASE_WORD`.

The existing native adapter discarded this returned selection word. The new
successful flight exposed one compared RAM byte at objective body 8,938:
original C458C0 was FF, native was 00. With selection FFFF and quota four, the
original publishes FF04, rather than 0004. `native/records.c` now carries the
returned selection to the generic handler; `postflight_scheduler.c` also takes
the selection child's returned event. The complete original body now agrees,
including this word. Original countdowns, outcome predicates and comparison
masks are preserved. The exercised routines already have readable owners;
no remaining translation was encountered or newly decompiled in this batch.

Source authority is the original disk plus the byte-exact observed files
`initialize_c09e06_record_update_gate.asm`, `dispatch_c09e3c_mode_scheduler.asm`,
`gate_c230b0_record_limit.asm` and `prepare_selected_record_scheduler_state.asm`.
The C25B66/C25BEE aircraft expiry predicates and original scene-table quota
ownership are documented in the preceding final-mission context note.

## Flight and result evidence

The first three aircraft use the previously accepted radar/radar/infrared
route. The validation pilot then gains separation, makes two gun hits and
finishes the fourth aircraft with an infrared missile. These are read-only
flight observations and ordinary key choices in a test executable; the pilot
is never linked into `fa18_native`.

| Milestone | Body / host tick | Observed original result |
| --- | --- | --- |
| Aircraft slot 10 expiry | 6,368 / 13,477 | Counter 0 -> 1 |
| Aircraft slot 12 expiry | 6,773 / 14,844 | Counter 1 -> 2 |
| Aircraft slot 8 expiry | 8,028 / 19,080 | Counter 2 -> 3 |
| Respawned slot 12 expiry | 8,937 / 22,147 | Counter 3 -> 4 |
| Final objective | 8,938 / 22,174 | Phase FF, sequence three, quota four |
| Carrier touchdown | 12,150 / 32,268 | Contact 8002 -> C082, region C0 |
| Stopped-aircraft admission | 12,199 / 32,433 | Speed zero, phase FC |
| Actual config save | 32,456 | Completions 3 -> 4, mode-eight grade 0 -> 1 |
| Escape/menu return | 33,466 | C0FCB4, phase zero, saved log unchanged |
| Cold Next Mission | 3,004 in new session | Saved mode eight selects mode three |

All 42 flight input/stage intervals and 239 sampled bodies match original
compared RAM/drawing. There are 101 combat bodies: each of the six hit events
has a full 20-body window, with overlap between the consecutive gun hits.
All four aircraft expiries and the FF objective are sampled. Landing includes
64 consecutive bodies; the original file owners perform exactly one config
write. A further input/stage interval and body compare the cold mission wrap.

The playable executable delivers the same 2,626 flight host events, saves the
same 78 bytes and reaches the same result menu without queued input or a crash
reset. A separate four-event cold replay selects Next Mission and reaches
C0FECE/mode-intro in mode three. Cold load checks all 78 saved bytes before
input; entering the menu then performs the original enlistment acknowledgement,
incrementing RAM word +4 from two to three. All other loaded result bytes and
the saved file remain unchanged.

## Bounded acceptance gate

Run the serial `fa18_native_mission_8_sequence` gate:

```powershell
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_mission_8_sequence$'
```

Two identical normal-input flights split capture storage at host tick 23,000.
The first compares 30 intervals/164 bodies, removes passing RAM, then the
second compares 13/76 including cold wrap. Each stays within the existing
240-pair / 480 MiB fixture cap; inputs, saved bytes, result and wrap summaries
must agree across flights. No retention budget or comparison exclusion is
increased. Failures retain the failing case only. Builds and CTest keep the
4 GiB artifact pruning hooks.

Original instructions run in a separate process from native before-states.
This establishes the sampled original parent/body contracts and connected
playable result, not independent original full-flight parity. Build/check
hashes are recorded in `figures/native_final_mission_sequence_checkpoint.json`.
The three-aircraft diagnostic remains a separate accepted partial-route gate.
Debug/Release playable and fixture builds pass. Eight Release checks pass:
modes three/four/five/eight sequences, the partial patrol diagnostic, frontend
and artifact policy/cleanup. The Debug final sequence and policy/cleanup checks
pass; inputs, counters, saved bytes and cold wrap agree across configurations.
The refreshed canonical executable matches the accepted Release runner hash.
Build-cache use remains 2.00 GiB after pruning, within the 4 GiB budget.

Remaining complete-port work includes earned final availability, successful
internal modes six/seven, independent complete flights, remaining caller
contracts and decompilations, typed state, audio and wider performance audits.
