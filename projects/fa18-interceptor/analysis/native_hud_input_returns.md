# Native HUD returns consumed by recorder input — 2026-10-07

The existing native flight loop now carries defined HUD drawing results into
the next first depleted pending flare/chaff command. Previously a frame without
a final message assignment or periodic page clear left this input undefined.
This batch removes that dependency for the reconstructed bar/text/redraw paths.

The connected caller is native entry -> frontend -> native_flight_tick ->
native_hud_draw -> existing HUD owners. Native composition remains in
port/game/native; shared renderers remain in port/game. No captured return or
CPU register file supplies native gameplay.

## Source contracts

- C30CC4/C30CE0 leaves the actual destination used by bar fills. The existing
  fill implementation now returns that destination. C310E2 bounding preserves
  this value when it clips a fill. Indicator bars retain their original
  B/C/E order and clipped/successful early exits. C30D34's image destination
  supersedes its preceding fill when selected. A final marker-line child has
  an unfinished return and invalidates the result.
- C32794 loads each character at C327AA even when clipped. Accepted characters
  select their actual glyph at C327D6; C32806 preserves that address. The shared
  small-text owner returns the last character or glyph selection. Its separate
  odd-destination fault contract remains unresolved. Scale/message owners
  expose this result; their no-assignment exits preserve preceding HUD output.
  A selected info formatting pass whose context suppresses all text remains
  unresolved rather than claiming preservation.
- Native HUD composition follows mode bar -> scale -> message -> indicator
  bars. Earlier HUD readouts remain unresolved when these final owners do not
  define an output. Selected grid, labels, debug overlays or selection cleanup
  that publishes a view command invalidate the result. Their initial skip
  gates preserve it. Existing timer/sample owners preserve the value.
- C082B8 explicitly constructs the three-pass redraw count and preserves it
  through its remaining stores. The shared cockpit redraw owner now returns
  that existing count. The periodic clock owner applies it at the original
  `(saved_tick & 31) == 16` gate, after HUD drawing and before labels/messages.
  An independent full-frame comparison caught this overwrite; retaining the
  preceding glyph at that boundary was incorrect.

The frontend's final message owner can supersede these outputs as before.
The input dispatcher consumes only a known completed result for the first
depleted pending command and invalidates it after a dispatched command.
Unknown paths still fail explicitly when this live input is needed.

## Validation

`python tools/native/check_countermeasures.py` starts ordinary disk/key Free
Flight using the playable runtime objects. Twelve additional consecutive
completed bodies exercise bar destinations, glyph addresses and a periodic
redraw count. UPDATE_TICK is never seeded. Each body is followed by controlled
first-depleted recorder input, covering modes 1/2/3, flare/chaff, stocks 1/$80
and raw/translated queue wrap positions. The original body oracle independently
computes the return; its result supplies only the original input comparison.
Native gameplay never reads reference values.

| Scope | Result |
| --- | --- |
| Complete actual flight bodies | 43 match compared RAM and all drawing-page bytes: four keyboard, fifteen message, twelve page-clear and twelve HUD/redraw bodies |
| Actual recorder input parents | 60 match compared RAM: 24 claimed/chained and twelve each from final messages, page clears and HUD/redraw results |
| Focused HUD components | Three ordinary runtime snapshots each pass 135 original non-stack RAM comparisons and 39 independently derived defined returns; active, clipped and no-redraw paths included |
| Existing input/control contracts | 2,512 input parents, 256 control-effect cases, two $FD parents and four actual collision parents pass |
| Affected native checks | Host keys, scene exit, frame body, frontend, HUD, frame tail, input, game input and automatic artifact cleanup pass |
| Builds | Native Release/Debug and both reference MSVC runners pass |

Passing RAM remains temporary. Logs/metadata are under
build/native-flight/countermeasure-check; no HUD/display exclusion changed.
This establishes the connected paths and bounded component contracts, not
general depleted-recorder or whole-game acceptance. Earlier/clipped HUD
producers, final marker lines, active grid/labels/debug tails, intervening
commands, complete scenario acceptance, typed-state migration, audio fidelity
and measured 20 ms performance remain unfinished. The goal is active.
