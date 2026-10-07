# Native indexed input outputs and throttle correction - 2026-10-07

Number keys, indexed selections and function-key throttle commands now expose
their actual assigned selection, level or accumulator. Gates that assign no
value preserve the preceding output. Recorder $FD exposes its relative change
to that preceding selection. These results compose subsequent depleted input
through the same playable native command dispatcher.

The connected caller is native entry -> frontend -> native_input_process ->
native_menu_dispatch_raw/pending -> command dispatch ->
execute_indexed_command_result. The domain owner remains indexed_commands.c;
native/menu.c composes its named output with the preceding actual domain value.
The legacy event API executes the same body. This removes unresolved indexed
outputs from this call chain without preserving CPU registers in native state.

## Original contract

- C1BC66/C1BC72 assign function-key/number-key selections before enable, mode,
  table and pose gates. C1BC78 can replace index zero/one with $11/$10;
  C1BCA8 increments admitted indices before storing COMMAND_MODE_GATE.
- C1BD78-C1BE5E expose actual mode selections, including the table-loaded
  level, its bounded increment and mission availability selection. C3318E
  changes the queue event but preserves that assigned selection.
- C1BE60-C1BEDA's pose scan preserves the assigned index. The cockpit-flag
  path increments it only for recorder zero/two.
- C1BCCE's positive-recorder and other recorder/origin gates preserve output.
  C1BCEE/C1BCF2 applies -$50 then +11 to the inherited selection without
  loading a new event into it. The domain reports that relative change; the
  native caller applies it to its actual prior byte. Unknown prior output
  remains unknown. Successful outer publication still owns its queue index.
- C1BD04-C1BD74 exposes the actual throttle index, F1 toggle result, computed
  byte level, or sign-extended, scaled and clamped accumulator. The event and
  domain output remain distinct, preserving source writes and queue order.

## Connected gaps found by the extended run

The additional frames reached C230E8 -> C23716's release action, which had no
native child composition. native/records.c now calls the existing
initialise_flight_record_release owner. The corresponding C22BBA-C22C6C
drawing path selects the original hunk-58 model stream by record lifetime and
action nibble, updates its flag and calls the existing aircraft descriptor.
Addresses use the playable loader's verified relocation, not synthetic
addresses assigned to unresolved hunks by the standalone disassembler.

Its stream reached command offset $104 -> C0D6C4/C0D6E0. The new
derive_shown_translated_vertices owner in vertex_tail.c writes the two original
translated vertices in both record and transformed banks. native/model.c calls
it from the actual stream interpreter. These paths now complete in the playable
runner and match the original complete-body comparison; all lifetime/model
branches are not independently accepted by this bounded run.

The original comparison then failed at cleanup body 79, after F1's special
negative level request. C1B35A tests the stored byte before C1B35E clamps a
negative request to zero. The native flight recorder had tested it after
clamping, skipping its release behavior. C1B36C also excludes recorder mode 1;
native code incorrectly excluded mode 2. update_flight_input now follows that
original ordering and gate. The same failing body and the complete suite pass
with no added RAM/display exclusions or fitted state.

## Validation and limits

Twelve additional complete Free Flight bodies supply actual cleanup outputs
to twelve intervening indexed commands, then to depleted recorder input.
The run retains earlier fire, radar, view, drawing and input comparisons.
175 full bodies, 192 recorder parents and 72 intervening keyboard parents
match original compared RAM/drawing and defined returns. Original parent
outputs feed only later original parents; native gameplay computes its own.
The fixture restores ordinary readiness before the next controlled recorder
parent after capturing each indexed command. It does not seed command output.
Passing raw RAM remains temporary; an aborted native run now retains its
latest before-capture for diagnosis, and validation stdout is unbuffered.

10,688 selected command parents match compared non-stack RAM and defined
low-byte returns: 2,038 preserve prior output, 1,698 publish selection, 156
publish queue indices, 3,218 publish flight actions, 1,318 publish view actions
and 2,260 publish indexed actions. Zero is unresolved within this bounded suite.
The additional 2,432 parents cover all 256 saved F1 levels under readiness and
phase gates, F1-F10 modifier/recorder/origin gates, all 256 incoming bytes for
two recorder-$FD keys, and number/indexed enable/mode/cockpit/recorder gates.
Some indexed keys deliberately route to existing view actions in ordinary
flight, which is reflected in those counts. Independent source/native incoming
contracts remain $51AB12E7/$E7, with matching independently supplied prior bytes
for the $FD cases. Existing 2,512 input parents, sixteen Delete parents and
control/collision comparisons pass.

Native Release/Debug and both reference MSVC runners build. Nine affected
native CTests pass: host keys, scene exit, frame body, frontend, frame tail,
input, game input, qualification and artifact cleanup. Whole mission and
combat outcomes, independent scenario acceptance, context/reset and earlier
HUD output contracts, readable typed state, audio fidelity and measured 20 ms
frame performance remain open. This is a connected input/action improvement,
not whole-game completion.

The next concrete integration gap is native/menu.c's context_child: calculation
children CONTEXT_COMMAND_LOCAL_TO_WORLD and CONTEXT_COMMAND_SET_OBSERVER still
abort. Their existing matrix/view owners and the original C091E0/C0915A call
contracts should supply that composition and its outputs before expanding
context-command runtime coverage.
