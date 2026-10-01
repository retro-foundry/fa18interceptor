# Native RAM-to-ROM transition inventory

Stage E starts with observed crossings into Kickstart. The optional
`--rom-transitions OUT.json` runner flag records the previous instruction PC
and the next interpreter PC whenever execution enters `$F80000-$FFFFFF` from
below `$F80000`. It counts transitions without reading game memory or changing
the execution path. Each JSON row has `source`, `rom_entry`, and `count`.

These are **transitions**, not identified OS calls. A source can be a game jump
stub, a library vector, or the last RAM instruction before an interrupt. The
two PCs alone do not name the library function, its arguments, or its caller.
The report omits ROM-to-ROM transfers and therefore is an entry inventory,
not a full ROM execution profile.

Collected with the GNU native runner over all three sealed recordings:

```text
fa18_recomp --state captures/native/NAME/state.bin \
  --input captures/native/NAME/input.fa18in --to-end \
  --rom local/system/kick13.rom \
  --rom-transitions build/recomp/rom_transitions_NAME.json \
  --ram-out build/recomp/rom_transitions_NAME_ram.bin
```

| Recording | Frames | Distinct pairs | Transitions | Final RAM equals seal |
| --- | ---: | ---: | ---: | --- |
| demo01 | 20,833 | 4,437 | 982,814 | yes |
| qual_carrier_success | 12,353 | 3,195 | 1,183,087 | yes |
| qual_fail_crashes | 3,050 | 1,016 | 495,767 | yes |
| Combined | 36,236 | 5,769 unique | 2,661,668 | all |

Most frequent pairs across all three:

| RAM source | ROM entry | Transitions | Established source |
| --- | --- | ---: | --- |
| `$C02776` | `$FC5ECE` | 2,157,736 | graphics.library `VBeamPos` vector; see below |
| `$C00252` | `$FC0E9C` | 41,338 | Exec interrupt dispatcher; see below |
| `$C001FE` | `$FC1428` | 40,560 | Exec `Disable()`; see below |
| `$C001F8` | `$FC1436` | 40,560 | Exec `Enable()`; see below |
| `$C00102` | `$FC1BEA` | 37,669 | Exec `GetMsg(port)` `-$174(A6)`; see below |
| `$C023B8` | `$FE44F2` | 36,236 | Byte-exact `JMP` in `source_amiga/observed/jump_c023b8_library_stub.asm` |

`$C02776 → $FC5ECE` alone accounts for 81.07% of observed crossings, but it
is an **internal graphics.library callback**, not a game-originated OS call.
`port_info.py C02776` gives ROM return site `$FC5E94`. The pinned ROM bytes at
`$FC5E90` call `-$180(A6)` and then compare the result with a row limit; the
Kickstart 1.3 `GRAPHICS_LIB.FD` and `LVO.OFFS` on the Amiga Developer CD name
`-$180` as `VBeamPos()`. The RAM vector at `$C02776` is a byte-exact jump to
`$FC5ECE`. There, the pinned ROM executes `MOVE.L $DFF004,D0`, `ASR.L #8,D0`,
`ANDI.L #$1FF,D0`, `RTS`: it returns the raster row. This is strong evidence
for the identity and behaviour of this one vector, but its frequency mostly
measures how often graphics.library polls the beam. Stage E should identify
the game's direct library calls separately before replacing their larger ROM
call chains.

The pinned `VBeamPos` leaf is now replaced by C in the translated runner; its
source and full native proof are in [the VBeamPos report](routines/fc5ece_vbeam_pos.md).

The pinned Exec `Disable()` and `Enable()` leaves are also replaced by C in
the translated runner. Their source, ROM disassembly, and three-recording
proof are in [the Exec interrupt report](routines/fc1428_fc1436_exec_interrupts.md).

The pinned ROM at `$FC0E9C` tests the saved exception status on the stack,
checks ExecBase interrupt/task state, restores saved registers, and returns
with `RTE` or continues into interrupt server dispatch. `$C00252` is its
observed RAM jump stub. These crossings count interrupt handling, not a
library call made by the game.

`$C00102` is the ExecBase `-$174` vector, identified as `GetMsg(port)(A0)`
by the local Kickstart 1.3 `LVO.OFFS`. Its observed jump stub enters ROM at
`$FC1BEA`; the source there advances A0 to the port's message list, masks
interrupts, removes the first node if present, and restores the interrupt
state. This is a direct service candidate for the next source-backed C leaf.

An otherwise identical 300-frame demo replay with and without the inventory
produced the same CPU totals and byte-identical RAM/register output. The
full-recording hashes above provide the stronger unchanged-replay check. GNU
and MSVC Release produced identical 300-frame inventories and RAM output;
run075 frames 393-402 remained 10/10 pixel-exact after the instrumentation.
