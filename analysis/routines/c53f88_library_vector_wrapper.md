# `$C53F88` library-vector wrapper

Classification: **structural external-call wrapper**.

This complete 20-byte leaf saves `A6`, forwards its caller's longword argument
from `SP+$08` into `A0`, loads the library base at `$C182CA`, calls
`-$192(A6)`, restores `A6`, and returns. The library identity and external
operation meaning remain unassigned.
