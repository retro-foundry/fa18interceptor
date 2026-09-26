# Run060 root-header scheduler operands

Classification: **scenario-backed dataflow**. This isolates observed writers
of two bits read by the selected-record scheduler gate. It does not identify a
physical qualification rule or say that either bit alone causes success.

`$C0A2F0` tests the selected root record at `$C46184` only after its selector
byte is zero. Its relevant header predicates are byte `+$01` bit 6 and:

```text
(word[$C46184 + $02] & $C080) == $C080
```

## Canonical replay observations

The sealed canonical run060 replay, restored from
`captures/run060/restored-state.bin`, was sampled through GUI frame 9,200 at
the first four bytes of `$C46184`. The sampled changes establish two separate
facts:

| GUI frame | observed change | bounded producer evidence |
| ---: | --- | --- |
| 5,091 | word `+$02`: `$0082 -> $8082` | A parent-update trace pauses at `$C0EFD4` and records `$C1B630: EORI.W #$8000,$C46186`, changing header `11 C8 00 82` to `11 C8 80 82`. |
| 8,434 | byte `+$01`: `$C8 -> $D8` | The parent trace records `$C1CA8C: OR.B D7,$1(A0)` with `A0=$C46184`, setting bit 4. |
| 8,435 | byte `+$01`: `$D8 -> $C8` | The same bounded trace records `$C1D65E: BCLR.B #4,$1(A0)`, clearing that transient bit. |

`$C1B630` is the byte-exact raw-$20 arrestor-hook state toggle in
`toggle_arrestor_hook.asm`; it is therefore a proven producer of the mask's
bit 15. The run060 trace does not identify a producer of mask bit 7.

The `+$01` observations are deliberately narrower: bit 4 is a workspace
context lane written and cleared around the traced update, while scheduler bit
6 remains set throughout this interval. They rule out conflating the observed
transient bit-4 update with the scheduler's bit-6 predicate.

## Checkpoint caveat

The independently captured frame-9,200 checkpoint contains `11 C8 C0 82`,
whereas the canonical replay sampler's last observed bit-15 toggle yields a
different `+$02` high byte before that frame. These are distinct authority
captures and have not yet been reconciled. This report therefore uses the
canonical replay only to assign the exact observed writers above; it does not
claim that either capture alone gives the scheduler-gate value at frame 9,210.

Authority:

- `build/run060_canonical_root_header_to_9200_samples/memory_region_mutations.json`
- `build/run060_canonical_header_writer_5091_exact_trace/memory_writes.json`
- `build/run060_frame08000_header_writer_8434_parent_trace/memory_writes.json`
- `source_amiga/observed/toggle_arrestor_hook.asm`
- `source_amiga/observed/set_context_workspace_bit4.asm`
