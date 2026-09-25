# Indirect stage target at `$C1EE14` (Hunk 10 +`$DCC`)

Classification: **dataflow with scenario-backed renderer descendants**. This
is the real dynamic target of the `JSR (A2)` at `$C1CC86` in the captured
human-flight invocation. Its individual record and object ownership remain
unassigned.

## Runtime packet

- State: `run001` frame 2,696 plus its sole rebased joystick press.
- The stage packet executes `$C1CC86  JSR (A2)` with target `$C1EE14`.
- Breakpoint `$C1EE14`, frame 9; complete return to `$C1CC88`.
- 7,866 instructions.
- P-code: `pcode/raw/run001_c1ee14_indirect_target/`, 1,523 observed RAM
  starts / 10,404 operations. 1,471 starts map to resolved Hunks; 52 remain
  unmapped.

The deterministic qualification oracle now independently reaches the same
target: normal run060 hits `$C1EE14` at replay frame 301 and returns to
`$C1CC88` after 21,030 instructions. This confirms the stage boundary on the
current canonical replay, while its branch/source-family interpretation remains
bounded by the existing evidence.

Its run060 call census confirms that this invocation is renderer-heavy:
`$C2F688` is entered 22 times, `$C2F5F4` 14 times, and `$C2FA7E` eight times,
alongside the established table/wrapper and line-setup family. These are
direct-child counts within the one completed stage call, not a claim about any
descriptor's game-world identity.

This supersedes the earlier stack-only association between `$C1CC86` and the
JOY0DAT callback. `$C1718E` remains independently proven as a registered
hardware callback, but this particular dynamic indirect call target is
`$C1EE14`.

The adjacent observed Ghidra entry `$C1ED48` is a four-byte `BRA.W` trampoline
to this stage. Its exact source is
`source_amiga/observed/branch_to_indirect_stage.asm`; it adds only this direct
structural edge and does not change the stage's classification.

`$C1ED4C-$C1ED6F` is also now reconstructed as a separate, byte-exact entry
gate in `source_amiga/observed/gate_c1ed4c_record_stream_stage.asm`.  It tests
bit 6 of `$C4585B`; that path branches backward to `$C1ED38`. Otherwise it
enters `$C1EE14` whenever `$C45785` is nonzero or the words at `$C459B6` and
`$C458DE` differ. Only equality with `$C45785` clear continues at `$C1ED70`.
This is structural/dataflow evidence for a route guard, not a claim
about object identity or stream semantics.

## Reverse caller-chain result

`analysis/reverse_call_stack_priorities.md` and its generated JSON reconstruct
the active user-stack calls in sealed run037, run041, and run042 traces.
In run037, all seven `$C1CC86` descriptor-stage calls return to `$C1CC88`:
five enter `$C1EE14` directly and two enter `$C1ED3C` before branching through
`$C1ED48` to `$C1EE14`. Six enter both the `$C1F4AC` immutable-triple
transform and `$C1F6F8` control-stream walker; one direct call returns after
39 instructions without either. Within those six frames are nine line-emitter
and four polygon-wrapper entries. The run041 close-landmark trace independently
reaches the same transform/walker path through `$C1EE14`, including calls from
`$C1CFA6`.

The 39-instruction exit is descriptor `$C225FC` with control pointer
`$C37EA6`, not a missing descriptor. It scans words `$4880/$4D00` at live
shift `7` and limit `96`, then reaches the `$FFFF` sentinel at `$C37EB2`.
The shared `$C1EEA0` exit returns `D0=0` to `$C1CC88`; its six bytes are now
reconstructed in `source_amiga/observed/return_zero_from_c1ee14_stream_scan.asm`.

Run041 now proves the stream-record selector itself. Four checkpoint windows
join live descriptor/control pointers, every `$C1EE62` comparison, selected
record, transform source, walker, and primitive in the same returned call.
See `analysis/data/run041_descriptor_detail_selector.md` and its generated
JSON. The selector is byte-exact at `$C1EE58-$C1EE83`; its live limit comes
from `$C45B40` and shift from `$C45AB8`. Different values select different
source families, while the 6,250 window also changes the active caller pass.
`analysis/data/run041_selector_input_provenance.md` proves that the immediate
writers are caller-loop record fields, not the earlier generic fixed-point
helper: `$C1CC86` uses `$C1CBA8/$C1CC60` and `$C1CFA6` uses
`$C1CE66/$C1CF6A`. Their physical meanings are not yet proved.

The transform and walker are **branches inside the enclosing invocation**, not
separate nested calls. Together with the proven `$C1CC70` descriptor-field
handoff and `$C1EF10` cursor publication, this supports a conditional
descriptor/control rendering-stage role. The stage's branch predicate, source
family choice, and per-placement primitive ownership still need a continuous
same-invocation trace; do not name a particular landmark or LOD rule from the
stack alone.
