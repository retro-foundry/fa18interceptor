# `$C17B2C` indexed pointer / Copper-trigger stage

Classification: **structural indexed-update stage**.

This complete 106-byte routine accepts three stack longword arguments. It uses
the first as a `*4` index into `$C0A438`; a null entry returns immediately.
Otherwise it passes the second argument to `$C17B08`, shifts the third argument
left 16 bits and stores it at `+$0C` through the source-table pointer, then
copies the source-table pointer into `$C4FE38[second * 4]`. Finally it invokes
`$C4FFB0` with that second index, causing the established indexed `COPJMP2`
strobe.

The routine is observed in eight capture packets, including direct command
children. Table/record ownership and the meaning of the arguments or Copper
selection remain unassigned.
