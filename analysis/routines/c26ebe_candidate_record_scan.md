# Candidate-record scan at `$C26EBE`

Classification: **runtime-backed two-stage candidate geometry filter**.
`$C2600E` calls this helper after forming a relative `D2/D3/D4` triple, then
branches on its returned condition. The entry is observed in attract, no-key,
run001, and run003 update packets.

The exact `$C26EBE-$C26EFB` entry clears `$C4589F`, uses `$C459B6` as an offset
into the record region rooted at `$C46184`, snapshots words at relative `$00`
and `$02` plus byte `$5E`, and masks record byte `$04` down to its low six bits.
It extracts the high nibble of byte `$62` and returns immediately for class
`$20`; otherwise later raw code advances candidate offsets by `$200` and bounds
them against `$1E00`.

This proves record selection and class filtering, but not a collision/object
identity interpretation for the scan or its relative triple.

Its observed child at `$C27410 -> $C27456` is now a bounded indexed-triple
plane-side scan: it repeats while the signed side score is below zero and
returns `Z=1` on the first nonnegative candidate. See
`c27456_candidate_plane_side_test.md`. This is geometry evidence within the
candidate scan, not yet proof of collision or visibility ownership.

Each candidate advances the offset by `$200` and stops above `$1E00`.  An
eligible candidate needs header bit `$40`, no `$600` header bits, and a `$5E`
byte differing from the initial record.  Its class rejects `$30`, has a special
`$20` path with `$A0000` in `A1`, and otherwise reaches raw class-specific code.
The observed common path loads three longs at relative `$14/$18/$1C`, subtracts
the caller's `D2/D3/D4` components, takes each absolute value, and applies the
class-selected `A1` bound before scanning on.

The focused no-input attract trace at frame 607 resolves the handoff between
the two stages. It advances through the `$200`-stride record region and reaches
the `$1C00` record (`$C46184 + $1C00 = $C47D84`). That record passes the
three-component bound stage: `$C4589F` changes from zero to one and byte
`$03` of the selected record changes from `$00` to `$01`. The following stage
uses that same `$C47D84` record as `A3`, prepares two reference probes through
the mutable `$C46228/$C4623A` workspace, and calls `$C27456` with static
pointer streams rooted at `$C39168` and `$C391E4` in `A4`. Thus `$C27456` is
the narrow plane-side phase for a broad-phase-selected candidate record.

In this trace the plane-side phase eventually returns to `$C26014` with
`D0=0,Z=1`; its caller's `BEQ.W $C26294` is taken. This does **not** negate the
earlier broad-phase acceptance: the separate status byte remains set. The
exact game-world meaning of these two outcomes remains unassigned.

The observed scan-limit exit at `$C27504` re-forms the selected record pointer
from `$C46184 + $C459B6` and tests signed long `$10` against `$7FFF` before
entering the next stage.  The relation of that terminal record to candidates is
not yet assigned.

Evidence: `build/no_key_c26ebe_candidate_scan/trace.jsonl` (focused trace,
3386 instructions, frame 607, ending at `$C26014`), its before/after slow RAM
snapshots, and the byte-exact source slices under `source_amiga/observed/`.
