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
