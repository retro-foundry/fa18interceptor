# `$C53F30`, `$C53F44`, and `$C53F54` library-vector wrappers

Classification: **structural external-call wrappers**.

These adjacent complete leaves preserve `A6`, load the library base at
`$C182CA`, call LVOs `-$DE`, `-$E4`, and `-$168`, restore `A6`, and return.
The `-$DE` and `-$168` forms also load their caller's longword argument into
`A1` from `SP+$08` after preserving `A6`; the `-$E4` form forwards no explicit
argument register.

The external library identity, API names, input ownership, and side effects are
not proven by these wrappers. The 56-byte range is reconstructed exactly by
`source_amiga/observed/invoke_library_lvo_de_e4_and_168.asm`.
