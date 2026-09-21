# `$C2374C` selected-fire consumer (run004)

Classification: **behavioural packet**. During the isolated run004 later
Space event, the `$30` selected-fire branch writes `$C457BA = 1`. In the same
frame, `$C22DB4` observes that flag and calls
`$C2374C -> $C22DC8`. The child returns in 214 instructions with no future
input and makes no direct calls.

The observed packet immediately clears `$C457BA`, sets `$C457B7 = 1`, writes
`$C45797 = 8`, `$C458B0 = $FB`, `$C457C5 = 1`, writes 3 to `$C45843` and
`$C45844`, and later writes `$8C` to `$C45858`. It also reads several
matrix/control-record fields, including `$C45B50` and `$C459B4`.

Canonical P-code is `pcode/raw/run004_c2374c_second_fire_consumer/` (134
observed RAM starts, 939 operations). These effects prove immediate
consumption of the `$30` selected-fire request but do not yet identify an
ammunition counter or spawned-object record.

The complete `$C2374C-$C23A25` routine is now reconstructed byte-for-byte as
`source_amiga/observed/consume_selected_fire_request.asm` (730 bytes). It
clears the request, checks selection conditions, copies `$29` longwords from
the A2 record to the A1 record, chooses the initialization kind and counters,
configures the copied record, applies a fixed-point orientation transform, and
conditionally calls `$C2574A`. Those operations identify a record initializer;
they still do not establish an ammunition or object-type identity.

The following bounded continuation is `$C23A7E -> $C23F4A`, returning to
`$C22DCC` in 46 instructions. It takes the mode-zero update tail, exported as
`pcode/raw/run004_c23a7e_second_fire_followup/` (46 starts, 254 P-code ops).
The byte-exact `$C23F4A-$C23FC1` tail is
`source_amiga/observed/update_mode_zero_record_motion.asm`; it adjusts the
record heading toward a mode-selected target and returns one. This is update
behavior for the initialized record, not sufficient evidence for its type.
