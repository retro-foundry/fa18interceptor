# Connected native countermeasures — 2026-10-07

Latest follow-up: claimed recorder depletion is connected and validated; a first
depleted recorder command with an unclaimed input queue still needs its real
enclosing carry producer. See the final section for current evidence. Earlier
counts and remaining-scope paragraphs below describe their original batches.

Keyboard F and C now execute the original flare/chaff commands, post their
messages, consume stock, and launch/update/draw control records in the playable
`fa18_native` runner. The removed dependencies were missing command children
and the reset-only scene adapter for C1518C. This does not complete the port.

## Runtime ownership and source contracts

The actual caller is frontend -> flight -> `native_input_process()` / C0F3C4
-> C1AD74 command dispatch -> C1C0E0/C1C172. C25704 posts the source messages;
MISSION_FLAGS_A then reaches scene -> `native_control_effects()` / C1518C
-> C15688 launch and C153FC update. Composition lives in
`port/game/native/control_effects.c`, reusing existing record actions,
normalisation, collision/ground checks, audio and drawing owners.

C1AE02/C1AE08 load and mask COMMAND_BLOCK_FLAGS before selecting direct
keyboard commands. A selected depleted countermeasure therefore has a known
zero low carry byte; the inherited high byte cannot reach the event queue.
Successful commands save their own event at C1C0FC/C1C18A. Their C25704 child
returns that carry and masks the message event's low byte. SHIFT-F in mode 6
calls C17F8C's sound-6 owner; the historical SPAWN child name is misleading.

Source C159AE/C15AD4 and C257EC calculate launch direction. The existing
C266AE collision owner now returns its typed result for native consumption;
the reference adapter still observes the same operations. C26C72 checks ground.
The source draw owners are C2CD94 (three template pairs using C2ED70), C2CD28
(chaff point/layer transition at timer 58) and C2CCA0 (ordinary point). The
existing C2CE82 rotation now also returns values without requiring CPU glue.
Visible comparisons caught and fixed an incorrect C2EE4A clipped-segment call:
the original pair renderer calls C2ED70 and preserves its endpoint workspace.

## Evidence

`python tools/native/check_countermeasures.py` runs the runtime integration
entry and focused original opcode comparisons, retaining exports under
`build/native-flight/countermeasure-check/`.

| Scope | Result |
| --- | --- |
| Complete C0F3C4 input parent | 688 cases match compared non-stack RAM; includes 544 additional keyboard stock/modifier cases |
| Complete C1518C control-effect parent | 192 cases match compared non-stack RAM: 20 launches and 172 motion/drawing/expiry cases |
| Actual keyboard-driven Free Flight | Two launches and two early ground-contact expiries; final stocks 15/15, flight stage C10DAE |
| Original C0EFEA-C0F3C0 on four actual native bodies | Zero compared gameplay differences and zero display bytes in every body |

Input cases include every stock byte for both keyboard commands and 32
SHIFT-F mode-6 gate combinations, with deliberately nonzero original incoming
D4. The control-effect oracle varies original record inputs only in its test
entry. Ninety-six additional cases put effects into a visible identity view,
including pair endpoints, chaff layer sizes and ordinary points. These component
comparisons use reference drawing facilities; the four complete body comparisons
exercise the actual native raster and the same runtime objects as the executable.

The integration entry starts from the ADF and ordinary host key presses; it
does not seed gameplay records. Captures bracket F/C launch at host ticks
6201..6203 / 6903..6905, and early expiry at 6208..6210 / 6910..6912. Saved
game ticks are 92, 94, 300 and 302. Ground contact shortens these lifetimes;
this run does not demonstrate uninterrupted airborne 30/60-tick lifetimes.
Controlled component cases cover timer transitions separately.

The component oracle excludes original stack and six explicitly bounded native
automatic-frame scratch ranges. Complete-body exclusions remain the existing
scratch, asynchronous voice and blitter-busy scopes; no HUD/display mask was
added. Copper fade remains outside display-plane comparisons. Original
blitter cycles are serviced in the component oracle so visible line waits finish.

Affected validation also passes queued input timing, active/crash/map frame
bodies, three native scene/expiry/countermeasure integration tests, frontend
and native link omission checks, and twelve reference host/loading checks.
Both MSVC native and reference runners build. No full original replay was run.

## Remaining scope

Depleted pending recorder countermeasure carry and recorder $FD function-level
selection still require their real enclosing caller contract. C266AE's component
and face collision children remain explicit missing-child failures if reached by
a control effect. Other commands/modes, complete gameplay sequences, full
record/flag/counter comparisons and post-result behavior remain unfinished.
This evidence establishes the connected tested countermeasure paths, not
whole-game equivalence or bit-exact audio.

## Follow-up: control-effect scene collisions

The native C266AE caller now consumes C26CC0/C26D8A's typed results. These
owners return their complete working values and preserve the original A3/A5
cursors; their CPU adapters still observe the same operations. C26CC0's
native C27456 child reuses `faces_all_behind()` on the actual face-pointer
stream, original record points, shift and eye coordinates. Only its stream
cursor and predicate are live to the component parent.

