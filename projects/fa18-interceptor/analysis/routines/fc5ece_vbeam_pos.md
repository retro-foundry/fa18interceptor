# Kickstart 1.3 graphics.library VBeamPos at `$FC5ECE`

The pinned 256 KiB ROM has these 16 bytes at `$FC5ECE-$FC5EDD`:

```text
20 39 00 DF F0 04  E0 80  02 80 00 00 01 FF  4E 75
MOVE.L $DFF004,D0  ASR.L #8,D0  ANDI.L #$1FF,D0  RTS
```

The Kickstart 1.3 `GRAPHICS_LIB.FD` and `LVO.OFFS` in the local Amiga Developer
CD identify `-$180(A6)` as `VBeamPos()`. ROM `$FC5E90` calls that vector; its
RAM vector at `$C02776` jumps to `$FC5ECE`, and the return site is `$FC5E94`.
The function reads VPOSR/VHPOSR as a longword, shifts it right eight bits,
keeps nine bits, and returns the beam row in `D0`. `ASR.L` sets `X` from bit 7
of the register pair; the final `ANDI.L` sets `N/Z/V/C` and preserves `X`.

`port/os/graphics.c` contains the row calculation. Its temporary CPU bridge
in `port/os/graphics_glue.c` performs the four source instructions at separate
instruction boundaries, with the original opcode and data bus accesses,
cycle charges, register effects, and return. The runner checks the exact ROM
bytes once before enabling this bridge. `--no-os-vbeam` restores ROM execution
for comparison; the C path is enabled by default for the translated runner
with the pinned ROM. `--no-recomp` keeps the pure interpreter baseline unless
`--os-vbeam` is explicitly requested.

The first full C trial (`--os-vbeam`) over all three native recordings ended
with each recording's sealed RAM hash. The transition inventory recorded zero
entries from `$C02776` to `$FC5ECE` with the C path enabled, compared with
738,245 demo, 982,339 carrier, and 437,152 crash entries under ROM execution
(2,157,736 total). A 300-frame default-on versus `--no-os-vbeam` comparison
was byte-identical in RAM and registers; GNU and MSVC default-on outputs also
matched. Run075 frames 393-402 remained 10/10 pixel-exact. With the C path
as the translated runner's default, the full standard gate passed: 386
routines, 726,979 shadow matches, 925,873 sandbox matches, zero mismatches,
identical poison frames, and all three recordings still sealed.
