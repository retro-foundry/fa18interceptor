# Record-component constraint at `$C1FC42` (Hunk 10 +`$F0A`)

Classification: **structural**. This helper selects and compares a shifted
signed component from a table-referenced record. The component's coordinate
or gameplay meaning is unknown.

## Runtime packet

- No-input `start_demo` replay; direct call `$C1F77C → $C1FC42`.
- Breakpoint `$C1FC42`, return boundary `$C1F780`.
- 18 instructions, complete at the observed `RTS` `$C1FCCE`.
- P-code: `pcode/raw/no_key_c1fc42_record_constraint/`, 18 starts / 156
  operations, all mapped to verified Hunk 10.

## Observed branch

The helper obtains a base pointer from `$C45A32`, extracts a selector from
bits `$0C00` of `D7`, and reaches the selector-zero branch at `$C1FCAE`.
There it loads a signed word at `base + D0 + $0C`, shifts it right by the low
nibble at `base + 6`, negates longword `$C45A78`, and compares the values.
On this call the comparison takes the non-less-than path and returns with the
condition flags produced by `BTST #12,D1`; no data register result is assigned
on the observed path.

The two other selector branches are statically present but unexecuted here.
