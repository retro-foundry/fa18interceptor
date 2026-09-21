# Additional traced runtime projected-edge lists

Each exact range below is inline data inside an original `HUNK_CODE` payload. The enclosing Hunk remains code; only the listed selector-plus-offset-pair bytes are classified as data. In every case the trace enters `$C212B0`, which consumes the selector then resolves each pair through the mutable `$C48390` triple workspace before calling `$C2EE4A`.

The coordinate plots linked below are human-identification aids, not proof that a packet is a complete immutable model.

## `$C3751A`

`$C3751A-$C3752F` is 22 bytes: one selector and five endpoint-offset pairs. `build/run001_c1f6f8_record_walk_stage/trace.jsonl` enters `$C212B0` with `A2=$C3751A`; its plot is [run001 packet `$C3751A`](../plots/run001_packet_c3751a_orthographic.svg).

## `$C393C4`

`$C393C4-$C393D1` is 14 bytes: one selector and three endpoint-offset pairs. `build/attract_hud_trace_450/trace.jsonl` enters `$C212B0` with `A2=$C393C4`; its plot is [attract packet `$C393C4`](../plots/attract_packet_c393c4_orthographic.svg).

## `$C39886`

`$C39886-$C3988F` is 10 bytes: one selector and two endpoint-offset pairs. `build/attract_focus_600/trace.jsonl` enters `$C212B0` with `A2=$C39886`; its plot is [attract packet `$C39886`](../plots/attract_packet_c39886_orthographic.svg).

## `$C3989A`

`$C3989A-$C398A3` is 10 bytes: one selector and two endpoint-offset pairs. `build/attract_hud_trace_450/trace.jsonl` enters `$C212B0` with `A2=$C3989A`; its plot is [attract packet `$C3989A`](../plots/attract_packet_c3989a_orthographic.svg).
