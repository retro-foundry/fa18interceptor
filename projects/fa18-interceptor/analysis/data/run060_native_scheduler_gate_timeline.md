# Run060 native scheduler-gate timeline

Classification: **native deterministic checkpoint state timeline**. Every row
is a fresh boot-restore replay of sealed run060, not an in-process direct-core
replay. It establishes state ordering and the exact native transition interval;
it does not identify the physical meaning of the flags or the instruction
writer.

`$C0A2F0` requires the selected root-record header's bit-6 and `$C080` mask,
a zero selector byte, and zero word `+$6E` before it initializes its scheduler
fields. Native checkpoint reads give the following timeline:

| GUI frame | root header | `$C45798` | root `+$6E` | `$C4582A/$2C/$AD` | conclusion |
| ---: | --- | ---: | --- | --- | --- |
| 8,750 | `11 C8 80 02` | `$00` | `$0718` | `$00/$FF/$00` | `$C080` mask fails: bit 7 is clear. |
| 8,772 | `11 C8 80 02` | `$00` | `$06A0` | not sampled | still mask-failing. |
| 8,796 | `11 C8 80 02` | `$00` | `$05EC` | not sampled | still mask-failing. |
| 8,799 | `11 C8 80 82` | `$00` | `$05B0` | not sampled | `$C080` mask is now satisfied; `+$6E` remains nonzero. A no-input continuation from native frame 8,796 reaches `$C14E08`, which ORs `$0080` into the selected root word `+$02`, reproducing `$8002 -> $8082`. |
| 8,855 | `11 C8 80 82` | `$00` | `$0593` | not sampled | header stays gate-ready while `+$6E` decreases. |
| 8,870 | `11 C8 C0 C2` | `$00` | `$057E` | not sampled | additional header bits change; the required mask remains satisfied. |
| 9,000 | `11 C8 C0 82` | `$00` | `$00A9` | not sampled | only `+$6E` still blocks the listed gate conditions. |
| 9,205 | `11 C8 C0 82` | `$00` | `$0007` | `$00/$FF/$00` | gate is still blocked by nonzero `+$6E`. |
| 9,208 | `11 C8 C0 82` | `$00` | `$0007` | `$00/$FF/$00` | still blocked. |
| 9,209 | `11 C8 C0 82` | `$00` | `$0000` | `$00/$FF/$00` | all listed record/selector conditions are now ready; scheduler state is not yet initialized. |
| 9,210 | `11 C8 C0 82` | `$FF` | `$0000` | `$03/$04/$01` | native scheduler setup has completed. |

Thus the native run establishes a necessary-state sequence:

```text
header mask becomes ready (8,797..8,799)
  -> word +$6E reaches zero (9,208..9,209)
  -> selector/phase/countdown/latch setup on the following frame (9,210)
```

The recorded joystick release at frame 8,791 precedes the bit-7 transition,
but the transition is delayed through frame 8,796; this is temporal proximity,
not input causality. The register/byte-level native snapshots identify no CPU
writer. The separately retained direct-core instruction trace at `$C0A324`
matches the static scheduler stores, but is local dataflow evidence only and
is not used to date the native transition.

Authority:

- `build/run060_frame08750_checkpoint/`, `run060_frame08772_native_checkpoint/`,
  `run060_frame08796_native_checkpoint/`,
  `run060_frame08799_native_checkpoint/`, `run060_frame08855_native_checkpoint/`,
  `run060_frame08870_native_checkpoint/`, `run060_frame09000_checkpoint/`
- `build/run060_frame09205_native_checkpoint/`
- `build/run060_frame09208_native_checkpoint/`
- `build/run060_frame09209_native_checkpoint/`
- `build/run060_frame09210_native_checkpoint/`
- `source_amiga/observed/prepare_selected_record_scheduler_state.asm`
