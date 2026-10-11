# `$C53F88`: graphics.library `WaitBOVP` wrapper

Classification: **behavioral display-synchronization wrapper**.

This complete 20-byte leaf saves `A6`, forwards its caller's longword argument
from `SP+$08` into `A0`, loads the library base at `$C182CA`, calls
`-$192(A6)`, restores `A6`, and returns. `$C182CA` is the live
`graphics.library` base established in
[`c53fb0_c53fc0_library_vector_wrappers.md`](c53fb0_c53fc0_library_vector_wrappers.md);
the pinned Kickstart graphics headers identify `-$192` as `WaitBOVP(A0)`.

The outer-loop child `$C1612C` calls this wrapper immediately after entering,
passing the address `$C1822A` as its viewport argument. It also calls the same
wrapper in its activity path. This is the proved display-synchronization gate
immediately preceding the outer-loop back-edge; it is not merely an arbitrary
library callback.
