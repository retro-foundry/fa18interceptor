# `$C1B3EA` three-axis update gate

Classification: **structural with byte-exact source**. Bit 4 at `$04(A1)`
gates the bounded three-axis helper. When set, a nonnegative word at `$4C(A1)`
clears `$28/$29/$2A(A1)` and returns; a negative word clears the bit then joins
the helper. A clear bit joins the helper directly.
