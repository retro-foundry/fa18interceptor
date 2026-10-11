# Kickstart 1.3 graphics.library WaitBOVP at `$FC5E58`

The observed RAM vector `$C02764` jumps to `$FC5E58`. With graphics.library
base at `$C028F6`, it is the `-$192(A6)` vector, identified as
`WaitBOVP(viewport)(A0)` by the local Kickstart 1.3 `LVO.OFFS`. Its observed
return site is game code at `$C53F98`. The pinned 70-byte ROM body reads
viewport size and mode, saves D2-D3, and derives a signed beam-row target.
It clamps the target to the library's maximum row, then calls its `VBeamPos`
vector at `$FC5E90` until the beam reaches the target. A second entry at
`$FC5E5E` shares the body but was not seen as a native RAM-to-ROM entry.

`port/os/graphics_wait_bovp.c` performs the original instructions at their
boundaries, including word arithmetic and flags, stack saves in MOVEM order,
cycle charges, and the loop branches. The nested `VBeamPos` call at
`$FC5E90` remains on the ordinary vector path, where the existing C leaf
handles it. The runner checks all 70 ROM bytes before enabling this bridge.
It is on by default in translated mode; `--no-os-waitbovp` selects ROM,
and `--no-recomp` defaults to ROM.

The original native transition inventory counted 16,526 entries: 4,901 in
demo01, 8,051 in qual_carrier_success, and 3,574 in qual_fail_crashes. A
300-frame demo C-versus-ROM comparison matched RAM and runner statistics;
the ROM path entered the leaf 1,735 times and the C path entered it zero
times. Complete C-path runs of all three recordings retained their sealed
final RAM hashes and had zero entries to the original leaf. With C
default-on, the full gate passed: 386 routines, 726,979 matching shadow
calls and 925,873 matching sandbox calls across three recordings, zero
mismatches, and identical poison frames. GNU and MSVC Release produced
identical RAM and transition inventories for the focused 300-frame C-path
run. Run075 parity frames 393-402 remained 10/10 exact.
