# Earned pilot progression and restored region coverage

2026-10-08. The normal menu reset/callsign flow, carrier qualification and
mission-three success/save/menu return earn a level-zero pilot log that unlocks
mission four. The region-flight fixture now loads that exact 78-byte log through
the ordinary saved-file path before gameplay. It retains all flight keys,
5,000-frame, spawn, zone-exit, missile-slot-thirteen and reset-stage assertions.

## Source admission and scenario selection

`frontend_open` -> `native_flight_initialize` -> `load_saved_scene_level`
implements C08EB8, loading the saved level's low byte. C28722's scene dispatch
selects the original streams and publishes the admission count. C28996 ->
C28B16 -> C28B34 uses a signed comparison of C458AA/C458A9 at C28BC2/C28BC8.
All three mode-four variant rows admit two aircraft. With the original disk's
level-two log, two initial aircraft fill those slots, so region spawning is
correctly rejected. RAM snapshots at ticks 11,500, 18,500, 26,000 and 30,000
retain admitted=created=2. Steering changes alone cannot remove that rejection.
The earlier passing region scenario preceded the connected cold-level loader;
its default level zero was not the original disk pilot's startup state.

The new fixture is earned without changing flight records, counters, eligibility
bytes or outcomes. Normal pilot-log SHIFT-2/1 resets and saves the log. Callsign
NEW establishes a first tour. The existing qualification replay earns qualification.
Mission three earns its first grade/completion, persists it, finishes the result
messages and returns to the menu. Its saved level remains zero and its first
mission grade enables F2 through the original availability gate.

`tools/native/fixtures/region-flight-pilot.json` retains the exact bytes, source
ADF hash and normalized replay hashes. The earned log SHA256 is
`7bc9e00e8691143313b0c9f3a1739a05799f9b7e49c5c5683bafbf7f0d5e77b4`.
`region-pilot-enlist.e9k`, the existing qualification input and
`region-pilot-mission-three.e9k` reproduce it. No manual pilot byte edit is used.

## Connected evidence

Qualification's 50 input/stage intervals and 174 sampled bodies match original
compared RAM/drawing, including its one real config write, success and restart.
Mission three's 44 intervals and 164 bodies also match, including its real config
write, completion 0 -> 1, grade 0 -> 1, result messages and Escape/menu return.
The playable executable independently consumes the same ordinary inputs and
produces identical saved qualification and mission logs, returning to the menu
at tick 27,979. These are comparisons within the native runtime and against
sampled original before-states, not independent complete original flights.

Region flight then passes its established assertions with 5,737 scene frames:
spawned records 12/13 (mask 3000), record 12's zone exit (1000), and NPC missiles
9/13 (2200). All 57 input/stage intervals and 49 sampled bodies match original
compared RAM/drawing, including C11788/C11830/C11872. There are no config writes;
the loaded saved log remains unchanged. This is coverage/restoration acceptance,
not successful completion of mission four.

The former direct region CTest now uses the existing Python source checker with
temporary capture storage. The new serial progression gate reproduces the earned
fixture, compares both original flight sequences and checks playable saved results.
Each passing capture pair is deleted immediately; phase capture storage is reused
below 480 MiB. Pruning remains enabled at 4 GiB, including after manual runs.

## Host replay integration

The consumed mission-three replay has 1,652 events. The playable entry formerly
rejected every file after event 1,024 as an unsupported row. `port/native/main.c`
now grows its existing event storage with checked allocation/size arithmetic,
reports stream read/close errors and frees storage on every exit. Event parsing,
ordering and keyboard/mouse delivery remain unchanged. The real caller is
`main` -> replay loading -> `deliver_event` -> `native_frontend_event`.
This removes a host recording limit; no game rules or CPU/chipset code changes.

## Validation

Debug and Release playable/affected fixture builds pass. All seven selected
Release CTests pass: region flight, pilot progression, mode two, ordinary mode
four, frontend/link omission, artifact policy and cleanup. Debug region flight,
pilot progression and cleanup pass (3/3). The final Release region/report guard
and cleanup repeat passes (2/2). Both configurations reject an unordered event
after all 1,652 valid replay events, before game startup. All comparisons preserve
the strict original guards/masks. See
`figures/native_region_pilot_progression_checkpoint.json` for results and hashes.

The refreshed canonical/Release playable SHA256 is
`d6c594410c91ac4d534353b644127f03df67f929a85bd94add1fdc9bb192c8d6`.
Passing raw mode-four CTest captures (132 MiB) are removed; cache use is 2.00 GiB.

Reproduce:

```powershell
python tools/native/region_pilot_fixture.py
python tools/native/check_mode_two.py --mode 4 --flight
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_region_(flight|pilot_progression)$' --output-on-failure
```

Mode-four successful flight/landing/save, successful modes six/seven/eight,
independent complete original flights, remaining caller contracts, typed state,
audio fidelity and wider performance remain open. The complete-port goal stays active.
