# `$C32D24`: message-record selector scaffold

Classification: **observed selection/dataflow, with unknown input origin**.
This block sits directly upstream of the observed
`$C32C16 -> $C456FE -> $C32FCE` text-compositor path.  Its input producer is
not yet attributed to a game-state transition.

The byte-exact entry reconstruction is
`source_amiga/observed/select_message_record.asm`.  Its negative-selector path
uses the captured immediate `ADDA.L #$1B8,A4` encoding and deliberately skips
the two auxiliary activation writes that the positive-header path performs.

## Static contract

- `$C32D24` loads layout descriptor base `$C41066` into `A1` and relative-word
  table base `$C3ED0A` into `A0`.
- It normalizes incoming `D0`, reads one signed word from `A0 + 2*(D0 - 1)`,
  and forms `A2 = A0 + selected_word` at `$C32D4C`.
- The selected record is decoded through its leading bytes and an `A4` offset;
  `$C32E0E` publishes `A1/A2/A4` at `$C4570A`.
- `$C32C16` later restores that exact triplet and copies it to `$C456FE`.
  The run024 qualification transition observes this latter handoff with
  `A1=$C41066`, `A2=$C3F303`, and `A4=$0EB2`.

The table contains entries leading to text-record interiors across the
main-menu, training, qualification, and mission briefing/result pool.

## Flight-return selection observation

`build/run024_flight_return_trace/trace.jsonl` executes this path at chipset
frame 19,425.  The selector comes from a live sequence rooted at `$C4574A`,
not an arbitrary caller register.  `$C32BD2` reads byte cursor
`$C457C6=$12`, tests the *next* word at `$C4574A + $12 + 2 = $C4575E`, and
because it is nonzero (`$006D`) advances the cursor to `$14`.
`$C32CEE` then sign-extends that cursor, reads word `$C4574A + $14`, and
passes its value `D0=$006D` (109) to `$C32D24`.

`$C32D24` retains the value in `D2`, applies `(109 - 1) * 2 = $D8`, reads
the relative word `$069D` at `$C3EDE2`, and forms `A2=$C3F3A7`.

The selected descriptor bytes are `$A5 $02 $00 $A2`; the routine derives
`A4=$19CA`, consumes the two following metadata bytes, then publishes
`A1=$C41066`, `A2=$C3F3AB`, and `A4=$19CA` at `$C4570A`.  The cursor is now at
the NUL-terminated payload:

```text
$C3F3AB  ESC ......... RESTARTS YOUR SELECTION
```

This is direct runtime evidence for a queued/ordered selector sequence and
for the relative-table message-record selector: selector 109 chooses a
flight-return/menu instruction record.  The trace does not establish which
routine initially populated this sequence, nor does it identify a persistent
mission-status byte.

## Controlled selectable-missions observation

The controlled run029 digit-6 replay reaches this entry at frame 328 with
`D0=$806E`.  Since `D2` is negative, the entry masks the selector to `$006E`
(110), resolves table word `$0A1B` at `$C3EDE4` to `$C3F725`, skips two record
bytes, and publishes `A2=$C3F729`.  The resulting payload is `ESC - TO MAIN
MENU`, visibly present in the frame-400 selectable-missions screen.  The
bounded trace returns to `$C32F5C` after 37 instructions.

The same no-future-input replay does not reach another `$C32D24` entry between
frames 329 and 500.  This proves a live submenu instruction record, but not
the selector producer or an F1 mission-selection path.

## Controlled optional-label record sequence

After the controlled `$C1017E` condition-byte state is resumed, five
successive no-input `$C32D24` packets receive `D0=77,78,79,80,81` in that
order. Each takes the positive record path and returns to `$C32F5C` after 43
instructions. The static relative table resolves these to `$C3F46C`,
`$C3F494`, `$C3F4BA`, `$C3F4E2`, and `$C3F510`, the visible F2--F6 lines.
This is direct controlled selector-to-record consumption, not evidence that
the force-written state represents any legitimate qualification condition.
See `analysis/run029_key6_all_conditionals_display_probe.md`.
