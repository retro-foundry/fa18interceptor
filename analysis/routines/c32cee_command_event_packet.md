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

The observed route tests `$C45871`, loads a word from the table at `$C4574A`
using byte offset `$C457C6`, and stores it at `$C45772`. It then follows
observed guards at `$C457C3`, `$C45744`, `$C457E0`, and `$C457F5`, finally
reads one byte from the raw command-event array `$C457E1` using index
`$C457F8`. That byte is zero in this packet, causing the shared `$C32CEC`
return. The table and queue’s wider ownership remain unassigned.
