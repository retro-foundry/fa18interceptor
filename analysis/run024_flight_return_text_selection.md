# Run 024 flight-return text selection

Authority: `build/run024_flight_return_trace/trace.jsonl` and its matching
Slow RAM snapshot.  This note isolates the message-selection evidence within
the wider flight-return trace; it does not characterize the entire trace.

## Observed path

At chipset frame 19,425, `$C32BD2` reads byte cursor `$C457C6=$12` into `D0`
and tests the next word at `$C4574A + $12 + 2 = $C4575E`.  That word is
nonzero (`$006D`), so it increments the cursor to `$14`.  `$C32CEE` then
reads the word at `$C4574A + $14`, passing `D0=$006D` into `$C32D24`.

`$C32D24` executes the positive-selector path, saving the value in `D2`, then
reads the relative-word table rooted at `$C3ED0A`:

```text
selector: 109
table index: (109 - 1) * 2 = $00D8
table address: $C3EDE2
relative word: $069D
record descriptor: $C3F3A7
```

The descriptor starts `A5 02 00 A2`.  By `$C32E0E`, the routine has consumed
those four bytes and writes the compositor handoff `A1=$C41066`,
`A2=$C3F3AB`, `A4=$19CA` to `$C4570A`.  `A2` therefore addresses the string:

```text
ESC ......... RESTARTS YOUR SELECTION
```

This makes both the selector sequence and the relative-table message selector
live evidence, complementing the direct menu-to-qualification compositor read documented in
`analysis/routines/c32fce_static_text_compositor.md`.  The trace starts after
the writer that initialized this selector sequence, so neither its producer
nor a persistent qualification/mission flag can be inferred from this packet.

## Queued top-level menu batch

The matching final Slow RAM snapshot holds this nonzero prefix at `$C4574A`.
The byte-exact static writer `$C0FBE0` writes this exact code sequence and
terminating zero.  Each word resolves through the same `$C3ED0A` relative table
and descriptor format; the text below begins at descriptor plus four header
bytes.  Thus this is stronger than an address-ordered string inventory: it is
the actual ordered batch available to the compositor in the flight-return
scenario.

| Cursor offset | Code | Descriptor | Payload | 
|---:|---:|---:|---|
| `$00` | 6 | `$C3F237` | `F/A-18 INTERCEPTOR` |
| `$02` | 100 | `$C3F25D` | `SELECT:` |
| `$04` | 101 | `$C3F26B` | `1 ... DEMO` |
| `$06` | 102 | `$C3F27C` | `2 ... FREE FLIGHT, NO ENEMY CONFRONTATION` |
| `$08` | 103 | `$C3F2AC` | `3 ... TRAINING: DEMO OF MANEUVERS` |
| `$0A` | 104 | `$C3F2D5` | `4 ... TRAINING: PRACTICE MANEUVERS` |
| `$0C` | 105 | `$C3F2FF` | `5 ... QUALIFICATION: REQUIRED FOR MISSIONS` |
| `$0E` | 106 | `$C3F330` | `6 ... SELECTABLE MISSIONS` |
| `$10` | 107 | `$C3F350` | `7 ... NEXT ACTIVE ADVANCED MISSION` |
| `$12` | 108 | `$C3F379` | `8 ... YOUR CURRENT FLIGHT LOG STATISTICS` |
| `$14` | 109 | `$C3F3A7` | `ESC ......... RESTARTS YOUR SELECTION` |

The trace specifically reaches code 109 after the cursor advance described
above.  It does not prove that every preceding entry was rendered in this
trace window, only that this menu batch is resident and ordered for selection.

The table is reproducible as
`analysis/data/run024_flight_return_message_sequence.json` with:

```text
python scripts/decode_message_sequence.py build/run024_flight_return_trace/slow.bin --output analysis/data/run024_flight_return_message_sequence.json
```
