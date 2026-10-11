# Native flight-record destruction/expiry rendering

The active `fa18_native` path is `main.c` -> `native_frontend_tick()` ->
`native_flight_tick()` -> `native_scene_draw()` -> scene/followup placement
traversal -> `native_scene_placement()`. A reached C22AC0 descriptor with flags
$0040 and $0200 previously aborted at the missing C22ADE transition. The native
owner now sets its record timer at +$4C to 15, clears $0200 and sets $0400 before
the existing aircraft descriptor draw, exactly as C22ADE-C22AFC. This removes
that missing runtime dependency. The original C22C70 render hook is an RTS.

The existing readable C09DD0 owner clears selection only when SELECTED_RECORD
equals CHOSEN_RECORD and is not $FFFF. It posts message $4016 through C25704,
then masks WARNING_CAUSES with $FFFFBDFF. The original message-table entry says
TARGET DESTROYED. Its historical `CONTEXT_PUBLISH_SELECTION_TONE` hook name is
misleading: this child posts a message; the later C11BFC owns message timing
and sound. No immediate tone or substitute geometry was added. Repeated draws
after the $0200 flag clears preserve the expiry countdown and do not repost.

Component evidence: `native_model_oracle.c` retains each reached disk-backed
C22AC0 descriptor and render inputs, comparing native returns, every drawing
byte, record/vertex buffers and non-stack data with original instructions.
Its existing exclusions are only native model local scratch and original stack.
Controlled cases cover no selection, a different selection, a matching
selection, and drawing again at expiry timer 12 after clearing the message.
Five cases passed at each of three actual setup checkpoints (4000/5300/6100),
and five passed using retained demo frame-body entry 2364: **20/20** for this
descriptor sample. These inputs belong only to validation, not native startup.

Runtime integration: `native_record_expiry_test.c` links the exact same native
runtime objects as `fa18_native`, without CPU/chipset/glue/reference objects.
It starts from the original ADF and sealed demo input, reaches gameplay tick
222, takes the visible record from the actual selected-render list, and supplies
controlled destruction/selection inputs. The complete `native_scene_draw()`
traversal must perform the timer/flag change, clear selection, post the message
and preserve the next draw's timer. The player's cockpit view need not draw
the player's exterior model, so the test uses the source-generated render list.
It does not claim that an input-driven kill/collision sequence has been accepted.

Reproduce the integration check:

```powershell
cmake -S port/recomp -B build/native-cmake -DFA18_NATIVE_ONLY=ON -DBUILD_TESTING=ON
cmake --build build/native-cmake --config Release --target fa18_native fa18_native_record_expiry_test --parallel 8
ctest --test-dir build/native-cmake -C Release --output-on-failure -R '^fa18_native_record_expiry$'
python tools/native/check_models.py --runner build/native-cmake/native/Release/fa18_native.exe
```

Final production validation also passed frontend settled artwork, callsign/
save/reload, SDL presentation, unchanged ADF and the native link's CPU/chipset
omission check. Active update 2401 and crash update 2000 frame bodies still
match original compared gameplay state and every drawing byte. These focused
source oracles execute only the affected routines; no full original replay ran.
Copper fade remains excluded and faster intro/loading is allowed.

This batch is **1/1 complete (100% of the missing expiry-render connection)**.
Full recorded gameplay-sequence acceptance remains **0/3**; the retained
independent 128-update window still has 128/128 player-state/control matches
and 53/128 complete drawing-page matches. Seconds-driven target-info cadence,
C0DA38's alternate scene exit and other unported runtime branches remain open.
