# Complete menu follow-up and delayed transition family (2026-10-03)

`port/game/menu_transition.c` recreates complete C0FCB4/C0FECE owners,
C0FFE2/C1000A table-arm entries, C17C2A sound pair and C24E8A summary
formatter. Source CPU adaptation and resumable timing are separate glue.
All six are original translated entries. Internal table arms and the shared
C10162 completion are parts of their owners, not additional registered entries.

## Complete original scope

`analysis/data/menu_transition_source_scope.json` seals 324 unique source
instructions, including 31 shared boundaries and all six eight-byte key/BRA
table rows. The table at C0FFDE selects 9/125/3/2/1/127 in reverse scan order.
The complete C0FECE owner has 134 instructions: 66 are cold instructions absent
from its 68-instruction generated listing. The audit decodes every table arm,
follows its branches through the enclosing return, and checks every generated
boundary is present. The committed byte/hash scope separately retains the
additional instructions. Reproduce with `python tools/recomp/audit_menu_transition.py`.

C0FCB4 saves the mode and queue cursor in its LINK -6 frame before checking
the sequence phase. It preserves the phase-six bypass, positive-mode messages
65/66/67/68/69, auxiliary message 6B, conditional D2/96 countdown and negative
mode paths. Positive routing uses the saved mode even when children change
globals; negative routing reads the changed mode after the actual children.
Queue stores, terminators, pointer arithmetic and callback publication retain
their source order and byte/word widths.

C0FECE saves the sign-extended mode in a LINK -2 frame and uses the signed
negative countdown gate. Its common body stops channels, chooses the exact
pair/noise/scripted sound route, prepares context/root/viewport globals and
scans the original table. All six arms remain complete, including source-only
children reached by the cold arms. The mode-two word update at C46986 is a
word operation despite that address also naming a byte flag elsewhere.
C0FFE2/C1000A enter continuations inside the already active enclosing frame;
their UNLK/RTS exits preserve that frame rather than adding their own LINK.

C17C2A preserves the bit-two gate and both real C17B2C calls, with the source
stack arguments and sound IDs 23/24. C24E8A reads every field from the child-
returned A1, retaining D0's returned high word before each subsequent MOVE.W.
Both unsigned DIVU stages preserve quotient/remainder packing, word sign
extension and overflow's unchanged dividend. The packed first result remains
on the original stack across the hours field. The final field falls through
into C24F76; its actual C25A08/C0F56A children and MOVEM stack writes are
preserved without a fictitious extra call frame.

## Independent readable-C proof

`python tools/recomp/check_menu_transition.py` compares complete original byte
execution with the independent normal C adapters in two distinct layers:

- Six entries with real original children, 16,384 calls each: 98,304 cases.
- Six entries with controlled child contracts, 8,192 calls each: 49,152 cases.
  Contracts compare all registers, full SR and all RAM at every child-entry
  boundary, then return changed registers and formatting-table pointers.

Each layer independently visits every owned source boundary, including all
six delayed table arms. All 147,456 calls compare all registers/high halves,
PC, full SR and all Chip/Slow RAM, including stack bytes, without exclusions.
The table-arm fixtures start with the parent's original LINK save frame.
Fixtures cover signed countdown/mode boundaries, initial CCR combinations,
sound gates, relocated tables and unsigned-division overflow. Controlled
children are test-only; production retains actual source child behavior.

Recorded normal C proof disables the selected timing steps while retaining
production liveness. C0FCB4/C0FECE pass 5,476 shadow / 5,332 sandbox
comparisons with zero hardware classifications or mismatches; eight incomplete
shadow calls remain retained and sandbox has none. Their four
peers are cold. The generic six-entry check retains its correct
`C0FFE2: no completed comparisons` rejection and raw zero-call reports.
Cold entries are proven by the complete original-byte structural tests;
live recordings establish their nonregression evidence. Shadow incomplete
classifications remain retained and must not be counted as completed proof.

## Timing

The local group passes all 324 instructions / 10,368 DMA cases, comparing
CPU, PC, full SR, cycles and RAM. Reproduce with
`python tools/recomp/check_active_planes_step.py --group menu_transition --bus`.
The oracle uses the complete audited C0FECE scope rather than its incomplete
generated listing. The shared operand adapter now supports the source's
PC-relative displacement/indexed forms. Existing audit defaults remain strict;
prior command, scheduler and context scopes still match their committed seals.
The fresh combined instruction oracle passes 14,963 instructions / 478,816
DMA fixtures after the shared-helper change. There are 257 source-timed entries
and 465 registered translated entries at this checkpoint. All 36,236 isolated
live RGB444 frames and final RAM seals match fresh OFF streams. The full gate
passes 554,025 shadow / 413,303 sandbox calls with zero mismatches, exact seals
and identical poison frames. GNU and MSVC Release builds pass; build artifacts
occupy 0.541 GiB. Detailed classifications and hashes are in
`analysis/figures/native_menu_transition_checkpoint.json`.

The 600-frame isolated probe is exact. ALL still first differs at 416/361;
the user-deferred Copper/HUD difference remains unchanged. Native OFF/ON
comparisons share the machine model and do not establish independent Amiga
timing parity. Continue C0FBE0/C17B96 and the remaining complete menu/input
helpers. Installed source-only callbacks C0FE36/C1017E/C10272/C103E4 are absent
from the recording-seeded translation and must also be reconciled before
declaring the whole game complete. Stage D -> F -> necessary E remains the goal.
