# Final mission with earned availability - 2026-10-08

The active `fa18_native_mission_8_sequence` now starts from the actual pilot
log saved after the accepted rescue and cruise-missile flights. Ordinary keys
earn the prerequisite, complete the final aircraft objective, land on the
carrier, stop, save, finish messages and return to the menu. A cold Next Mission
selection wraps to internal mode three. This replaces the active success gate's
synthetic eligibility dependency; gameplay and comparison masks are unchanged.

## Original ownership and progression

`native_frontend_tick` reaches the existing native command-selection adapter
and `indexed_commands.c:execute_indexed_command_result`. Final mission F6 maps
to internal mode eight; its original availability guard reads saved-log byte
25, the mode-seven grade. The actual cruise save sets that byte to one.
The rescue save first earned mode-seven availability through byte 24.
The existing result path C110A4 -> C11350 updates the log and writes the config.
No flight state or prerequisite grade is injected during these flights.

`rescue-sequence.e9k` reproduces the log in `cruise-mission-pilot.json`.
`cruise-sequence.e9k` reproduces the log in
`final-mission-earned-pilot.json`. The mode-six and mode-seven gates check
exact retained input and all 78 saved bytes. `load_final_pilot()` checks their
hash chain, original ADF identity and prerequisite/result fields before the
final gate loads the saved pilot through the existing loader.

The original ADF pilot already contains earlier progress. This establishes
the rescue -> cruise -> final availability chain, not a complete tour from
a newly enlisted pilot. The earlier availability-only log remains an explicitly
unearned reference for `fa18_native_final_patrol_diagnostic`; the historical
final success checkpoint also retains its original provenance.

## Final outcome and comparisons

The earned log changes final completions from five to six; mode-eight grade
advances from zero to one. Four aircraft expiries satisfy this fixture's quota
four. The objective is admitted at tick 22,174, wire touchdown occurs at 32,268,
the stopped result at 32,433, save at 32,456 and menu return at 33,466.
Class-20 surface record 14 remains active at objective admission. The original
generic scheduler checks aircraft counters and requires no visible submarine
explosion on this path. This supports the user's reported historical context
without independently identifying record 14 as the submarine.

All 42 flight input/stage intervals and 239 sampled bodies, plus one cold-wrap
interval/body, match original compared RAM/drawing. Coverage includes 101 combat
bodies, 64 consecutive landing/result bodies and one actual config write.
The playable runner delivers 2,626 flight host events and four wrap events,
agrees on saved bytes/menu/wrap and has no pending input or crash reset.
Cold wrap preserves result fields and the saved file; its original in-memory
enlistment word advances four to five.

Original instructions run from native before-states. Independent original
whole-flight parity remains open. This batch changes validation and retained
evidence only; no gameplay correction or new decompilation is claimed.

Two identical final flights partition temporary captures inside the unchanged
240-pair / 480 MiB cap, removing passing RAM before the next partition.
Build pruning remains bounded at 4 GiB. Configuration checks and sealed hashes
are recorded in `figures/native_final_mission_earned_availability_checkpoint.json`.

Six selected Release checks pass: rescue, cruise and final sequences, artifact
policy/cleanup and playable frontend/link validation. Four Debug checks pass:
cruise and final sequences plus artifact policy/cleanup. Final inputs, saved
bytes, counters, message/menu sequence and cold wrap agree across configurations.
The canonical Release playable is unchanged; passing RAM is removed and pruned
build-cache use remains 2.00 GiB. The complete-port goal stays active.
