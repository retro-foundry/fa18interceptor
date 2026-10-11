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
| `$C00102` | `$FC1BEA` | 37,669 | Exec `GetMsg(port)` `-$174(A6)`; C bridge, see below |
| `$C023B8` | `$FE44F2` | 36,236 | potgo.resource `WritePotgo(word,mask)`; see below |
| `$C02812` | `$FC5A58` | 21,331 | graphics.library `WaitBlit()`; C bridge, see below |
| `$C02764` | `$FC5E58` | 16,526 | graphics.library `WaitBOVP(viewport)`; see below |
| `$C0272E` | `$FC64BC` | 16,012 | graphics.library `OwnBlitter()`; C bridge, see below |
| `$C02728` | `$FC64D4` | 16,010 | graphics.library `DisownBlitter()`; C bridge, see below |
| `$C02818` | `$FC63CC` | 15,979 | graphics.library `LoadView(view)`; see below |

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
state. This was the next direct service selected for a C bridge.

The `GetMsg` leaf now runs through the C bridge in translated mode. The
byte-exact ROM sequence and full native proof are in
[the GetMsg report](routines/fc1bea_exec_get_msg.md).

`$C023B8` is a byte-exact jump to `$FE44F2` in
`source_amiga/observed/jump_c023b8_library_stub.asm`. Its ROM caller at
`$FE584A` invokes `-$12(A6)`; the local Kickstart 1.3 `POTGO_LIB.FD` and
`LVO.OFFS` identify that vector as `WritePotgo(word,mask)(D0,D1)` in
`potgo.resource`. The pinned target masks D0 with D1, merges it with the
resource's cached word, writes `$DFF034` (POTGO), and calls Exec `Disable`
and `Enable` around that update. The 36,236 crossings are therefore an
internal ROM-to-resource call, one per observed native frame. This leaf now
runs through a C bridge in translated mode; its pinned instructions and full
native proof are in [the WritePotgo report](routines/fe44f2_potgo_write.md).

`$C02812` is the graphics.library `-$E4(A6)` vector, named `WaitBlit()` by
the local Kickstart 1.3 `LVO.OFFS`. Its pinned ROM target polls the blitter
busy bit in DMACONR. It now runs through a C bridge in translated mode; the
source and complete native comparison are in
[the WaitBlit report](routines/fc5a58_wait_blit.md).

`$C02764` is the graphics.library `-$192(A6)` vector, named
`WaitBOVP(viewport)(A0)` by the local Kickstart 1.3 `LVO.OFFS`. The pinned ROM
entry at `$FC5E58` reads viewport dimensions, computes a beam-row limit,
then calls `VBeamPos` at `$FC5E90` until the beam reaches it. Its observed
return site is game code at `$C53F98`. It now runs through a C bridge in
translated mode, with the existing C `VBeamPos` leaf handling its inner
poll. See [the WaitBOVP report](routines/fc5e58_wait_bovp.md).

`$C0272E` and `$C02728` are the graphics.library `-$1C8(A6)` and
`-$1CE(A6)` vectors, named `OwnBlitter()` and `DisownBlitter()` by the local
Kickstart 1.3 `LVO.OFFS`. Their pinned ROM entries are `$FC64BC` and
`$FC64D4`; the observed game return sites are `$C53FBC` and `$C53FCC`.
The first adjusts the graphics library ownership counter and calls an
internal ROM helper. The second adjusts the same counter, tests owner and
blitter state, and can call several internal helpers or write the blitter
interrupt and DMA registers. These paired services require their branch
paths and nested calls to be preserved together in a C replacement. Both
now run through a C bridge in translated mode; the source and native proof
are in [the blitter ownership report](routines/fc64bc_fc64d4_blitter_ownership.md).

`$C02818` is graphics.library `-$DE(A6)`, named `LoadView(view)(A1)` by the
local Kickstart 1.3 `LVO.OFFS`. Its pinned ROM entry `$FC63CC` saves A1 and
calls the substantive helper at `$FCD564`; the observed return site is game
code at `$C53F40`. That helper waits on raster state, updates display
registers and library view pointers, and takes further ROM calls. Replacing
only the short vector wrapper would leave the actual `LoadView` work in ROM;
port the wrapper and helper as one service.

An otherwise identical 300-frame demo replay with and without the inventory
produced the same CPU totals and byte-identical RAM/register output. The
full-recording hashes above provide the stronger unchanged-replay check. GNU
and MSVC Release produced identical 300-frame inventories and RAM output;
run075 frames 393-402 remained 10/10 pixel-exact after the instrumentation.
