# Observed in-flight line-emission call path

This is a call-edge record for hardware frame 602 of
`build/attract_focus_600`; it is not a complete call graph.

```text
$C21304  [Hunk 14 +$2A4] jsr $C2EE4A   (12 observed calls)
  $C2EEAC  [Hunk 35 +$23C] bsr $C2F0F4 (one observed edge in this packet)
    $C2F088 [Hunk 35 +$418] jsr $C2FA7E (12 observed calls)
      blitter_draw_line_to_enabled_planes [Hunk 36 +$5EE]
```

The Hunk-14 entry `$C212B0` is reached through an observed indirect dispatch at
`$C1F942` (Hunk 10 +`$C0A`), not a fixed absolute call. Its exact instructions
mask `D0` with `$3FFF`, index a pointer table at `$C1FCE8`, then execute
`JSR (A0)`. A live Engine9000 breakpoint at `$C212B0` records return PC
`$C1F944`, confirming that edge. The table's discriminator meaning is unknown.

The `$C2F088` caller restores four words from `$C4B390` into `D0-D3` before
calling `$C2FA7E`. Immediately before that it performs signed multiply/divide
and bounds checks against `$0140` and `$00B4`, then stores two words through
`A0`. This supports only the structural description “screen-bounded line-input
preparation.” It does not establish a world coordinate system, object type or
main-loop ownership.

The twelve calls to `$C2EE4A` from `$C21304` provide the next upward breakpoint
target. Capture a two-frame packet at a different demo time before naming either
caller: count changes or shared state could distinguish a primitive list from a
higher-level object loop.

## Bounded target at frame 602

`$C212B0` now has an independent no-input trace from its indirect-dispatch
entry to return `$C1F944`. It executes 2,552 instructions and makes exactly ten
`$C21304 -> $C2EE4A -> $C2F088 -> $C2FA7E` submissions. This establishes a
bounded display-record submission unit for this scenario; it does not identify
the records' owner. See `c212b0_display_submission.md`.

## Cross-check at frames 1801-1802

`build/attract_focus_1800` is a separate packet later in the same scripted
flight. The screen is the cockpit/forward-view state. It has eight calls to
`$C2EE4A` and two calls to `$C2FA7E`; both line-emitter calls arrive from
`$C302B6` (Hunk 36 +`$E26`), rather than `$C2F088`. This confirms that the emitter is shared by
more than one observed caller and that the first packet's twelve-call count was
not a routine identity. It does not yet establish what either caller draws.
