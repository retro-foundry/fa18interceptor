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

In ordinary run075 replay, after the mode-$7F arm sets `$C45AD6=4`, live
`$C0F5F8` entries occur at frames 290, 317, 335, 351, and 369 with countdowns
4, 3, 2, 1, and 0 respectively. Its decrement-before-dispatch tail makes the
last entry call `$C0FA04` with `-1`, matching the bounded frame-370 expiry.
Those five replay-owned ticks are present in `captures/uae/run075/timing.e9t`; no
general presentation-frame cadence is implied.

The run075 frame-291 trace proves the nonnegative call reaches `$C2FD22`;
`FA18DemoController` maps that clear to its native chunky work buffer.
`fa18_demo_contract_test` reaches the expiry after the five ticks required to
decrement the entry delay from 4 to -1.

## Source-timed registered bridge

`glue_C0FA04_step` covers all 15 instructions in `$C0FA04-$C0FA4A`. It keeps
the parent resumable while its `$C2FD22` buffer-clear child runs at source
instruction boundaries and while the expiry path dispatches `$C0FAA4`. The
former fixed 40,000-cycle return charge is removed.

The direct instruction oracle matches 15 instructions over 480 fixtures,
including registers, full SR, PC, cycles and RAM. The combined bridge oracle
matches 1,178 instructions over 37,696 fixtures. Fresh isolated source-OFF and
`PORTS_ONLY=C0FA04` streams match all 36,236 frames in the three sealed native
recordings. GNU and MSVC Release builds pass. The full 414-entry gate remains
clean at 721,752 shadow matches and 1,169,610 sandbox matches, with sealed RAM
and identical poison frames.

With C0FA04 corrected, a 500-frame all-registered demo probe moves to frame 416
with 361 differing pixels, the same result as isolated C0D752.
