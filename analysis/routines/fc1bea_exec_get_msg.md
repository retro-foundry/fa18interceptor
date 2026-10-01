# Kickstart 1.3 Exec GetMsg at `$FC1BEA`

The observed RAM jump at `$C00102` enters the pinned ROM at `$FC1BEA`.
With ExecBase at `$C00276`, this is the `-$174(A6)` vector, named
`GetMsg(port)(A0)` by the local Kickstart 1.3 `LVO.OFFS`. The NDK's
`exec/ports.h` places `mp_MsgList` at offset `$14` in `MsgPort`. The pinned
ROM contains these 46 bytes:

```text
$FC1BEA  LEA     $14(A0),A0
$FC1BEE  MOVE.W  #$4000,$DFF09A
$FC1BF6  ADDQ.B  #1,$126(A6)
$FC1BFA  MOVEA.L (A0),A1
$FC1BFC  MOVE.L  (A1),D0
$FC1BFE  BEQ.B   $FC1C08
$FC1C00  MOVE.L  D0,(A0)
$FC1C02  EXG     D0,A1
$FC1C04  MOVE.L  A0,4(A1)
$FC1C08  SUBQ.B  #1,$126(A6)
$FC1C0C  BGE.B   $FC1C16
$FC1C0E  MOVE.W  #$C000,$DFF09A
$FC1C16  RTS
```

The routine returns zero for an empty list. Otherwise it removes and returns
the first message node, updating the head and following node's predecessor.
It masks interrupts while changing the links and uses the ExecBase interrupt
nesting byte at `$126(A6)` to decide whether to reenable them.

`port/os/exec_glue.c` performs these instructions in C at their original
instruction boundaries. It uses the same depth operations as the C
`Disable`/`Enable` pair and preserves the bus accesses, condition flags,
cycle charges, and return stack effects. The runner checks all 46 ROM bytes
before enabling this path. It is on by default in translated mode;
`--no-os-getmsg` selects the ROM path, and `--no-recomp` defaults to ROM.

The original transition inventory counted 37,669 entries to this leaf over
the three sealed recordings. A 300-frame demo comparison with the ROM path
was identical in RAM and runner statistics. The ROM path entered the leaf
3,514 times; the C path entered it zero times. Complete C-path runs of
demo01, qual_carrier_success, and qual_fail_crashes retained their sealed
final RAM hashes and had zero entries to the original leaf. GNU and MSVC
Release C-path runs produced identical RAM and ROM transition inventories
over the focused 300 frames. Interpreter-only mode still entered the ROM
leaf 3,640 times in its separate 300-frame probe. The default-on full routine
gate passed: 386 routines, 726,979 matching shadow calls and 925,873 matching
sandbox calls across three recordings, zero mismatches, and identical poison
frames. Run075 parity frames 393-402 remained 10/10 exact. With `--no-recomp`
and explicit `--os-getmsg`, RAM matched the interpreter-only ROM default over
300 frames while the C path recorded zero entries to the original leaf.
