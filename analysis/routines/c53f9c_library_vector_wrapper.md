# graphics.library `FreeSprite` wrapper at `$C53F9C`

Classification: **behavioural wrapper**. In the deterministic training
frame-600 false-guard route, `$C0F378` calls `$C53F9C` after pushing a zero
longword and execution returns to `$C0F37E` after 53 instructions. P-code is
`pcode/raw/training_600_c53f9c/`.

The wrapper preserves `A6`, loads its caller-supplied longword at `8(A7)` into
`D0`, loads the library base stored at `$C182CA`, and invokes its `-$19E`
vector. The established `graphics.library` base and pinned Kickstart graphics
headers identify that vector as `FreeSprite(D0)`. The complete
`$C53F9C-$C53FAF` wrapper is byte-exact source in
`source_amiga/observed/invoke_library_lvo_19e_with_argument.asm`.
