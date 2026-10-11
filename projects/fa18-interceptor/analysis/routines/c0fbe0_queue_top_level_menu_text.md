# `$C0FBE0`: top-level menu text-sequence writer

Classification: **static behavioral data layout, corroborated by a scenario
snapshot**.  Byte-exact source:
`source_amiga/observed/queue_top_level_menu_text.asm`.

## Proven writes

After calling `$C11312` to reset sequence-head/control state, this routine
writes the following 16-bit values at `$C4574A` and terminates them with zero:

```text
6, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 0
```

It then installs `$C0FCB4` at callback slot `$C1820C`.  The selector and text
consumer contracts resolve these values to title, prompt, the eight top-level
menu choices, and the Esc instruction; the complete mapping is in
`analysis/run024_flight_return_text_selection.md`.

## Scenario corroboration and limit

The matching `run024_flight_return_trace` Slow RAM snapshot contains exactly
this sequence at `$C4574A`, and its trace observes `$C32BD2/$C32CEE/$C32D24`
consuming code 109 from that sequence.  The trace window begins after
`$C0FBE0`, so the writer's own execution is static evidence, while the stored
sequence and its consumer are scenario-backed evidence.  This establishes the
flight-return top-level menu batch but does not establish every branch or
screen effect of `$C0FBE0`'s helper calls.

`$C0FBE0` installs `$C0FCB4` to poll the following selection stage.  The
flight-return capture observes its idle zero-value path, while its static
positive branches append individual Demo/Free Flight/Training/Qualification
or advanced-mission selector codes.  See
`analysis/routines/c0fcb4_top_level_menu_followup.md`.
