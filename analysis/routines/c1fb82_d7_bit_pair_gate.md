# `$C1FB82-$C1FB8B`: `D7` bit-pair gate

Authority: the identical observed instruction rows in
`pcode/raw/attract_600/instructions.segmented.jsonl` and
`pcode/raw/attract_1800/instructions.segmented.jsonl`. Ghidra inventories this
as a distinct ten-byte entry.

The entry copies `D7.w` to `D1.w`, masks `$0C00`, then branches to `$C1FC3A`
when the result is nonzero. The zero result falls through to `$C1FB8C`, which
is deliberately outside this exact slice. Neither the `D7` bit-pair ownership
nor either successor's wider meaning is assigned.

Byte-exact source: `source_amiga/observed/branch_on_d7_bit_pair.asm`.
