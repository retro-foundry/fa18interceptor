# `$C27456`: indexed-triple plane-side scan

Classification: **runtime-backed behavioral geometry, observed path
memory-read-only**. This is a plane-side test inside the `$C26EBE` candidate
record scan, not yet a proved collision, visibility, or model-identity rule.
It is called at `$C27410`, and the caller branches on its returned Z flag.
The focused `$C26EBE` trace establishes that `A3` is the broad-phase-selected
`$C46184` record (observed: `$C47D84`), while `A4` is a static pointer stream
selected by the follow-on phase; see `c26ebe_candidate_record_scan.md`.

Each nonnegative long read from `(A4)+` points to a record whose words at
`+2,+4,+6` select three signed offsets into the table at `A3` (the third
offset is first masked with `$0FFF`). `$A4` is added to each offset. The
selected locations hold three-word points `p,q,r`. With 68000 widths, the
helper forms `u=wrap16(q-p)` and `v=wrap16(r-p)`, then computes the three
components of `u cross v` with `MULS.W`/`SUB.L`. Each component is arithmetically
shifted right by `7 + word[-$5A(A6)]` using the register-shift count, then
only its low 16 bits are used in the final `MULS.W`.

The other dot-product operand is derived from `p`. Its X and Z words, and
its sign-extended Y long, are arithmetically shifted by `word[-$5A(A6)]`.
The routine then adds `word[A3+$0C]`, `long[A3+$10]`, and `word[A3+$0E]`
respectively, subtracts `A2.w`, `A1.l`, and `word[-$42(A6)]`, and negates
**only the low word** of each result. Thus the Y long calculation is
ultimately truncated to a signed word at `MULS.W`. The three signed products
are accumulated in `D7` with two `ADD.L` operations; `BLT` tests N xor V
from the second addition, not a fresh test of the wrapped sum.

When `BLT` is taken, the next candidate starts at `$C27456`. Otherwise the
routine scans `(A4)+` longwords to the next negative sentinel, backs `A4` up
by two bytes, sets `D7=0`, and returns with `Z=1`. The negative first-long
branch at `$C27450` is outside the observed P-code path; this report does
not assign its return contract. The routine consumes stream data via `A4`
even though the captured path stores no persistent memory.

The live packet is the 64-start `observed_call_00c27456` subset of
`pcode/raw/no_key_c1c63e/`, backed by
`build/no_key_parent_update_capped/trace.jsonl`. At trace rows 4697-4880,
three candidate side scores are negative `-109968`, negative `-30545`, then
positive `98990`: the first two take `BLT` back to `$C27456`; the third
reaches the sentinel scan and returns `D7=0,Z=1` to `$C27414`. This verifies
the comparison direction and cursor/return behavior, without identifying
the candidate's game-world object. Byte-exact source is split across
`prepare_c27456_inner_record.asm`, `calculate_c27478_inner_record_products.asm`,
`prepare_c274ba_inner_relative_components.asm`, and
`finish_c274ec_inner_record_loop.asm` under `source_amiga/observed/`.
