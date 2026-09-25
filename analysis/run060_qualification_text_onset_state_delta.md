# Run060 text-onset native state delta

Authority: two fresh native checkpoint captures from sealed `captures/run060`:
GUI frame 9,285 (the last sampled frame with no measured result-text pixels)
and GUI frame 9,290 (the first sampled frame with 28 such pixels).  The
visual boundary is documented in [run060 qualification-text onset](run060_qualification_text_onset.md).

Reproduce the state boundary without changing the sealed recording:

```powershell
python scripts/capture_run_checkpoint.py --run captures/run060 --frame 9285 `
  --output build/run060_frame09285_checkpoint
python scripts/capture_run_checkpoint.py --run captures/run060 --frame 9290 `
  --output build/run060_frame09290_checkpoint
python scripts/engine9000_bridge.py --restore build/run060_frame09285_checkpoint/frame_09285_state.bin `
  --frames 0 --output build/run060_frame09285_slow
python scripts/engine9000_bridge.py --restore build/run060_frame09290_checkpoint/frame_09290_state.bin `
  --frames 0 --output build/run060_frame09290_slow
python scripts/diff_slow_ram.py build/run060_frame09285_slow/slow.bin `
  build/run060_frame09290_slow/slow.bin
```

The Slow-RAM comparison reports 126 changed bytes in 49 contiguous ranges.
Most are expected mutable execution, display, and stack/workspace regions and
remain unassigned.  The small message-state changes are:

| Address | Frame 9,285 | Frame 9,290 | Established limit |
|---|---:|---:|---|
| `$C45746-$C45747` | `$0000` | `$FFF2` | Existing message-layout state word changed; writer untraced. |
| `$C45749` | `$05` | `$02` | Adjacent layout state changed; ownership unassigned. |
| `$C45775` | `$FA` | `$FF` | Message workspace byte changed; meaning unassigned. |
| `$C457C1` | `$FF` | `$0D` | Message workspace byte changed; meaning unassigned. |
| `$C457DE` | `$01` | `$00` | The byte is decremented by reconstructed `$C32E7C-$C32EAD` when that consumer's positive path executes. This state delta alone does not prove that path executed in this interval. |

This is an exact native pre/post state boundary around the first visible sampled
text pixels.  It narrows future producer tracing to the changed state and its
writers, but does not identify the qualification predicate, success writer,
text payload, or rendered-character timing.
