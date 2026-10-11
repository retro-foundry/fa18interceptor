# Native intervening-command input return - 2026-10-07

Keyboard counter waits, modifier-only events, empty pending selection and
queue-only skips now preserve the preceding domain output, except when the
actual keyboard selector replaces it with its masked block byte. Successful
publication replaces it with the actual signed translated-queue index. These
results can reach a following first depleted recorder flare/chaff command.
Previously every intervening dispatch invalidated the completed result.

The actual caller is native entry -> frontend -> native_input_process ->
process_pending_key_events -> native_menu_dispatch_raw/pending ->
dispatch_keyboard_command_result/dispatch_pending_command_result -> existing
command selection, action and publish_command_event_result owners. Native
composition stays in port/game/native/menu.c. Legacy void dispatcher APIs
delegate to the same owners for reference/glue callers.

## Original contract

The relocated original disk source establishes the following exits:

- C1AD7E-C1AD8C increments a nonpositive keyboard counter and returns through
  C1AD72 while still nonpositive. This precedes any selection output load.
- C1AE02-C1AE08 loads and masks the blocked-command byte. The existing selector
  observer exposes that actual value, including for a later modifier-only exit.
- C1B092-C1B0EE modifier presses/releases change only their input latches and
  return through C1C2B6. They do not publish or assign an action output.
- C1AD64-C1AD70 empty pending selection clears the two secondary latches when
  selected by recorder mode; it leaves the prior output untouched.
- Queue-only C1C23C exits assign nothing when the queue is taken, the event is
  a release, or signed count is full. An accepted publication loads and extends
  its signed translated index at C1C298-C1C29E before the translated store.

CommandDispatchResult exposes the selected action and actual publication result.
It does not manufacture a result for an action whose own contract is incomplete.
Native dispatch composes the proven selection/preservation/queue exits; other
action-owned results remain UNKNOWN unless successful publication supersedes
them. Gameplay retains no CPU register shadow or captured value. Selection,
action, queue and modifier writes preserve their existing order and values.

## Playable integration

The countermeasure suite retains all previous 103 full bodies and 120 recorder
parents. Twelve additional ordinary Free Flight lost-target bodies produce a
defined cleanup output, followed by a captured actual keyboard input parent,
then first depleted recorder input. The intervening cases include modifier
presses/releases, queue-only press/release, two nonpositive counter waits and
two accepted control publications. Selection gates, counters and queue indices
are validation inputs; no completed return is seeded in native gameplay.

All 115 complete bodies, 132 recorder-input parents and twelve intervening
keyboard parents match original compared RAM/drawing. Original body execution
independently derives each preceding output. Original keyboard execution then
derives and checks its final output; that value is supplied only to the original
following-input oracle. Native input uses its own composed result. This proves
the connected sequence across two successive input parents without feeding the
original output into the playable runtime. Passing raw captures stay temporary.

## Focused comparisons

512 actual keyboard command parents match original non-stack RAM. 471 defined
low-byte results match: 366 preserve the prior output, 77 select the actual
masked block byte and 28 publish the signed translated-queue index. Fixtures
cover ordinary/bypassed selection, counter transitions, claimed/full queues,
negative/reset raw indices and signed translated indices. Original incoming
output is independently $51AB12E7; native stores only its low-byte contract $E7.
The 41 other action-owned results remain unresolved and are counted explicitly.

Existing 2,512 input-parent comparisons, sixteen menu/mission Delete parents,
256 control-effect cases, two $FD inputs and four collision parents still pass.
Native Release/Debug and both reference MSVC runners build. These bounded
component checks remain separate from complete-body integration and whole-game
acceptance; no comparison mask is widened.

Nine affected native CTests pass: host keys, scene exit, frame body, frontend,
frame tail, input, game input, qualification and artifact cleanup.

Remaining action-owned input returns, earlier HUD outputs, full scenario
acceptance, typed-state migration, audio fidelity and measured 20 ms frame
performance remain open. The complete-port goal stays active.
