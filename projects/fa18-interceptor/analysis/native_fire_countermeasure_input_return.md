# Native fire/countermeasure input outputs - 2026-10-07

The existing fire helper now publishes its actual selected weapon/block nibble.
Successful flare/chaff actions publish their saved event word; depleted stocks
preserve the preceding input output through the existing message child.
Eject exposes the actual nested publication result, including its signed
translated-store index. Modified flare gates and sound 6 preserve prior output.

The connected caller is native entry -> frontend -> native_input_process ->
native_menu_dispatch_raw/pending -> command dispatch ->
execute_flight_command_result -> existing stages/message/context-publication/
audio owners. Game behavior stays in port/game/; native child composition and
the final input return stay in port/game/native/menu.c. This removes unresolved
fire/countermeasure/eject child outputs from this real native call chain.

## Original contract

- C1B21C tests the modifier before C0833E. The accepted fire helper loads the
  masked command-block nibble; ordinary modes then load the masked weapon
  nibble. Qualification gates retain the block result. The native child now
  consumes dispatch_space_command_effect's existing returned byte. Modified
  fire and C08394's release stores preserve the preceding output.
- C1C0FC/C1C18A saves the actual event only after successful stock subtraction.
  Depleted stocks do not assign it. C25704 changes D0, saves/restores D1 and
  leaves D4 untouched. Both actions still restore the child's semantic event
  word to the queue event. Typed output distinguishes assignment from
  preservation, including a KEY_TAKEN skip where that queue event is dead.
- C1B146 calls C1C214's toggle and complete shared publication before the
  outer queue tail. The child now returns the actual CommandPublicationResult;
  its assigned translated-store index survives the outer KEY_TAKEN skip.
  Modifier/origin gates and non-queued toggles preserve prior output.
- C1C122-C1C16E's modified flare gates preserve output. C17F8C and its
  C17B08/C17B2C, C4FFB0 and C50B02/C50AB4 children do not assign D4.
  C52EC8's signed division saves/restores D2-D5. Enabled, disabled and missing
  voice paths therefore preserve the same incoming domain output.

FlightActionOutput names fire selection, saved countermeasure event and nested
queue index separately from the event being published. Actual helper/child
results provide assigned values. No register shadow, captured output, fitted
constant or new game behavior supplies the result. Legacy event APIs delegate
to the same action bodies. CPU adapters retain their existing original child
execution and explicitly leave unused new child-output metadata unresolved.

## Connected comparison and probe correction

The prior weapon/radar probe set MODE_SELECT to zero. Original RAM/return
comparison passed, but that mode routed those new keyboard cases past the
intended flight actions. Its 4,416 focused command comparisons did exercise
the real action owners. The current runtime probe corrects the mode to one
and requires the intended owner and value for all 24 radar/weapon/fire/
countermeasure/eject cases. This supersedes the earlier action-integration
claim; those keys now demonstrably reach their actual playable owners.

Twelve additional Free Flight bodies supply actual cleanup outputs to fire,
release/modifier, successful/depleted stocks, eject gates/nested publication
and modified flare input before subsequent first depleted recorder input.
The suite retains previous comparisons. Each original body independently
supplies the original keyboard parent; its checked output supplies only the
original recorder parent. Native gameplay derives its own results throughout.

163 complete bodies, 180 recorder parents and 60 intervening keyboard parents
match original compared RAM/drawing and defined returns. Corrected radar and
weapon actions include the actual $80 subtraction branch. Passing raw captures
remain temporary, and no HUD/drawing exclusion or clock/output seed was added.

8,256 selected command parents match all compared non-stack RAM and defined
low-byte outputs: 1,958 preserve prior output, 1,638 publish selection, 156
publish queue indices, 3,218 publish flight-action values and 1,286 publish
view-action values. None is unresolved within this bounded suite. The added
3,840 parents include 1,024 pending fire cases across all block/weapon bytes
and qualification gates, 128 modified/released fire cases, 1,024 pending and
1,024 keyboard countermeasure cases covering all stock bytes, 512 eject cases
covering modifier/origin/toggle/queue gates and signed index pairs, and 128
modified-flare cases covering record eligibility, spawn, voice and sound gates.
Independent source/native incoming contracts remain $51AB12E7/$E7.

Native Release/Debug and both reference MSVC runners build. Existing 2,512
input parents, sixteen Delete parents and control/collision comparisons pass.
Component coverage remains separate from the complete-body integration above.
Nine affected native CTests pass: host keys, scene exit, frame body, frontend,
frame tail, input, game input, qualification and artifact cleanup.

Whole mission/combat outcomes, independent scenario acceptance, indexed/context
command contracts, earlier HUD outputs, readable typed state, audio fidelity
and measured 20 ms frame performance remain open. The full-port goal stays active.
