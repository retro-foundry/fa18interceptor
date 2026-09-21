# Run 024 qualification-selection chain

Authority: sealed `captures/run024/playback.e9k`, plus bounded no-future-input
traces made from its frame-584 prefix using `scripts/clip_replay.py` and
`scripts/trace_from_breakpoint.py`.

## Proven chain

The original replay delivers frontend key `5` (code 53, character 53) at frame
584.  The following links are direct trace facts:

```text
frame 584 frontend key '5'
  -> $C1BD78 observes D4 low byte = 4
  -> $C1BE46 normalizes D4 := 9
  -> $C1BDEC stores byte 9 at $C458A6
  -> $C0FCB4 reads positive $C458A6 = 9
  -> $C0FD68 compares it with 9
  -> $C0FD74 writes selector 105 at $C4574A
  -> $C0FD9C clears word $C4574C
```

The static, byte-exact `$C32D24` selector maps code 105 to descriptor
`$C3F2FF`, whose payload begins:

```text
5 ... QUALIFICATION: REQUIRED FOR MISSIONS
```

The normal replay's frame-600 keyframe visibly shows the qualification-gate
screen.  Together, these facts establish that the recorded frontend `5` event
selects the qualification menu path in this scenario.  They do not establish a
general keyboard mapping outside the recorded menu context, nor any persistent
qualification-status byte.

After the display delay, the same scenario reaches `$C0FEEA` at frame 627.
Its mode-9 jump-table lookup selects `$C10102`, clears the selector-sequence
head, and installs callback `$C101FC`.  This is the bounded post-gate route;
see `analysis/routines/c0fece_delayed_menu_transition.md`.  It is not yet a
gameplay-state or persistence interpretation.

The next equality wait succeeds at frame 713 (`D1=15`) and installs callback
`$C10678`; see `analysis/routines/c10228_advance_post_gate_match_callback.md`.
This is a callback/timer handoff, not a claim that `$C10678` has started the
qualification flight.

At frame 750, `$C10678` queues selector 71 and installs `$C1072E`.  Selector
71 resolves to the embedded `CRACKED BY A-HA` credit string, not a mission or
qualification message; see
`analysis/routines/c10678_post_gate_interstitial_selector.md`.

At frame 790, that interstitial advances to whitespace-only selector 4 and
callback `$C1075A`; this is a credit-clear transition, not a game message.
`$C1075A` then takes its zero guard and returns without writes, ending the
observed callback/text chain.

## Trace artifacts

- `build/run024_qualification_select_dispatch_trace02/` is an 18-instruction
  trace ending at `$C3318E`; it proves `D4=4 -> $C458A6=9`.
- `build/run024_qualification_menu_append_trace03/` is a 32,473-instruction
  trace ending at `$C0FE32`.  Most instructions are the invoked display-buffer
  clear at `$C2FD22`; its final rows prove selector-105 append and terminator.
- Both traces use a generated playback prefix with no events after frame 584;
  the press was delivered under ordinary replay before instruction stepping.
