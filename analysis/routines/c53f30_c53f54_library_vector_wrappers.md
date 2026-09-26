# `$C53F30`, `$C53F44`, and `$C53F54`: graphics-library display wrappers

Classification: **behavioral OS-wrapper identification**.

These adjacent complete leaves preserve `A6`, load the library base at
`$C182CA`, call LVOs `-$DE`, `-$E4`, and `-$168`, restore `A6`, and return.
The `-$DE` and `-$168` forms also load their caller's longword argument into
`A1` from `SP+$08` after preserving `A6`; the `-$E4` form forwards no explicit
argument register.

The live `$C182CA` Library node is `graphics.library`, and the pinned
Kickstart graphics headers map these LVOs and argument registers exactly:

```text
$C53F30  -$DE(A6), A1  = LoadView(A1)
$C53F44  -$E4(A6)      = WaitBlit()
$C53F54  -$168(A6), A1 = InitView(A1)
```

The 56-byte range is reconstructed exactly by
`source_amiga/observed/invoke_library_lvo_de_e4_and_168.asm`.
