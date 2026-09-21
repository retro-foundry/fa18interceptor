# `$C2DE96`: matrix-record zero guard

Static, byte-exact reconstruction: `source_amiga/observed/clear_zero_matrix_record_guard.asm`.

It tests word `$50(a1)`, returns after rewriting zero to zero, and otherwise
falls through to `$C2DEB0`. The owning record and continuation semantics
remain unassigned.
