# Native post-flight scheduling

Actual view publication is now available through the optional `publication`
owner. The connected proof, `check_native_postflight_publication.py`, passes
90,112 calls at all 481 reachable source boundaries with no child contracts.
It runs original publication/zoom/redraw/queue instructions and the actual
native bodies, sharing canonical view and queue fields. The contracted proof
below remains evidence of the separately tested parent boundary. Production
must bind the actual owner. See `native_context_publication.md`.

`port/native_postflight.c/.h` implements complete `$C09E06`, all eight dispatched
mode owners, `$C0A12E` view restoration and `$C0A3EA` readiness. The native
record scheduler now invokes finish directly. Only periodic and dispatch remain
in `FA18NativeControlRecordOps`.

The authority is the sealed original instruction graph, corroborated by
`port/game/postflight_scheduler.c`. Mode 3, 4, 5, 6, 7, 9, 125 and other retain
their separate phase rules, signed gates, countdowns and outcome values. The
shared existing-phase tail calls actual native readiness. The two mode-five
restorations read five signed words from the caller's original parameter view
and publish through the same live record fields. No packed record mirror,
guest pointer or CPU state is exposed.

Finish clears all three original latches before actual selection release.
The generic phase write retains the old selection word's high byte after
release, rather than reading the possibly changed selection again. View saving
retains the signed countdown comparison, saved-view bit, context flags,
heading and command-word updates. Mode-four/seven preparation rereads sequence
phase after the lower publication call. Its interface carries the source slot,
command event and route, allowing the actual queue publication to be ported
without guessing an input. `$C1BEE8` remains the one true lower boundary, at
return sites `C09FBC` and `C0A2BC`.

Mode-five and mode-six distance gates use the mathematical signed subtraction
to decide negation, followed by wrapped long results for comparison. The full
axis value is retained after reference loading, partial distance processing,
restoration and preparation-child mutations. The original slot-four change is
visible to the caller; it is not reset to the scheduler's prior slot.

Readiness shares the actual indexed `pose_entry` field. Bootstrap binds phase
publication to its existing first startup word pair, target selection to the
aircraft spawn-gate word, command flags to the aircraft command word, and
player phase/flags to the native player setup. The other scheduler-owned mode,
event, sequence, view-side, origin and selection pointers must stay shared.
Additional production bindings between the queue's neighboring fields and
startup owners remain required before native-main integration; the composed
bootstrap fixture is not a complete original-state constructor.

`python tools/recomp/check_native_postflight.py` passes 90,112 complete calls
(8,192 per entry) at all 371/371 original boundaries. Actual selection-release,
readiness and restoration instructions run in the source oracle. All Chip/Slow
RAM matches except CPU ABI stack `C7FD00..C7FF00`; typed aircraft/geometry
owners and the carried axis are independently checked. The 1,334 publication
contracts verify both return sites, record/slot identity and command event,
then independently mutate the live sequence phase, slot, record and axis.
They prove the parent boundary, not the actual publication child.

Fixtures exercise all modes, saved/unsaved views, preparation before both phase
outcomes, depleted and wrapped countdowns, swapped record pairs, signed distance
overflow, negative parameter offsets, all byte selectors and root-readiness
gates. Bulk RAM comparison covers exactly the same full regions and ABI-stack
exception as the diagnostic byte loop; detailed byte reads run on a mismatch.
No comparison region or typed-owner check was removed.
See `analysis/figures/native_postflight_checkpoint.json`.

MSVC Release game and affected contracts, strict GNU compilation, fifteen
affected CTests and the 505-file native guard pass. Composed scheduler contracts
exercise actual finish mode-three publication with no outer child callback;
update-stage contracts observe its real latch clears. The historical bootstrap
parent proof also passes 16,384 calls at all 291/291 boundaries, while retaining
its three contracted whole children. Original asset/state construction, actual
view publication, lower flight/render/audio owners and native main remain open.
