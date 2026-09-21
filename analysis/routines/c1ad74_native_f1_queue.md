# Native F1 queue route at `$C1AD74`

Classification: **bounded input-routing evidence**.

The native Engine9000 event `K 282 0 0 1` (Libretro F1) is replayed from the
verified `free_flight_600` state. There is no later event in the replay. A
breakpoint at `$C0F43A` hits on frame one, and the packet returns to `$C0F45C`
after 288 instructions.

The game dispatcher receives `D0 = $50` at `$C1AD74`. Its observed branch
falls through to `$C1C23C`, which sets `$C457A3`, appends the raw input to the
bounded queue at `$C457E1`, translates through `$C331CE`, clears the three
input bytes at `$C45878-$C4587A`, and returns.

The queued byte is consumed by `$C32CEE` in the same frame and returns to the
parent at `$C0F3C0` after 47 instructions. That packet reads raw `$50`, looks
it up through `$C331CE`, obtains zero, and returns via `$C330F4`. Thus this
native F1 representation produces no observed game command in this state. It
must not be treated as function-key gameplay coverage.

Evidence:

- `build/run013_f1_keyboard_poll/trace_summary.json`
- `pcode/raw/run013_f1_keyboard_poll/`
- `build/run013_f1_command_event/trace_summary.json`
- `pcode/raw/run013_f1_command_event/`
- byte-exact queued-input tail:
  `source_amiga/observed/enqueue_command_input_event.asm`
