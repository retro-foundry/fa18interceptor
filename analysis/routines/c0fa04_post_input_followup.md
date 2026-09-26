# `$C0FA04`: timer-gated post-input callback

## Evidence

Static disassembly of `$C0FA04-$C0FA4B`, reconstructed byte-for-byte in
`source_amiga/observed/finish_post_input_followup.asm`. `$C0F992` installs this
callback when its `$C4584B == 3` route is selected.

On a negative `$C45AD6`, it calls `$C0FAA4`, writes the recorded state values,
and installs `$C0FA4C` as the next callback. While the countdown is
non-negative it calls `$C2FD22`.

The run075 expiry packet reaches this callback at direct-core frame 370 with
delay `-1`. It executes `$C0FAA4` before its own stores; its trace return is
at index 1128, followed by the direct `$C0FA04` stores at indices 1129 through
1138. Native `fa18_expire_run075_demo_entry` preserves that observed order:
it first applies the bounded `$C0FAA4` direct state subset and then records
the caller's command mode 3, cleared auxiliary/input values, delay 2,
followup mode `$0F`, and typed `FA18_MENU_CALLBACK_DEMO_FOLLOWUP_MATCH`.
The direct initializer subset and unresolved nested helpers are documented in
`c0faa4_run075_scene_initialization.md`.

The run075 frame-291 trace proves the nonnegative call reaches `$C2FD22`;
`FA18DemoController` maps that clear to its native chunky work buffer.
`fa18_demo_contract_test` reaches the expiry after the five ticks required to
decrement the entry delay from 4 to -1.
