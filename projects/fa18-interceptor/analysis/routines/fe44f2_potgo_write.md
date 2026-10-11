# Kickstart 1.3 potgo.resource WritePotgo at `$FE44F2`

The ROM caller at `$FE584A` invokes `-$12(A6)`. The local Kickstart 1.3
`POTGO_LIB.FD` and `LVO.OFFS` identify that vector as
`WritePotgo(word,mask)(D0,D1)`. The observed RAM vector `$C023B8` is a
byte-exact jump to `$FE44F2`. Its pinned 50-byte ROM body is:

```text
$FE44F2  AND.W   D1,D0
$FE44F4  NOT.W   D1
$FE44F6  MOVE.L  A6,-(A7)
$FE44F8  MOVEA.L $22(A6),A6
$FE44FC  JSR     -$78(A6)        ; Exec Disable
$FE4500  MOVEA.L (A7)+,A6
$FE4502  AND.W   D1,$28(A6)
$FE4506  OR.W    $28(A6),D0
$FE450A  MOVE.W  D0,$DFF034     ; POTGO
$FE4510  CLR.B   D0
$FE4512  MOVE.W  D0,$28(A6)
$FE4516  MOVE.L  A6,-(A7)
$FE4518  MOVEA.L $22(A6),A6
$FE451C  JSR     -$7E(A6)        ; Exec Enable
$FE4520  MOVEA.L (A7)+,A6
$FE4522  RTS
```

`port/os/potgo.c` contains the word-exact mask, merge, and cached-word
operations. `port/os/potgo_glue.c` performs the surrounding instructions at
their original boundaries, including bus accesses, flags, cycles, and stack
effects. The two nested Exec calls execute on the ordinary path, where the
existing Exec C leaves can intercept them. The runner checks all 50 ROM
bytes before enabling this bridge. It is on by default in translated mode;
`--no-os-potgo` selects the ROM body, and `--no-recomp` defaults to ROM.

The original native transition inventory counted 36,236 entries to this
leaf, once per frame across the three sealed recordings. A 300-frame demo
comparison with and without the C bridge matched RAM and runner statistics;
the ROM path entered the leaf 300 times and the C path entered it zero times.
Full C-path runs of demo01, qual_carrier_success, and qual_fail_crashes
retained their sealed final RAM hashes and recorded zero entries to the
original leaf. GNU and MSVC Release produced identical RAM and transition
inventories in the focused 300-frame C-path run. Interpreter-only mode
matched RAM with explicit C and default ROM settings over 300 frames, with
300 ROM entries and zero C-path entries. With C default-on, the full gate
passed: 386 routines, 726,979 matching shadow calls and 925,873 matching
sandbox calls across three recordings, zero mismatches, identical poison
frames. Run075 parity frames 393-402 remained 10/10 exact.
