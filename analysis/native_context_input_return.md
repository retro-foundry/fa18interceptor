# Native context camera and zoom/message outputs - 2026-10-07

Context calculation now reaches the existing matrix and observer owners in
the playable native runner. Context commands expose their actual record
selection, local height, preset displacement or map-origin result to later
depleted recorder input. Request and gated commands preserve their preceding
output; successful queue publication retains ownership of its translated index.

The connected caller is native entry -> frontend -> native_input_process ->
native_menu_dispatch_raw/pending -> command_dispatch ->
execute_context_command_result -> native/menu.c context_child ->
local_to_world/set_observer_position. This removes the two aborting context
children from this runtime path. Game behavior remains in context_commands.c,
matrix.c and view.c; native/menu.c supplies composition and typed return
ownership. The existing event API delegates to the same command body.

## Original contracts

- C091E0 transforms the selected local point through the record's inverse
  matrix and adds its world position. Its input local Y survives the transform.
  Aircraft kind $20 selects (-36,47,-48); the other branch selects (0,20,-106).
  These existing source values now reach both camera composition and output.
- C0915A stores the source observer position, masks X/Z to $3FFFFF and stores
  the negated observer coordinates. Its returned event is the actual stored
  observer X. It preserves the preceding local-height or preset displacement
  output; native state does not shadow CPU registers.
- C1B74E's preset branch shifts the actual table-loaded X displacement by
  eight. A later record-context start supersedes it with the actual VIEW_RECORD
  offset. Enabled-origin calculation preserves the local-height/displacement.
- Fresh map entry computes the masked/negated fixed-origin X before its gates.
  Record-context copy replaces that result when selected. Cached map entry also
  publishes the actual copied record offset. Request/voice release assigns no
  new action output. Event publication and action output remain distinct.

## HUD gap exposed by the connected camera run

Cleanup body 85 matched gameplay RAM and drawing but lost its native output.
The original C0F274 call to C31ACC supplied a zoom text result that survived
the later gated drawing calls. draw_zoom_readout now returns its actual
TextDrawResult, and native_hud_draw composes it in the existing source order.
The inactive display-update gate preserves the preceding result.

The message owner had also invalidated every skipped text result, even when
no formatting occurred. C32494/C324C6/C32506 assign the decimal zero policy
for altitude/heading/speed. C3267A and its packed-BCD child preserve that policy;
C32662/C32622 can then skip text under context gates. message_line.c now
returns the actual selected policy on that path. No-format skips preserve their
preceding result; actual text character/glyph results supersede formatting.
The native HUD composes this named result without an oracle-provided value.

## Runtime integration evidence

The extended ordinary disk/input Free Flight fixture adds twelve complete
bodies followed by twelve context/map/request keyboard commands and depleted
recorder parents. It uses the loader's actual pose and preset tables, including
both record-relative rows, origin gates and the cached-map branch. Every added
keyboard parent requires its intended owner and value, including local heights
47 and 20. Existing physics, drawing and input comparisons remain included.

187 complete bodies match compared gameplay RAM and both drawing pages;
204 recorder parents and 84 intervening keyboard parents match RAM and defined
returns. Original parent outputs feed only subsequent original parents. Native
gameplay computes its own outputs. No new drawing/RAM exclusion or fitted
clock/output seed was added.

The final two added bodies take the real idle POST_INPUT_AUX=0 branch after a
request command. Their RAM/drawing match, but their inherited output remains
explicitly unresolved. They remain in the comparison. The following context
commands assign independently checked outputs before depleted recorder input.
This run does not establish idle/stage return preservation generally.

## Component evidence and limits

12,576 selected command parents match non-stack RAM and defined low-byte
returns: 2,782 preserve prior output, 1,698 publish selection, 508 queue,
3,218 flight action, 1,318 view action, 2,260 indexed action and 792 context
action. None is unresolved within this bounded selection. The additional
1,888 parents cover special context calculation across the five actual pose
rows, map/detail/origin/cache gates, request/recorder gates and pending
map/refresh/calculation commands. Existing 2,512 input parents and sixteen
Delete parents remain checked.

The focused HUD suite adds zoom returns and sixteen context-message/zoom
variants. It requires six skipped-text decimal-policy assignments, alongside
the existing odd-destination and marker-line output checks. Component return
coverage is separate from the complete-body runtime evidence above.

Three fresh startup/flight snapshots each pass 203 HUD non-stack RAM cases
and 128 defined returns, including eleven odd-destination faults, two marker
lines and six skipped-text decimal policies (609 RAM cases/384 returns total).
Native Release/Debug and both reference MSVC runners build. Ten affected
native CTests pass: host keys, scene exit, frame body, frontend, HUD, frame tail,
input, game input, qualification and artifact cleanup. The gameplay comparison
command is `python tools/native/check_countermeasures.py`; focused component
checks are `fa18_native_input` and `fa18_native_hud` in native Release CTest.

Full independent missions/combat/outcomes, stage/reset and other unresolved
return contracts, readable typed state, audio fidelity and measured 20 ms
frame performance remain open. The next concrete return gap is the idle frame
entry's unconditional invalidation and the actual notification/control-action
owners before its skipped overlays; preserve only outputs whose original
contracts can be demonstrated.
