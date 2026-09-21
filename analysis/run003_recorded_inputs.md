# Run003 recorded input inventory

This inventory is derived directly from `captures/run003/playback.e9k`, not
from the intended plan.  Frame numbers are Engine9000 input frames; a listed
key is a recorded key-down event.  Repeated entries may be deliberate taps or
frontend repeat events, so they are coverage evidence rather than a count of
game actions.

| Frames (first--last) | Recorded key | Planned/documented interpretation |
|---|---|---|
| 341--944 | `2`, Space, `2`, `1` | Menu/setup selections; matches a Free Flight setup route but does not by itself label the selected base/type fields. |
| 1965--2073 | `H` x3 | HUD toggle coverage. |
| 2184--2341 | `M` x4 | Map toggle coverage. |
| 2397--2690 | `R` x7 | Radar-range cycling coverage. |
| 2823--2915 | `T` x2 | Target selection coverage. |
| 2972--3184 | `A` x4 | Arrestor-hook toggle coverage. |
| 3305--3477 | `J` x2 | Manual-documented ECM key. |
| 3615--3701 | `K` x2 | Alternative ECM-key experiment. |
| 3888--4226 | frontend key code 291 x10 | Restored-state replay proves raw `$59`, the F10 end of the `$50-$59` function-key range. It takes the throttle-level route and stores `$79` at `$C45870`; see `c1bd04_apply_function_key_throttle_level.md`. |
| 4790 | `G` | Gear coverage. |
| 4939--5127 | `J` x4 | Additional ECM-key traffic. |
| 5143--5213 | `K` x2 | Additional alternative-key traffic. |
| 5360--5530 | `,` x2 | Rudder-left coverage. |
| 5429--5677 | `.` x2 | Rudder-right coverage. |
| 5775--5955 | `[` x4, `]` x2 | Zoom coverage. |
| 6000--6732 | key/symbol zero | Unlabelled frontend events.  They are not sufficient evidence to call any particular cursor direction covered. |
| 6938--7332 | `P` x4 | Pause/resume coverage. |
| 7609--8309 | Space, Return, mouse, joystick | First Space is raw `$40`; first Return is raw `$44` and takes the documented weapon-cycle handler. Mouse/joystick traffic remains to be isolated. |

The recording also contains mouse motion and joystick button 4/5 events.  They
are preserved as part of the deterministic run but are not translated into
game controls here.  In particular, zero-valued keyboard entries and the
The mouse/joystick traffic must be traced through the raw-key or hardware
handler before it is used to label a specific game command.
