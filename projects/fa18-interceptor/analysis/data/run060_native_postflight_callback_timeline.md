# Run060 native postflight callback timeline

Classification: **native deterministic checkpoint state timeline**. This
extends the native scheduler-gate sequence from setup through the observed
success-side callback handoff. It proves state order at GUI-frame boundaries;
it does not identify the physical qualification predicate or date individual
instructions inside a frame.

| GUI frame | `$C45798` | phase / countdown / event | `$C45AD6` | callback slot `$C1820C` | observed state transition |
| ---: | ---: | --- | --- | --- | --- |
| 9,210 | `$FF` | `$03 / $04 / $00` | not sampled | `$C10DAE` | selected-record scheduler setup complete. |
| 9,263 | `$FF` | `$00 / $FF / $01` | `$0002` | `$C110A4` | phase-three countdown has expired; event and delayed callback stage are installed. |
| 9,275 | `$FF` | `$00 / $FF / $01` | `$0001` | `$C110A4` | delayed callback countdown decremented. |
| 9,280 | `$FF` | `$00 / $FF / $01` | `$0000` | `$C110A4` | countdown reaches zero. |
| 9,285 | `$EF` | `$00 / $FF / $01` | `$FFFF` | `$C10DAE` | selector changes to `$EF`; callback returns to the shared postflight prefix. |
| 9,290 | `$EF` | `$00 / $FF / $01` | `$FFF1` | `$C11958` | later success-side callback target is installed. |

This natively verifies the state ordering previously suggested by local
instruction traces: the scheduler's expired phase progresses to the delayed
`$C110A4` stage, then to the `$EF` selector state and a later `$C11958`
continuation. The static/direct-core traces identify `$C11078` and `$C110A4`
as the relevant local code paths, but are not used as the timing authority.

Authority:

- `build/run060_frame09210_native_checkpoint/`
- `build/run060_frame09263_native_checkpoint/`
- `build/run060_frame09275_checkpoint/`
- `build/run060_frame09280_checkpoint/`
- `build/run060_frame09283_checkpoint/`
- `build/run060_frame09284_checkpoint/`
- `build/run060_frame09285_checkpoint/`
- `build/run060_frame09290_checkpoint/`
