# `$C1B410` bounded three-axis control update

Classification: **structural with byte-exact source**. The routine decodes
three two-bit direction fields from `$65(A1)` and updates signed bytes at
`$28(A1)`, `$29(A1)`, and `$2A(A1)`. The first two axes change by 1 and clamp
to `$EC..$14`; the third changes by 3 and clamps to `$C4..$3C`.

The axis names describe independent state slots and bounded arithmetic. Their
physical flight-control binding requires a frame-bounded caller trace.

## run060 initial-turn instance

The sealed run060 replay enters this exact routine at frame 949 with
`A1=$C46184`. Root control byte `+$65` is `$21`; its first two-bit field
(`$30`) is `$20`, the routine's decrement code. The bounded 22-instruction
packet writes root `+$28` from `$FF` (signed −1) to `$FE` (signed −2).
The `$C0` and `$0C` fields are zero, so it writes zero to both `+$29` and
`+$2A`.

The immediately following `$C1342C` packet consumes that `+$28 = −2` lane to
select the table-driven target for the first run060 pitch-like transform
update. This proves the data dependency from record control bits to the
numeric orientation path, while leaving the origin and real-world meaning of
`+$65` unassigned.
