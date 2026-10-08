# Normal-key mission-four escort and result sequence - 2026-10-08

Mission four now completes its escort objective, carrier arrestor landing,
stopped-aircraft result, actual config save, result messages, Escape/menu return
and cold log reload using ordinary keys. All 43 input/stage intervals and 159
sampled bodies match original compared RAM/drawing, including 20 consecutive
hit/expiry bodies and 64 consecutive landing bodies. Completions advance 1 -> 2
and mode-four grade 0 -> 1. No crash reset occurs.

The playable runner consumes the same 3,883 host replay events, reaches the
same menu and saves the same 78 bytes. Gameplay, arithmetic and comparison
masks are unchanged. The host runner adds delivered/pending replay counts to
JSON; `replay_events` remains the separate game-input recording counter.

## Original path and normal inputs

The active path is `native_frontend_tick` -> `native_flight_tick` -> native
records/weapons -> `postflight_scheduler.c:record_mode`. The initial saved
pilot is exactly `region_pilot_fixture.load_region_pilot()`, previously earned
through normal menu reset, carrier qualification and mission-three success.
No flight, objective, damage, contact or result state is seeded.

Record eight's radar hit advances the existing log 0 -> 1 at body 6,434, starts
its original 15-tick destruction lifetime and increments enemy-expiry accounting.
The regional fighter remains active; the pilot's shorter intercept lead and
normal shoot cue avoid the earlier fatal closing pass. At tick 17,346 the
escort (record four) is active and grounded, has a nonzero cell and speed 770
within the original limit 800. Records eight and ten are inactive. The original
five-step countdown admits objective phase FF.

The escort's result camera clears inputs through the original context reset.
The validation pilot releases/represses ordinary controls when phase changes
FF -> one. F9 on the long return maintains separation from the regional fighter;
F5, heading/height control and the original A arrestor command supply the final
carrier approach. Existing modes three/five and `4-mission` retain their inputs.

The wire touchdown at tick 26,834 changes contact $8002 -> $C082 in region $C0.
The aircraft stops; `postflight_player_readiness` admits phase FC at 27,000.
C110A4 performs the real config write at 27,023. Result messages finish at
27,998, ordinary Escape requests the original restart, and C0F946/C0F974/C0F992
return to C0FCB4/menu at 28,037. All saved bytes remain unchanged through this
restart and a fresh frontend open.

## Acceptance and reproduction

The serial CTest `fa18_native_mission_4_sequence` extends the existing mission
checker with the earned log, strict escort conditions, radar hit/expiry window,
carrier wire contact, consecutive landing bodies, completion/grade progression,
one original config write, result callbacks, cold reload and playable replay.
Run it with:

```powershell
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_mission_4_sequence$'
```

The source comparison runs original instructions in a separate process from
native before-states. This proves the sampled parents and bodies for this
normal-input route; it is not an independently executed original whole flight.
Passing RAM is temporary and removed after comparison inside the existing
480 MiB fixture cap. Builds/CTest retain the 4 GiB pruning hooks.
Hashes and checks are in `figures/native_mission_four_sequence_checkpoint.json`.
Debug/Release playable and mission fixture builds pass. Seven selected Release
checks pass: modes three/four/five sequences, the existing mission-four combat
reset, frontend/link gate and artifact policy/cleanup. The Debug mission-four
sequence and policy/cleanup checks pass, with the same input and saved-result
hashes. Build-cache use is 2.00 GiB after pruning.

## Remaining complete-port work

Independent complete-flight parity, successful modes seven/eight, further play
after the result menu, remaining caller contracts, typed state, audio and wider
performance remain open. The user notes mission six may be impossible: verify
its original target/lifetime/proximity rules and actual original outcomes before
requiring a successful flight or identifying a port defect. Preserve source
rules. Feasibility is unverified, and the complete-port goal remains active.

Decompile any remaining instruction translation encountered on the active path,
including required shared tails and child contracts. The comparison inventory's
75 source-only entries are already readable; 84 deferred translations remain
in that build. The native routines exercised here already use readable owners;
this milestone does not claim another routine decompilation or reduce that count.
