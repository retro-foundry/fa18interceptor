# Carrier qualification consumed input

`carrier_qualification_game_input.fa18in` is the retained original game's
consumed-key export from the sealed `captures/native/qual_carrier_success/`
recording. It contains 554 raw keyboard events over 8,038 game updates; frame
numbers are informational. It is input evidence, not captured flight state.

Original `--ports off --game-input-out` export was retained locally as
`build/native-flight/carrier-game-input.fa18in` during the game-input and
qualification-result milestones. This fixture is an unchanged copy, SHA256
`8d4dff657c006d5161ad4c719604f7bbc2a884977c475e9400942564cabd4340`.
The sealed injected input SHA256 is
`fe4ff3a4cac080a4534041eb42d84a9332916d7566336910d852f86cde070440`;
the start state SHA256 is
`760d729341bebb9d6aa49450e7c7a6b760fd2d35321e9bc1e4c5f09c4015a4f4`.
See `analysis/native_qualification_result_milestone.md` and
`tools/native/check_game_input.py` for original export/alignment scope.

`check_qualification_sequence.py` starts native gameplay from the ADF using
these keys, captures its actual inputs, landing and result/restart boundaries,
and independently executes original instructions on each native before-state.
It never supplies reference RAM to the playable runtime. This comparison is
separate from acceptance of an independent complete original flight replay.

## Earned cruise-missile availability

`rescue-sequence.e9k` contains the 870 ordinary host events from the accepted
Search and Rescue flight (F4/internal mode six). Starting with the unmodified
original ADF pilot, it completes the pod objective, carrier landing, actual
save and Escape/menu return at tick 25,872. The saved mode-six grade admits
the cruise-missile mission (F5/internal mode seven) through the original guard.

`cruise-mission-pilot.json` retains the resulting 78-byte log and normalized
input/ADF hashes. `cruise_pilot_fixture.load_cruise_pilot()` checks these
seals; `fa18_native_mission_6_sequence` reproduces the exact input and saved
bytes with original boundary comparisons and the playable replay. It creates
no flight state. The initial ADF pilot already contains earlier progress;
this fixture proves the rescue-earned availability step, not a complete tour
earned from a newly enlisted pilot. See
`analysis/native_rescue_mission_sequence_milestone.md`.
