# Native qualification startup and short takeoff

`fa18_native` -> `native_frontend_tick` -> `native_flight_tick` now admits
source mode 9 (main-menu digit 5), alongside Free Flight. The same predicate
connects input queuing, game work, final text and display publication.
Qualification no longer stops at its transition banner.

The original C0FECE mode-nine arm calls C28722 scene selection, C0924A
`reset_scene_context`, C11312 message reset and C082B0 `finish_scene_setup`.
The native consumer binds these existing owners. C0FB70/C0FBB6 then handle
the qualification briefing through their original countdown/message/key gates.
The existing C10C08/C10C68/C10CFE/C10D8A/C10DAE chain enters active flight.
Record completion dispatch now binds C0A2F0's qualification landing schedule;
this preserves its grounded/hook/speed/phase gates, not an invented success rule.

Carrier rendering reaches C207FE (command $84). Its existing flagged-view
helper latches the current model surface, and C1F7FA continues the proper
carrier/cockpit stream. C1FF0A (command $10C) exposed a separate result-contract
bug: its first normal-component product is accumulated by the model owner;
using a boolean produced identical pixels but incorrect followup cache words.
`FaceTestResult` now retains that value separately from the face predicate,
and the native model returns the source signed accumulated word.

`tools/native/check_qualification.py` exercises four fresh runner checkpoints:

| Host PAL tick | Source callback | Demonstrated behavior |
| --- | --- | --- |
| 3300 | C1072E | Scene constructed, carrier pose/aircraft retained |
| 4800 | C0FBB6 | Qualification briefing waits for acknowledgement |
| 5800 | C10DAE | Active flight on carrier, ground flag set, speed zero |
| 7000 | C10DAE | F10 throttle and pullback, ground flag clears, speed/height increase |

At tick 7000 the runner executes 567 record updates and 566 scene/HUD frames.
Three actual checkpoints each pass 20 original non-stack-RAM startup/briefing/
context/landing-schedule cases, complete C12098/C1C63E view/record comparisons,
and reached descriptor/followup rendering comparisons. The rendering oracle
also passes 48 carrier flag/face-result cases (result word, stream and RAM),
48 circle cases and eight hull-tail cases at each checkpoint. Frontend/link,
menu, model and takeoff-strip regressions and all twelve reference CTests pass.

This milestone is complete (100% of qualification startup and short-takeoff
integration). Complete carrier landing, qualification success/failure,
recorded input anchoring and full frame/timing parity remain open. Demo and
other game modes remain unconnected. Physical gameport acquisition, stores,
remaining record/frame children and sample output are still missing. No
substitute game behavior was introduced. No full sealed replay was repeated;
Copper fade remains excluded.

Next replay dependency: `port/recomp/loop_input.h` defines sealed input rows
as `<main-loop iteration> <video frame> ...`; the first number is the delivery
key, and the second is informational. Native E9K frame replay is a different
format. Preserve that distinction when connecting sealed recordings. Their
saved start state remains oracle evidence, not native runtime initialization.
