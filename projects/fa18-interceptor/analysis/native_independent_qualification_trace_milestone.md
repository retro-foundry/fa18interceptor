# Independent complete qualification flight

2026-10-09. The sealed carrier qualification now has a complete independent
original/native flight comparison through landing, result messages and the
original C0F992 restart callback. Gameplay and Escape handling are unchanged.
Release and Debug produce identical native traces, counters, final RAM and
78-byte saves. See `figures/native_independent_qualification_checkpoint.json`.

## Runtime and authority

The real playable caller remains `port/native/main.c` ->
`native_frontend_tick` -> `native_flight_tick` -> record/scene/HUD/timer owners.
No runtime dependency is removed by this batch: it expands independent evidence
instead of changing gameplay. The original runner uses the sealed menu state,
Kickstart ROM, original instructions and ports off. Native starts from the ADF
and ordinary title/control keys, never source RAM, clocks or pictures.

`captures/native/qual_carrier_success/input.fa18in` and its state/media seals
are preserved. Original consumes 554 actual key edges; native receives those
consumed edges at their recorded loop boundaries, retaining delayed releases
and simultaneous keys. Original completes 8,038 iterations / 12,353 PAL frames.
Its final RAM matches the 2026-09-29 seal exactly:
`b68cb41fce666cc678ffd5e548b83f770d8b6d6eadcebfcace2c5c5a23d66a30`.
Existing historical recordings retain their original gear commands; the user's
gear-up/down direction applies to newly generated flying routes.

## Complete first flight and restart

C10D8A / game tick one establishes loop4690 in both independent games. Through
loop7417, before C0F946, all **2,728 consecutive boundaries** match all
**43,648 complete 164-byte record cores**, and every named camera/control,
mode and target-selection field. Both complete four-plane pages match at
**2,722/2,728 boundaries**. Clock/HUD fields and every strict failure stay in
the report; no pixel mask, fitted clock or similarity alignment is used.

Nine actual callback runs through C0F992 account for source4690..7602 and
native4690..7452: 2,913/2,763 rows and **1,611 consecutive distinct gameplay
states each**, with every state sequence matching. A changed record byte inside
the frozen viewport wait is rejected. That wait has181 original/31 native
iterations; C0F974 starts32/33 PAL ticks after wait entry. Actual timestamps
and row counts remain explicit under the accepted presentation-cadence policy.

Focused live RAM observes the qualification success branch in both games:
PLAYER_PHASE at C45798 is FF at loops6294..6296 and EF at6297..6298, matching
C110A4 / `prepare_postflight_result`. This is distinct from the trace's
`phase` word at C458DE. Initial pilot contexts differ: original qualification
word0, native ADF pilot word1. Original earns qualification; native completes
qualification again and saves its own actual78 bytes. This is not a new-pilot
proof, and original/native entire pilot records are not asserted identical.

## Six message differences assessed

All six drawing failures are captured in bounded windows, with complete page
hashes and every changed byte retained. The two hook-message failures contain
50 differing bytes each; four stall failures contain15 each: **160 bytes in
total**, all in the original 26-character message line at x120..223/y192..196.
Both pages are checked; this localization is not a drawing exclusion.

The native notification counter is one step ahead at the initial flight
boundary and throughout all83 checked boundaries. Both preserve C11B44's
eight-step cadence. C11BFC expires the timed hook message on counter wrap:
native5530/original5531, countdown0 -> FF, clearing the held message. STALL
uses the same flashing C001 entry: native6230/original6231 turn it on at
counter4, and native6234/original6235 turn it off at counter8. The extra
failure after each change is the other cached drawing page. Warning causes
and post-input events agree throughout the checked windows.

The validation-only `native_message_cadence_oracle.c` compares C11B44,
C11BFC and C322EE separately on unmodified observed states. All249 original
and18 native failure-state owner checks match every non-stack RAM byte and
defined text return. This is component evidence, separate from the full native
runner's independent state/page comparison. Idle frames may retain a notification
code when POST_INPUT_AUX skips the message owner; the checker preserves that gate.

Release/Debug agree on all83 captured RAM hashes, message fields,267 owner
checks and four deliberately wrong cadence/countdown/text/result-state
rejections. The six strict picture failures remain explicit. Their observed
expiry/blink phase follows source timers under the existing equivalent-event
policy; message cadence outside these windows is not inferred.

## Reproduction and limits

```powershell
python tools/native/check_recorded_qualification_trace.py --runner build/native-cmake/native/Release/fa18_native.exe --out build/native-flight/recorded-qualification-trace/Release
python tools/native/check_recorded_qualification_trace.py --runner build/native-cmake/native/Debug/fa18_native.exe --out build/native-flight/recorded-qualification-trace/Debug --source-evidence build/native-flight/recorded-qualification-trace/Release
python tools/native/check_qualification_message_cadence.py --runner build/native-cmake/native/Release/fa18_native.exe --traces build/native-flight/recorded-qualification-trace/Release --out build/native-flight/qualification-message-review
```

`--assess-existing` verifies retained full traces/input hashes and reruns the
flight/restart assessment without replay. The message checker reuses compressed
original RAM, verifying it against every original trace core/field/page before
fresh native capture. Passing native RAM is temporary; only six failing native
states and reusable compressed original RAM remain. Capture ranges use the
default512 MiB bound and the build pruner stays enabled. Native executable code
is unchanged, so no rebuild or unrelated CTest rerun was needed.

The source recording ends during second-flight setup at C10C68; native is
already in the second active flight. Native flight-only tracing omits menu
iteration7453. The strict fixed-offset prefix ends before this gap and reports
the first later stage difference at7449, where the games occupy different
positions in the documented wait. The entire compressed traces retain both
recording endpoints. This does not prove the second flight, a menu return,
every arena byte or independent whole flights for all six missions.

Plain Escape keeps original restart behaviour by user direction; the proposed
override remains dropped. Other independent flights/drawing, recorded sound
and filtering, visible performance and named-state cleanup remain open.
