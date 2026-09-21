# `$C1B410` bounded three-axis control update

Classification: **structural with byte-exact source**. The routine decodes
three two-bit direction fields from `$65(A1)` and updates signed bytes at
`$28(A1)`, `$29(A1)`, and `$2A(A1)`. The first two axes change by 1 and clamp
to `$EC..$14`; the third changes by 3 and clamps to `$C4..$3C`.

The axis names describe independent state slots and bounded arithmetic. Their
physical flight-control binding requires a frame-bounded caller trace.
