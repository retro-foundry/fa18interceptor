# `$C50212` `A3+$2C` zero guard

Classification: **structural**.

The entry is observed in 15 independent P-code capture packets, including menu,
attract, human-flight, and run024 crash-result presentation. Its direct caller
at `$C5017C` has already loaded `A3` through a pointer chain. The observed path
is:

```asm
$C50212  CMPI.L  #0,$2C(A3)
$C5021A  BEQ.S   $C5027A
```

The branch target is the routine's static `RTS`; the nonzero continuation is
not reconstructed. `A3` record ownership and the meaning of the `+$2C` field
remain unassigned.
