# Run060 success-selector activation boundary

Authority: adjacent native checkpoints from sealed `captures/run060`, captured
at GUI frames 9,284 and 9,285.  Both replay restores the canonical recording
state SHA-256
`760d729341bebb9d6aa49450e7c7a6b760fd2d35321e9bc1e4c5f09c4015a4f4`.

## Exact state transition

The selector queue is not active at frame 9,284.  At frame 9,285 it holds the
first success selector and the standard message path is active:

| State | Frame 9,284 | Frame 9,285 |
|---|---:|---:|
| `$C4574A-$C4574F` | `$0000 $0000 $0065` | `$004A $8053 $0000` |
| `$C457C3` | `$00` | `$01` |
| `$C45703-$C45705` | `$C40EBE` | `$C3FC45` |
| `$C4570F-$C45711` | `$C40EBE` | `$C3FC3C` |
| `$C45708-$C45709` | `$0944` | `$04B4` |
| `$C45714-$C45715` | `$0944` | `$04B4` |
| `$C457DB-$C457DC` | `$06,$FF` | `$05,$03` |

The queue head `$004A` is selector 74; the following bounded selector trace
proves it resolves descriptor `$C3FC38` and publishes payload `$C3FC3C`, the
first `LANDING SUCCESSFUL` line.  The `$8053` word is only an observed second
queued selector word here; this boundary does not interpret it.

The pre/post Slow-RAM comparison reports 244 changed bytes in the full frame.
The table lists the message-path values only, not a claim that all other
changes are unrelated.

## Reproduction

```powershell
python scripts/capture_run_checkpoint.py --run captures/run060 --frame 9284 `
  --output build/run060_frame09284_checkpoint
python scripts/capture_run_checkpoint.py --run captures/run060 --frame 9285 `
  --output build/run060_frame09285_checkpoint
python scripts/engine9000_bridge.py --restore build/run060_frame09284_checkpoint/frame_09284_state.bin `
  --frames 0 --output build/run060_frame09284_state_view
python scripts/engine9000_bridge.py --restore build/run060_frame09285_checkpoint/frame_09285_state.bin `
  --frames 0 --output build/run060_frame09285_state_view
python scripts/diff_slow_ram.py build/run060_frame09284_state_view/slow.bin `
  build/run060_frame09285_state_view/slow.bin
```

## Limits

This narrows the success-message producer to one native-frame transition.  It
does not identify the writer PC, qualification predicate, collision/landing
logic, persistence behavior, or the later second-line selection/visibility.
