# Run024 crash-result return rebase

`pcode/raw/run024_crash_result_to_menu_rebased_trace/` is a bounded no-input
sample of the return path after the observed crash-result presentation in the
sealed qualification replay (`captures/run024/`).

## Construction

- Restore state: `build/run024_crash_result_trace/trace_final_state.bin`.
- The source trace ends at hardware frame 19,254, after a trace window that
  began at frame 19,251.
- From that restore, run 146 ordinary no-input frames and then trace two
  frames (hardware frames 147--148 relative to the rebase).
- The export contains 636 observed RAM instruction starts, 2,432 RAM
  instruction bytes, and 39 observed RAM call targets.

## Limits

This is evidence for code reached along this bounded return sample only. It
does not identify the qualification-status owner, establish mission pass/fail
logic, or prove equivalence to a later uninterrupted replay: the source state
was produced by a stepped trace window, so held-input/autorepeat equivalence
still requires a separate ordinary-replay validation.
