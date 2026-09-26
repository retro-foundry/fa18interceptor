# Run060 root-header scheduler operands

Classification: **scenario-backed dataflow**. This isolates observed writers
of two bits read by the selected-record scheduler gate. It does not identify a
physical qualification rule or say that either bit alone causes success.

`$C0A2F0` tests the selected root record at `$C46184` only after its selector
byte is zero. Its relevant header predicates are byte `+$01` bit 6 and:

```text
(word[$C46184 + $02] & $C080) == $C080
```

## Direct-core restore observations

The in-process Engine9000 bridge restored
`captures/run060/restored-state.bin`, applied `playback.e9k`, and sampled
through frame 9,200 at the first four bytes of `$C46184`. The sampled changes
establish two separate facts about that direct-core execution:

| replay frame | observed change | bounded producer evidence |
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

## Native replay boundary

The native boot-restore replay is deterministic at the frame-9,200 boundary:
the existing checkpoint and a fresh independent capture both hash to
`6400d29b3f34a95fe54d2a0f2170e8b60d86c4a887870a520a65348f77c48d15`.
Its root header is `11 C8 C0 82`.

Direct `retro_unserialize` restoration through the analysis bridge instead
reaches `11 C8 80 82` at frame 9,200. The bridge and native boot-restore paths
therefore are not interchangeable full-run validation oracles, even though
they start from the same serialized-state hash. This report uses direct-core
execution only to assign the instruction-level writers above; native
boot-restore checkpoints remain the authority for qualification outcome and
frame-state claims.

Authority:

- `build/run060_canonical_root_header_to_9200_samples/memory_region_mutations.json`
- `build/run060_canonical_header_writer_5091_exact_trace/memory_writes.json`
- `build/run060_frame08000_header_writer_8434_parent_trace/memory_writes.json`
- `build/run060_frame09200_checkpoint/checkpoint.json`
- `build/run060_frame09200_repro_checkpoint/checkpoint.json`
- `source_amiga/observed/toggle_arrestor_hook.asm`
- `source_amiga/observed/set_context_workspace_bit4.asm`
