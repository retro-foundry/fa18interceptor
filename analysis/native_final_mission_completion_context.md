# Final mission completion context - 2026-10-08

The user's mission-six concern refers to the final Carrier Sub mission.
The earlier handoff incorrectly associated it with the internal `mode_six`
scheduler. That scheduler belongs to Search and Rescue.

## Mission numbering from original data and code

`analysis/data/mission_text_inventory.json` contains the original F1-F6 labels.
`port/game/indexed_commands.c:execute_indexed_command_result` converts function
keys to mission modes by adding three to the zero-based function-key index.
The original qualification and availability guards still apply.

| Menu key / mission | Internal mode | Original mission label |
| --- | --- | --- |
| F1 / 1 | 3 | Visual Confirmation |
| F2 / 2 | 4 | Emergency Defense |
| F3 / 3 | 5 | Intercept Stolen Aircraft |
| F4 / 4 | 6 | Search and Rescue |
| F5 / 5 | 7 | Intercept Incoming Cruise Missile |
| F6 / 6 | 8 | Carrier Sub |

Existing milestone names such as "mission-four sequence" identify internal
mode four (the second menu mission); their accepted evidence is unchanged.

## Historical account supplied by the user

[Wikipedia's gameplay section](https://en.wikipedia.org/wiki/F/A-18_Interceptor#Gameplay)
reports disputed final-mission objectives, attributes possible submarine
destruction without a visible explosion to Bob Dinnerman, and describes patrol
aircraft destruction as sufficient for completion. It also reports menu return
and a wrap to the first mission instead of an ending sequence. These are
historical claims, not independent acceptance evidence for this port.

The article links an [interview with Dinnerman](https://steiny.typepad.com/premise/2004/01/interview_with_.html)
and a [recorded final-mission completion](https://www.youtube.com/watch?v=xdojCc3HUT0).
Neither linked page could be fetched during this check; their contents were
not independently verified. The original code/disk behavior remains authoritative.

## Original scheduler and next verification

`source_amiga/observed/dispatch_c09e3c_mode_scheduler.asm` and
`port/game/postflight_scheduler.c:schedule_postflight` send mode eight to
C0A364 / `POSTFLIGHT_MODE_OTHER`, then `generic_mode` / C0A39E.
Unlike `mode_six`, this branch does not inspect rescue target position/lifetime.
It compares `SCENE_DISPATCH_ADMITTED` against `SCENE_DISPATCH_AUX`, waits for
the original two-step countdown and sequence-phase condition, then publishes
successful objective phase FF. It contains no direct submarine-explosion test.

This local branch supports investigating the aircraft-completion account, but
does not alone prove which objects contribute to those counters or demonstrate
a complete successful final flight. Trace counter initialization, spawn changes
and completion increments in the actual native record path. Then exercise
ordinary-input patrol destruction and compare the original objective, landing,
stopped-aircraft admission, config save and menu return. Verify next-mission
wrap through the original pilot-log selection path. Do not invent a submarine
explosion, weaken counters or mark the final mission impossible from the briefing.

Documentation-only clarification: gameplay, comparisons, saved pilot logs and
the accepted mission-four sequence checkpoint are unchanged. Final-mission
runtime acceptance and independent original complete-flight parity remain open.
