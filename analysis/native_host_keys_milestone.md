# Native rudder and numeric keypad input

2026-10-07. User reported nonfunctional rudder and numeric keypad external
views in the playable native runner. Both were host mapping omissions.

The actual SDL path is `port/native/main.c -> native_host_keyboard_event ->
native_frontend_event -> native_input_enqueue -> native_menu_raw_key ->
native_input_process -> C1AD74` command selection and its existing game owners.
The old native mapper searched C331CE's text translation table to identify
physical keys; that table has zero for comma/period. SDL keypad symbols also
had no native mapping. Neither failure required changing physics or camera
behavior.

`port/amiga/host_keys.c` now owns the reference runner's existing physical
Amiga mapper. Its function bodies are unchanged apart from symbol names,
verified against the pre-change Git version. `port/machine/input.c` retains
its public wrappers and uses the same owner. Native mapping accepts both
SDL2 symbols and SDL1/libretro replay identities, including keypad digits,
operators, decimal and Enter. Press/release preserves raw bit seven.
The existing SDL normalization/repeat policy moved unchanged into
`port/native/host_input.c` so the integration entry uses the actual playable
handler. No gameplay fallback, captured state or invented key assignment is
used.

Source C1B58E/C1B594 select comma/period raw $38/$39 and opposite rudder masks
$80/$40; releases $B8/$B9 reach the zero form C1B59A. The existing command
owner preserves the other packed control bits and publishes C4582F. The
three-axis control owner then changes the aircraft's signed +$29 lane.
Source C1AEF4's alternate-key routes select keypad views: $1E -> mode six,
$2D/$2F -> previous/next mode, $1D/$3D -> modes thirteen/twelve, and $3E ->
zero cockpit mode under the normal flight context. These owners and their
source context gates remain unchanged.

`python tools/native/check_host_keys.py` starts ordinary Free Flight from the
ADF, pushes SDL keyboard events, polls them, invokes the shared playable
handler and advances the actual runtime. It checks both rudder masks, opposite
signed axis changes, releases, ignored repeats and reached view modes
0/5/6/12/13. Keypad cases alternate KMOD_NUM on/off. Mapping contracts cover
all keypad digits/operators/decimal/Enter press/release and legacy digits.

All 26 captured input/stage intervals match complete original C0F3C4/C0F5F8
parents with the existing host OS boundaries. All 52 sampled complete flight
bodies match compared gameplay and every drawing byte using the existing
scratch/async voice/blitter-busy exclusions. No new exclusions or original
full replay were added. Test captures are temporary; JSON and logs remain at
`build/native-flight/host-keys-check/`. These checks exercise the SDL event
queue and game handler, not a physical Windows keyboard automation session.

Native Release/Debug and both MSVC reference runners build. The native
frontend/link omission, menu, queued-input and replay regressions plus the
new host-key runtime gate and automatic artifact cleanup pass. Public native
Release is refreshed. This fixes the reported controls; complete independent
gameplay sequences, inherited depleted-recorder event production, typed-state
migration, audio fidelity and measured 20 ms acceptance remain unfinished.

## Recorder carry trace retained for the continuing goal

An optional `FA18_FRAME_TRACE_INPUT_CARRY` diagnostic in the original frame
oracle reports D4 at tail boundaries and the most recent instruction changing
it. Eight affected runtime bodies still pass with this diagnostic enabled.
The log is `build/native-flight/host-keys-check/input-carry-trace.log`.
At ordinary cockpit boundaries the last change is C30CE0's `add.l D1,D4`,
leaving an instrument drawing address; outside-view samples leave a text
character at C327AA; a plane-top clear leaves zero at C2F5A2. C32CEE can
preserve these values when no message character is emitted. This is evidence
that the first depleted recorder event cannot generally be replaced by zero.
The trace observes changes, so same-value writes are not reported; it is
producer navigation evidence, not a complete data-flow contract or native
behavior. Reconstruct those actual drawing/message return values before
removing the remaining unclaimed-queue abort.
