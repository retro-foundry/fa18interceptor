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
| `$C02776` | `$FC5ECE` | 2,157,736 | Byte-exact `JMP` in `source_amiga/observed/jump_c02776_menu_library_stub.asm` |
| `$C00252` | `$FC0E9C` | 41,338 | Unclassified |
| `$C001FE` | `$FC1428` | 40,560 | Unclassified |
| `$C001F8` | `$FC1436` | 40,560 | Unclassified |
| `$C00102` | `$FC1BEA` | 37,669 | Existing Ghidra coverage calls this an external-function thunk; service unclassified |
| `$C023B8` | `$FE44F2` | 36,236 | Byte-exact `JMP` in `source_amiga/observed/jump_c023b8_library_stub.asm` |

`$C02776 → $FC5ECE` alone accounts for 81.07% of observed crossings. Its
frequency makes it the first boundary to identify from the pinned ROM and
observed callers. The report does not establish that this target is safe to
replace with a single C function; inspect the ROM body and calling context.

An otherwise identical 300-frame demo replay with and without the inventory
produced the same CPU totals and byte-identical RAM/register output. The
full-recording hashes above provide the stronger unchanged-replay check. GNU
and MSVC Release produced identical 300-frame inventories and RAM output;
run075 frames 393-402 remained 10/10 pixel-exact after the instrumentation.
