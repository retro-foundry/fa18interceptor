# Native message return consumed by recorder input — 2026-10-07

The final C32CEE message renderer now publishes its character/glyph low byte
through `MessageSequenceResult`. `native_frontend_tick()` retains that result
only when a completed frame actually defines it. `native/menu.c` consumes it
for the first depleted pending flare/chaff command with KEY_TAKEN zero.
No-message, disabled and delay exits do not define zero. The first command
consumes validity; other command returns and drawing-only frame returns still
need their own contracts. Unknown consumed values retain the explicit failure.

The playable caller is native entry -> frontend -> flight -> C0F3C4 input
composition -> pending selection/C1AC28 -> C1C0E0/C1C172 -> C25704 -> C1C23C.
The removed dependency in this scope is the implicit return from the preceding
frame's final message renderer. It is a domain return, with no CPU register file,
register observer, source-capture input or fixed replacement event in native code.

Original C32CEE byte loads and character/glyph operations define the value.
C33066 replaces the character with the glyph address; C330FE uses that address
without replacing D4. C3316A saves/restores D0-D7; C25246 uses D0/D1 only.
C1643A/C0EF08 use D0/D1 and OS calls preserving D4. Between frames, C1612C
uses D0/D1 and graphics calls preserving D4; C2F558 uses A0/A1. C0EFD4's LINK
and the keyboard-only C0F3C4 prelude preserve the incoming value before pending
selection. C1C23C consumes only its low byte for release/claim and queue stores.

`python tools/native/check_countermeasures.py` starts ordinary Free Flight,
then queues existing source menu messages as controlled validation input.
Thirteen captured flight bodies execute the actual frontend's final renderer;
twelve are followed by a first depleted pending command. Two further bodies
exercise no-selector and disabled-renderer exits and clear native validity.
The original frame-body oracle independently derives D4, checks its low byte,
then supplies that original result to the original input oracle. Native gameplay
never reads this reference value.

| Scope | Result |
| --- | --- |
| Real C32CEE component owner | 8,192 calls match all registers, PC, SR and RAM using existing real-child fixture configuration |
| Actual flight bodies | 19 match compared RAM and every drawing-page byte: four keyboard countermeasure bodies plus fifteen message/return bodies |
| Actual recorder input parents | 36 match compared RAM: 24 existing claimed/chained cases plus twelve first-depleted message returns |
| First-depleted variants | Recorder modes 1/2/3, flare/chaff, stock 1/$80, raw/translated queue wrap positions; naturally produced glyph bytes cover both release-bit states |
| Existing input/control contracts | 2,512 input parents, 256 control-effect cases, two $FD input parents and four controlled collision parents pass |
| Affected runtime regressions | Host keys, frontend, menu start, input, replay and artifact cleanup pass |

Passing RAM remains temporary; reports/logs remain under
`build/native-flight/countermeasure-check/`. No drawing/HUD exclusions were added
and no full emulator replay was repeated. Native Release/Debug and reference
MSVC runners build. The public native Release executable is refreshed.

Drawing-derived returns (fill bars, text, markers/labels and frame-tail paths),
returns after intervening commands, complete gameplay sequences, typed-state
migration, audio fidelity and measured 20 ms acceptance remain unfinished.
This is a connected subset of first-depleted recorder behavior, not full port
acceptance.
