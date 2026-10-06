# Native menu-to-flight stage ordering

The actual path is `native_frontend_tick` -> `native_menu_tick` -> C0FCB4's
`follow_top_level_menu`, then the native flight frame. Previously that same
frame called C0F5F8 again, immediately executing newly published C0FECE and
decrementing its 210-update banner countdown to 209. Source C0F5F8 dispatches
one callback; the new callback runs on the following update.

The native flight frame now accepts that its menu stage already ran, continues
notification/frame work, and retains C0F808's key-claim clear without dispatching
C0FECE again. Menu messages now execute after stage publication, at C0EFD4's
final C32CEE boundary, so the new banner begins in the selection update.
The existing source countdown and message owners supply behavior; no extra
delay or fabricated replay offset is introduced.

Real recorded selection at native update 1525 now has C0FECE, countdown $D2,
cleared KEY_TAKEN, message-active 1, and queue $0065/$0000. These agree with the
original start-of-1526 checkpoint. Next native update first decrements to $D1.
The checker covers both boundaries in the actual runner, alongside 18 source
menu setup cases and the pause/queued-selection checks.

A bounded original prefix trace ended at update 2401; its 1 MiB checkpoint is
byte-identical to the existing reference checkpoint. The trace before this fix
isolates the larger remaining discrepancy:

| Stage at update start | Original update | Native before this fix |
| --- | ---: | ---: |
| C0FA04 demo followup | 1737 | 1736 |
| C0FA4C viewport wait | 1742 | 1741 |
| C0FA80 viewport ready | 1822 | 1784 |
| C10CFE context message | 1832 | 1794 |
| C10DAE active context | 2184 | 2146 |

The original performs 80 updates in its viewport wait across 34 PAL frames;
the native runner performed 43 updates across 45 PAL frames. Native C53F88
currently waits for the next whole PAL boundary. The original WaitBOVP boundary
can complete multiple times within a PAL frame; existing outer-loop evidence
also observes up to six iterations per frame. That evidence does not authorize
a fixed six-pass schedule. See `routines/c15d80_outer_update_loop.md`.
This isolates a display/task pacing dependency that still needs a source-backed
native contract. Copper fade remains excluded from pixel acceptance; control
wait ordering still matters.

After the dispatch fix, native end-of-2400 has tick 258 versus original
start-of-2401 tick 222: the measured lead falls **37 -> 36**. Menu banners,
missions/log/save, carrier qualification/save/restart/reload and crash/re-entry
checks pass. Source sound/startup/launch/record comparisons and the complete
4,892-update native demo pass. Final-message ordering preserves all RAM at
both the 2400 and final demo checkpoints. Source prefix captures are read-only
evidence; they never seed native runtime state. No original full replay suite
was run. Temporary trace instrumentation was restored before the native build.

This dispatch correction is **1/1 verified (100% of this scope)**. All six
previously identified missing frame owners remain connected. Full recorded
frame parity remains **0/3 accepted**, and display pacing, async voices/output,
complete mouse callback and other timing/state differences remain open.
