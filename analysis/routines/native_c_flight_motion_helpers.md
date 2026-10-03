# Complete flight motion helpers

Updated 2026-10-03. Five complete original callable owners are implemented and
registered. The registry is 505/624 translated entries plus 66 original
source-only callable entries: 571 rows and 391 timed entries (325 translated,
66 source-only). Complete game-function and call/callback coverage remain open;
the stopping point is when only Kickstart services and timing remain.

The immutable demo01 source seals 229 unique / zero shared instruction
boundaries in `analysis/data/flight_motion_helpers_source_scope.json`. Each
owner retains actual original incoming JSR/BSR bytes from the preceding
nine-owner dynamics inventory. The readable domain is
`port/game/flight_motion_helpers.c`. Its normal adapter owns CPU outputs,
stack/frame effects and the two original C27456 child sites. The generated
bridge owns source instruction/bus/event boundaries. Handwritten production
code invokes no opcode handler. Shared runtime, bus, memory, arithmetic and
instruction/classification fixtures are unchanged.

| Owner | Complete behavior | Owned PCs | Controlled PCs | Real-child PCs |
| --- | --- | ---: | ---: | ---: |
| C26322 | Project incoming record motion to zero height, including signed division and exact long position writes. | 20 | 20 | 20 |
| C26352 | Scan sixteen slots, copy the first eight longs into the first available slot, select the original parity-dependent type and publish the refresh byte. | 20 | 20 | 20 |
| C26C72 | Project an indexed scene record to zero height using its caller argument and original LINK/UNLK frame. | 29 | 29 | 29 |
| C26CC0 | Select class-specific component tables, compare plane heights, retry original face consumers and preserve their flags and changed cursors. | 61 | 61 | 61 |
| C26D8A | Scan vector/triangle face records, form cross products, apply original geometry scaling and test signed dot products, including ADD overflow. | 99 | 99 | 99 |

163,840 completed calls pass all sixteen registers, PC, full SR and all Chip/
Slow RAM: 16,384 controlled and 16,384 real-child calls per owner. Both layers
visit every owned source PC. The controlled C27456 layer also compares complete
CPU/SR/RAM at both original child entries, changes working values, pointers and
flags, and publishes a changed cursor sentinel for the caller's next read.
Real-child fixtures reach both hit and miss paths through the actual original
triangle consumer. No internal source segment or fabricated entry is needed.

The projection preserves full register high halves, signed divide remainder/
quotient packing, overflow leaving the old dividend, and the original
minimum-long divided by -1 result. Zero full shifted velocity returns early;
a nonzero full velocity with zero low-word divisor still invokes the actual
divide-zero exception backend. Instruction fixtures prove the exception entry.
Complete projection return through the Kickstart handler remains unproven.
No zero-divisor substitute or synthetic service return is introduced.

Geometry retains logical long shifts, sign-extending word loads/address
indices, word-only negation of the long Y working value, exact caller-frame
locals and cursor saves, and BGE's signed N xor V condition after the final
ADD. The slot copy uses the original MOVEM register set and leaves its cursors
unchanged. A failed full slot scan retains the final low-word -1 count.

Actual ON/shadow/sandbox dispatch passes 3,840 completed hardware-free fixtures
and all OFF, selection, non-call and source-write invalidation guards. Exact
one-call classification remains required. The guard resolves actual translated
labels, including earlier exits where applicable. The shared assertions and
production dispatch are unchanged.

The independent normal-C recording variant disables only these five timing
steps and retains ranges, ownership and dispatch contracts. All five owners
have zero calls, comparisons, hardware and incomplete rows in all six reports
over the three recordings, with enclosing parents omitted from the selection.
The unchanged generic checker rejects C26322 for zero completed comparisons.
This rejection remains recorded; no normal-C recording comparison is claimed.
These cold owners have original callability and completed full-source fixture
proof, separately from recording parity.

Local DMA proof passes 229 instructions / 7,328 cases. A fresh combined run and
independent union pass 20,488 / 655,616: previous 20,259 plus 229, no overlap.
All twenty earlier generator outputs remain byte-identical. GNU headless and
MSVC Release builds pass. The full 571-row gate passes 567,984 shadow / 417,363
sandbox matches, zero mismatches, exact seals and identical poison frames.
All 36,236 isolated live RGB444 frames and RAM seals match. The family is exact
through frame 600; ALL retains frame 416 / 361 pixels. build/ is 1.172 GiB.
Copper fade, service
replacement and timing-only work remain deferred.

Reproduce with `audit_flight_motion_helpers_source.py`,
`check_flight_motion_helpers.py`, `check_flight_motion_helpers_dispatch.py`,
`check_active_planes_step.py --group flight_motion_helpers --bus` or `--group all`,
the full recording gate, isolated live gate, and the strict normal-C checker.
Raw logs/report hashes are in
`analysis/figures/native_flight_motion_helpers_checkpoint.json`.

Next complete C25B66/C266AE/C28996/C28B16: 1,308 unique / zero shared PCs and
actual original incoming calls, sealed in
`analysis/data/flight_dynamics_remaining_scope_inventory.json` and reproduced
by `audit_remaining_flight_dynamics.py`. These four enclosing owners remain
implementation work. Related C2C392's computed transfer at C2C46E remains
unresolved and must be reconciled before assigning its complete ownership.
