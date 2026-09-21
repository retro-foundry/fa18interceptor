# `$C2DAF2`: current-record matrix setup

Static, byte-exact reconstruction: `source_amiga/observed/build_current_record_matrix.asm`.

The function selects `$C46184 + $C458DE`, derives an angle from word `$68`
relative to `$7080` when nonzero, and calls `$C2E370` with output `$C45C0E`.
The record's gameplay ownership and the matrix consumer remain unassigned.
