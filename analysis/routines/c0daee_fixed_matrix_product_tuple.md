# `$C0DAEE-$C0DB41`: fixed matrix-product tuple

Authority: canonical slow-RAM bytes, within the same `$C0D720-$C0DB44` display
preparation segment as the completed `$C0D752` caller. The range ends at its
direct child call and `RTS` before unrelated bytes at `$C0DB42`.

The routine initializes the three signed input words `($E000,$3800,$E000)`,
multiplies them through three consecutive three-word groups at `$C45BD8`, and
forms three arithmetic-right-shifted longword sums in `D0-D2`. It then sets
`D6=8`, writes word 9 to `$C45954`, invokes `$C2EC9C`, and returns. Matrix,
tuple, child, and state-word roles remain unassigned.

In the captured call, `$C2EC9C` rejects the tuple through the shared
`$C2EC82-$C2EC8F` return, reconstructed in
`source_amiga/observed/reject_matrix_product_tuple.asm`: it returns `D0=0` and
writes longword `-1` to `$C45958`. This is a runtime-backed state/return
contract, not an assigned semantic result.
