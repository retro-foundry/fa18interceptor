# run075 frame-392 `$C0F090` parent update

Classification: **scenario-backed call order and branch outcome**. This is a
single no-future-input trace after normal run075 replay; it does not identify
the physical meaning of the child records, matrices, or renderer page.

Authority:

```text
python scripts/trace_from_breakpoint.py \
  --restore captures/run075/initial_state.bin \
  --playback captures/run075/playback.e9k --playback-frame-offset 200 \
  --address 0xC0F090 --arm-frame 184 --return-pc 0xC0F124 \
  --frames 200 --max-instructions 120000 --ignore-future-input \
  --output build/run075_frame384_c0f090_parent_full
```

The breakpoint hits at local frame 192 (global frame 392 under the recorder's
frame labels) and returns to `$C0F124` after 31,800 instructions.

| Trace index | Parent site | Target/outcome |
| ---: | --- | --- |
| 3 | `$C0F0A6` | `$C1CB14` |
| 30 | `$C0F0B4` | `$C1CB26` |
| 1,434 | `$C0F0C2` | `$C279D0` |
| 7,144 | `$C0F0E0` | `$C265E8` |
| 7,288 | `$C0F0E6` | `D0=0` |
| 7,291 | `$C0F110` | `$C1CCBC` |
| 31,510 | `$C0F11E` | `$C1518C` |

Thus the trace takes the source false branch at `$C0F108`: it runs the
follow-up helper before the branch helper. The initial signed-stage comparison
does not skip (`$C0F09A` falls through), and `$C457AD` is zero at `$C0F0D0`.
This confirms the native `FA18_PARENT_FLIGHT_UPDATE_DECISION_FALSE` order for
this invocation, without supplying a frame-number schedule or making the
unresolved callbacks concrete.
