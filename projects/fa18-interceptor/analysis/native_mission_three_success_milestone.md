# Normal-key mode-three mission success - 2026-10-08

Mode three now completes its original confirmation objective and stopped-aircraft
result using ordinary keyboard input. Release compares **36 input/stage
intervals** and **184 frame bodies** against original instructions, including
all **96 consecutive landing bodies** and the actual result config write.
The completion counter advances **3 -> 4**, mode-three grade **1 -> 2**, and
the native saved pilot log reloads byte-for-byte. No crash reset occurs.

## Connected runtime and source conditions

`fa18_native_mission_success_test` links the existing playable runtime. It opens
the ADF with a fresh save directory and selects the mission list, F1 briefing
and aircraft two through ordinary menu keys. Its validation-only pilot reads
position/matrix/contact fields to choose pitch, roll, rudder and throttle keys.
It never writes flight/outcome RAM or supplies reference state to native play.
The pilot belongs only to the test executable. No runtime dependency or game
behavior is changed by this batch.

The actual caller is `native_frontend_tick` -> `native_flight_tick` -> native
record updates and source C09E06's postflight scheduler. The display variant
classifier produces `PLAYER_FLAGS_F`'s $1E confirmation countdown when an
aircraft-class record is nearby, `SELECTED_RECORD` is nonnegative and
`VIEW_RECORD` is zero. Normal T input supplies the original target-scan request.
The observed selected offset is $1800, aircraft slot 12. The original classifier
checks aircraft category $10 here; it does not require an enemy ownership bit.
The test preserves that actual source behavior.

Confirmation reaches $FF. Mode three produces `PLAYER_PHASE` $FF at body
11,638, host tick 19,285, then continues with phase one. The pilot returns and
lands on terrain before the runway at body 13,453, host tick 24,105. Contact
changes $0000 -> $0080. This is **not a runway touchdown**: the region byte
changes $05 -> $01, so stopping there would not pass the original readiness gate.
The pilot taxis into the original runway polygon, then reduces throttle until
the source speed word is zero. C2B05A's region probe owns record+4 bit two.

C0A3EA's readiness requires the flown prefix bit, grounded contact, the airfield
region bit and zero speed. C09E06 admits phase $FC at body 14,003, host tick
25,961. The real C110A4 input/stage interval at tick 25,984 invokes
`record_postflight_result`/C11350, increments the log completion word at +56,
stores mode three and its previous grade at +6/+7, and advances the mode grade
at +21. C1643A's config owner reaches exactly one original DOS Write; only its
OS boundary uses host file services. The actual 78-byte native config equals
final pilot RAM and a subsequent cold startup consumes precisely those bytes.
The fixture fails unless all these conditions hold.

## Checks and artifact retention

```powershell
python tools/native/check_mission_success.py
ctest --test-dir build/native-cmake -C Debug -R '^fa18_native_mission_three_success$' --output-on-failure
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_(mission_three_success|frontend|artifact_policy)$' --output-on-failure
```

Debug and Release playable/fixture builds pass. The Debug mission gate and
cleanup pass; the four Release checks pass: mission success, frontend, artifact
policy and cleanup. The playable executable stays SHA256
`35228ba51693afc342ae88e94728608c89cd4beb011c98c63e5df21c7c71c0bd`.
The checkpoint is `analysis/figures/native_mission_three_success_checkpoint.json`.
Local reports are in `build/native-flight/mission-success-accepted/` and
`build/native-cmake/native/mission-three-success-check/`.

The observer selects stage changes, periodic bodies, actual confirmation/contact/
phase changes, runway membership and stop admission, plus every landing-window
body. It caps before/after exports at 240 cases (480 MiB); this run uses 220
cases (440 MiB). Captures use temporary storage. Passing RAM is deleted after
comparison; only a failed case is retained. The original oracle builds run
serially, and existing 4 GiB build/CTest pruning remains enabled.

## Limits and next work

Original C0F3C4/C0F5F8 and C0EFEA/C0F3C0 execute on each native before-state in
a separate reference process. Existing masks are unchanged: game globals and
both drawing pages remain compared. The 184-body sample does not compare every
flight body or independently replay the whole original mission. The terrain
landing followed by taxi tests the actual admission conditions, rather than a
direct runway approach/touchdown. Mode-three message completion/restart, other
aircraft, independent complete flights and successful modes four through eight
remain open. Remaining callback contracts, typed game state, audio fidelity and
broader visible-window/combat performance also remain open. The complete-port
goal stays active.
