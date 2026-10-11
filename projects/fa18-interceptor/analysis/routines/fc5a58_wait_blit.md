# Kickstart 1.3 graphics.library WaitBlit at `$FC5A58`

The observed RAM vector `$C02812` jumps to `$FC5A58`. With graphics.library
base at `$C028F6`, it is the `-$E4(A6)` vector, named `WaitBlit()` by the
local Kickstart 1.3 `LVO.OFFS`. Its observed return site is in game code at
`$C53F50`. The pinned 36-byte ROM body is:

```text
$FC5A58  BTST.B #6,$DFF002
$FC5A60  BTST.B #6,$DFF002
$FC5A68  BNE.B  $FC5A6C
$FC5A6A  RTS
$FC5A6C  NOP
$FC5A6E  NOP
$FC5A70  BTST.B #6,$DFF002
$FC5A78  BNE.B  $FC5A6C
$FC5A7A  RTS
```

Bit 6 of the high DMACONR byte reports blitter busy. The first two reads
precede the branch; if the blitter remains busy, the two NOPs and third read
form the wait loop. `port/os/graphics.c` names the busy-bit test, and
`port/os/graphics_glue.c` performs the source instructions at their original
boundaries, preserving each hardware read, cycle charge, flag, branch, and
return. The runner checks all 36 ROM bytes before enabling the C path.
It is on by default in translated mode; `--no-os-waitblit` selects ROM,
and `--no-recomp` defaults to ROM.

The original transition inventory counted 21,331 entries across the three
sealed native recordings: 8,639 demo, 10,171 carrier, and 2,521 crash.
The first 300 demo frames did not call this leaf, so the full demo recording
was used for direct C-versus-ROM comparison. Those complete runs had
identical final RAM and runner statistics; the ROM path entered the leaf
8,639 times and the C path entered it zero times. Full C-path runs of all
three recordings retained their sealed final RAM hashes and had zero entries
to the original leaf. GNU and MSVC Release produced identical final RAM and
ROM transition inventories for the complete demo C-path run. With C
default-on, the full gate passed: 386 routines, 726,979 matching shadow calls
and 925,873 matching sandbox calls across three recordings, zero mismatches,
and identical poison frames. Run075 parity frames 393-402 remained 10/10
exact.
