# Command-event packet at `$C32CEE`

Classification: **behavioural packet**. The deterministic training frame-600
parent calls `$C32CEE` from `$C0F3BA` and returns to `$C0F3C0` after 24
instructions. P-code is `pcode/raw/training_600_c32cee/` (24 instruction
starts, 98 operations).

The exact entry prefix `$C32CEE-$C32D1D` is reconstructed in
`source_amiga/observed/consume_message_sequence_selector_prefix.asm`. It
contains the inhibit test, selector-word load, head-word publication, and the
initial active-sequence branch. Its raw return targets and divergent
continuations are intentionally outside this slice.

The same packet's executed state guards at `$C32E1A-$C32E2D` and command-event
lookup at `$C32E5C-$C32E7B` are reconstructed in
`guard_message_sequence_state.asm` and `lookup_message_command_event.asm`.
They prove a positive word at `$C45744` returns through the empty-selector
path, a mode byte of at least 2 takes the event-guard path, and a zero event
byte at `$C457E1 + sign_extend($C457F8)` returns without an event. The
nonzero-event and negative-guard continuations remain raw.

The shared six-byte fall-through at `$C32D1E-$C32D23` is separately
reconstructed as `clear_message_control_before_record_selection.asm`.  It
clears byte `$C457F6` before entering `$C32D24`; its caller and control-byte
ownership remain unassigned.

## Run060 success-selector handoff

At the sealed run060 checkpoint taken at GUI frame 9,275, the breakpoint at
`$C32D1E` is reached on the tenth following no-input frame (native frame
9,285).  The preceding static prefix supplies the exact immediate dataflow:
`$C32CEE` sees clear inhibit and active bytes, reads head word `$004A` from
`$C4574A + $C457C6`, and falls through `$C32D1E` into `$C32D24` with
`D0=$004A`.  The captured state at entry records:

```text
$C457C6 = $00          selector byte cursor
$C457C3 = $00          active-byte value on this branch
$C45871 = $00          inhibit-byte value on this branch
$C4574A = $004A         head selector word
$C4574C = $0053         next queued word (not consumed by this packet)
$C45772 = $004A         current-selector publication
```

The bounded trace is
`build/run060_frame09275_c32d1e_trace/trace.jsonl`; the companion selector
trace proves that selector 74 resolves to the `LANDING SUCCESSFUL` payload.
This ties the first success-line selector to the live sequence storage rather
than an arbitrary caller register.  It does not identify the sequence writer,
interpret selector 83, or establish any result/persistence decision.

The observed route tests `$C45871`, loads a word from the table at `$C4574A`
using byte offset `$C457C6`, and stores it at `$C45772`. It then follows
observed guards at `$C457C3`, `$C45744`, `$C457E0`, and `$C457F5`, finally
reads one byte from the raw command-event array `$C457E1` using index
`$C457F8`. That byte is zero in this packet, causing the shared `$C32CEC`
return. The table and queue’s wider ownership remain unassigned.
