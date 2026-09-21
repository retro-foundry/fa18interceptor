# `$C53FB0` / `$C53FC0` adjacent library-vector wrappers

Classification: **structural external-call wrappers**.

The two contiguous, byte-exact 16-byte leaves preserve `A6`, load the library
base pointer at `$C182CA`, issue a library-vector call, restore `A6`, and
return. `$C53FB0` targets `-$1C8(A6)` and `$C53FC0` targets `-$1CE(A6)`.

Both are observed from the outer update loop; `$C53FB0` also occurs in the
run024 crash-result execution sample. The external library's identity,
arguments, side effects, and LVO operation names are not established by these
wrappers.
