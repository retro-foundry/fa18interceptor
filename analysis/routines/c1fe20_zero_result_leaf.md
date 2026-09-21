# `$C1FE20`: zero-result leaf

Authority: `pcode/raw/attract_1800/instructions.segmented.jsonl`, whose two
observed instructions at `$C1FE20` and `$C1FE22` respectively write zero to
`D0` and return through the stack. Ghidra inventories this as a distinct
four-byte entry in Hunk 13.

`$C1FE20-$C1FE23` is byte-exactly reconstructed in
`source_amiga/observed/return_zero_result.asm`. This establishes only that the
observed path returns longword zero in `D0`; the caller and semantic meaning of
that result remain unassigned.