The complete C1518C comparison now covers **256 cases**, including 64 controlled
descriptor/position cases. Original opcode coverage records 256 component
child calls, 32 face child calls, 128 face-plane calls, 32 hit returns and
220 misses. All compared non-stack RAM still agrees. Descriptor metadata and
collision inputs are confined to tests; the model points/face tables come
from the disk-loaded carrier record. These cases demonstrate face hits and
component misses, not every possible component hit or complete weapon kill.

After the ordinary Free Flight run, the same native integration entry also
executes four controlled C1518C parents with the executable's runtime objects.
Exports `frame.collision.N.before.dat` / `.after.dat` bracket **only C1518C**,
not a complete C0EFEA frame. Original opcodes match all compared RAM, including
display bytes: two face hits and two component misses, with one of the latter
consuming eight face-plane calls. The previous four real keyboard-driven full
bodies still pass unchanged. The native frontend/link check and 256 cases per
owner in both legacy contract/real-child motion-helper comparisons also pass;
both MSVC runners rebuild. No original full replay was repeated.

The earlier missing component/face-child statement is superseded by this
follow-up. Depleted pending recorder carry, $FD indexed selection, other
commands/modes and complete gameplay equivalence remain open.

## Follow-up: recorder $FD function keys

C1BCEE subtracts $50 and adds $0B to the inherited selection, then C1BCF6
jumps to C1BEDA, whose unconditional branch publishes the original event at
C1C23C. That selection is dead to RAM and event publication. Other indexed
paths define their own selection before using it. The native caller therefore
no longer aborts on $FD function keys or requires a guessed carry.

Complete C0F3C4 input comparisons now pass **848 cases**, including 160 $FD
F1-F10 cases across mode, modifier, latch and publication gates with deliberately
nonzero incoming D4. Two controlled $FD input parents run through the actual
native host queue/process with the executable's runtime objects after ordinary
disk/input startup. `frame.fd.N.before.dat` / `.after.dat` bracket C0F3C4 alone,
not complete gameplay bodies. Both match original compared non-stack RAM.
The collision parents and four keyboard-driven full bodies still pass. Menu
mode banners, mission function keys, availability gates and pilot-log controls
also pass their affected native check.

The earlier $FD-selection limitation is superseded. Depleted pending recorder
countermeasure carry still requires its enclosing caller contract; whole-game
coverage and equivalent-state gameplay sequences remain unfinished.

## Follow-up: depleted commands after an input claim

The actual caller remains frontend -> native flight -> `native_input_process()`
/ C0F3C4 -> C1AC28 -> C1C0E0/C1C172 -> C25704 -> C1C23C. Pending selection
defines event zero but does not assign D4. Successful countermeasures save the
event before the message child; depleted ones inherit D4. The byte-exact
`source_amiga/observed/enqueue_command_input_event.asm` starts by testing
KEY_TAKEN and branches directly to modifier clearing when it is nonzero,
before the release-bit test and both queue stores. The inherited event is
therefore dead to the reached RAM behavior in that case. `native/menu.c` now
allows that path through its existing command, stock/message and publication
owners. It does not introduce a carry producer or replace the unclaimed case.

`python tools/native/check_countermeasures.py` passes these current checks:

| Scope | Evidence |
| --- | --- |
| Complete C0F3C4 component contracts | 2,512 parents match original non-stack RAM; 1,536 new cases cover every stock byte, flare/chaff and recorder modes 1/2/3 with KEY_TAKEN set and incoming D4=$51AB12E7 |
| Actual shared-runtime recorder input | 24 controlled C0F3C4 parents after normal Free Flight startup match compared RAM, including display; twelve start claimed and twelve execute successful flare before depleted chaff claims no second event |
| Existing keyboard-driven complete flight bodies | Two launches and two early ground-contact expiry bodies match compared gameplay and every display byte |
| Existing control-effect parents | 256 component cases and four actual controlled parents still pass |
| Existing $FD and Delete contracts | Two actual $FD parents and sixteen menu/mission Delete parents still pass |

The recorder fixtures cover stocks 1/$80 and zero/one after the successful
flare; raw queue cursors zero/nine and translated cursors zero/seven vary.
They seed only controlled input globals after the ordinary disk/key startup.
Their before/after files bracket C0F3C4 alone; they are not natural depleted
recordings or complete sequence evidence. The four ordinary full bodies remain
separate. No RAM/display exclusions were added, and no full replay was repeated.

The checker now uses `CaptureWorkspace`: passing runs retain logs and JSON,
not raw RAM; a failed original comparison retains its one case. Explicit
`--keep-captures` is available for deliberate diagnostics. All current passing
captures were removed automatically. Release/Debug builds pass, public Release
is refreshed, and five selected runtime CTests plus automatic cleanup pass.
The game-input gate initially failed on stale assertions requiring the old
cold-start flag/countdown gap. It now requires all 164 player-core bytes to
match at the consumed carrier-input checkpoint; the corrected gate passes.
This strengthens the comparison by removing its three-byte exception.

A first depleted recorder countermeasure with KEY_TAKEN zero still aborts
explicitly. Its event can change the claim/release test and raw/translated
queue contents, so zero is not a valid general replacement. The source
producer across preceding frame/message/drawing work must be reconstructed
before connecting that remaining path. Complete mission sequences, typed-state
migration, audio fidelity and measured 20 ms acceptance remain unfinished.
