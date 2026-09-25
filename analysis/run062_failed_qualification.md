# Run062 — deterministic failed qualification return

Authority: sealed `captures/run062`, pinned Engine9000 v0.62-alpha, and the
native host-scheduler renderer/checkpoint tools. The run has 195 events, last
input frame 2,470, and uses the same canonical boot state as run060:

```text
760d729341bebb9d6aa49450e7c7a6b760fd2d35321e9bc1e4c5f09c4015a4f4
```

Two native restore probes reproduce that state exactly. Rendering with
`scripts/render_run.py --run captures/run062 --output <new-directory> --every 5`
produces 495 frames through 2,475. The terminal native frame is the top-level
F/A-18 menu, not an active cockpit or a qualification-success message.

## Checkpoint-to-menu oracle

`scripts/capture_run_checkpoint.py` captured the live native state at GUI frame
2,470 (`build/run062_frame2470_checkpoint/frame_02470_state.bin`). A fresh
direct-core continuation from that exact state ran five frames with no recorded
input and reached chipset frame 2,475. Its screenshot and native render have
the same RGB pixel SHA-256:

```text
865d97c3f848119a7cca79cc41aba6167736c34ab57dbdcf2ed244f9814fa914
```

The bounded 53,576-instruction trace is at
`build/run062_frame2470_noinput_trace/trace.jsonl`. This validates its frame
window as a result-to-menu analysis oracle; it does not prove that every
instruction in the window belongs to the failure predicate.

## Scope of the claim

Run062 proves a deterministic failed-qualification outcome followed by a menu
return. It does not yet identify the failure test, the status writer, a result
message payload, pilot-log persistence, or a causal difference from run060.
Those require comparing the native success and failure checkpoints at matching
state-transition boundaries rather than assigning meaning from their final
screens.
