# Carrier landing, qualification and restart acceptance — 2026-10-08

Carrier qualification has repeatable source comparisons across takeoff, landing,
successful result, config write and restarted flight. Each of two normal-key
scenarios passes **50 input/stage intervals** and **174 frame bodies**, including
every one of **96 consecutive landing bodies**. The new-pilot case earns
qualification, changes the persisted word **0 -> 1** and reloads the saved log
on a fresh native startup. The ADF-log case exercises repeat qualification
with its existing word one.

## Runtime and source authority

The fixture links the playable `fa18_native_runtime` objects. Startup loads the
ADF; `NativeReplay` delivers all 8,038 recorded updates and 554 consumed raw keys
through the normal input queue. No reference RAM, CPU or chipset model feeds
native gameplay. The new-pilot fixture changes only a saved qualification word,
saves through the existing file owner and reopens the normal loader before
replay. It does not seed flight, contact or outcome state.

The actual `native_frontend_tick` -> `native_flight_tick` path runs player
dynamics and the existing C149BE -> C083E2 landing reset. C09E06's mode-nine
schedule observes carrier contact and a stopped aircraft, producing phase $FF.
The post-input callback reaches C110A4. When its countdown expires it changes
phase $FF -> $EF, sets qualification to one, calls C1643A's config owner and
queues the original success messages. C11958/C119D4 lead through
C0F946/C0F974/C0F992 and scene setup back to C10DAE.

Observed touchdown is update **6,243**, body **4,347**: contact word
$8082 -> $C0C2. Success phase is first set at update **6,287**. The actual result
executes at update **6,296**. The consecutive window runs through body **4,442**,
covering touchdown, stopping, success admission and result. The result interval
executes complete original C1643A/C0EF08 and observes exactly **one DOS Write**;
only actual OS services use the host file backend. The native saved config is
78 bytes, equals final pilot RAM and reloads byte-for-byte. Final flight has no
queued keys or crash resets and reaches C10DAE/mode nine.

No gameplay rule, physics constant, timer or playable executable changed.

## Validation and retention

`tools/native/fixtures/carrier_qualification_game_input.fa18in` is an unchanged
copy of the earlier retained original consumed-input export. Its README records
the export and sealed-recording hashes. It contains key events, not assets or
RAM, so tests no longer depend on a local build-cache input file.

```powershell
python tools/native/check_qualification_sequence.py
python tools/native/check_qualification_sequence.py --new-pilot --out build/native-flight/qualification-new-pilot
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_qualification_(sequence|new_pilot)$' --output-on-failure
```

The observer samples stage changes, periodic bodies, contact/phase changes and
the consecutive landing window. At most 240 before/after cases may be written:
480 MiB, within the default 512 MiB diagnostic budget. RAM lives in temporary
storage; passing files are deleted after each comparison. Only a failed case
is retained. Build/CTest pruning remains enabled with its 4 GiB cache budget.

Debug and Release builds pass. Six affected Release CTests pass: both sequences,
ordinary qualification, ready-player postflight, frontend and artifact cleanup.
Debug passes the original-log and new-pilot sequences plus cleanup in separate
targeted runs. The playable executable remains SHA256
`35228ba51693afc342ae88e94728608c89cd4beb011c98c63e5df21c7c71c0bd`.
The compact checkpoint is `analysis/figures/native_qualification_sequence_checkpoint.json`.
Local logs are in `build/native-flight/carrier-sequence/`.

## Scope and next work

The reference executes original instructions on each native before-state.
Existing masks are unchanged; gameplay globals and both drawing pages remain
compared. All 96 landing bodies are consecutive, but the 174-body set does
**not** compare all 6,142 bodies in the replay. Independent whole-flight state,
recorded cadence, drawing and audio acceptance remain open.

This strengthens the earlier functional qualification milestone. Successful
mission-list modes three through eight, independent complete flights, remaining
callback contracts, typed game state, audio fidelity and visible-window/combat
performance remain open. Continue with normal-input mission success grounded
in the original success conditions; controlled terminal-flag fixtures do not
establish naturally completed missions. The complete-port goal stays active.
