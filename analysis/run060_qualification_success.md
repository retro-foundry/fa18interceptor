# Run060 — deterministic successful qualification

Authority: sealed `captures/run060`, the pinned Engine9000 v0.62-alpha Amiga
core, and the native host-scheduler renderer introduced in commit `431b72d`.
This is the qualification-success scenario authority. It replaces run029 as a
success candidate; run029 remains useful only as an earlier, outcome-unproven
qualification selection.

## Reproducible result

`run.json` records 531 events, explicit boot restore frame 120, last input
frame 10,085, and canonical restored state:

```text
760d729341bebb9d6aa49450e7c7a6b760fd2d35321e9bc1e4c5f09c4015a4f4
```

Two independent runs of the native restore probe reproduce that exact 3,145,728
byte state. Render the native full replay with:

```powershell
python scripts/render_run.py --run captures/run060 --output build/run060_render_every5 --every 5
```

The command refuses an existing output directory, restores the sealed state,
checks the restored-state hash, and writes 2,017 native frames (`5` through
`10,085`). The present frame `build/run060_every5/9545.png` has SHA-256:

```text
2ec29c53b4fd76d25a24fcd19cb0793673245e8f32e49d212777e18481f6a8c0
```

It visibly contains both `LANDING SUCCESSFUL` and `YOU ARE NOW QUALIFIED FOR
MISSIONS`. The operator observed the fully drawn text at Engine9000 GUI frame
9,544; the every-five-frame native export samples it at frame 9,545.

## Input anchors

| Replay frame | Recorded input | Established fact |
| ---: | --- | --- |
| 5,086 | `A` press | Recorded flight input. |
| 5,089 | `A` release | Recorded flight input. |
| 9,541 | Space press | Recorded flight input while the message is drawn. |
| 9,546 | Space release | Recorded flight input. |

The visible text begins character-by-character before its full frame. These
events do **not** establish that `A` or Space is the success trigger: their
causal paths and the qualification-status writer remain untraced.

## Scope of the claim

This is a `scenario`-level result: a deterministic replay reaches the visible
qualification-success screen. It is not yet a behavioral contract for the
static payload at `$C3FC3C`, the text compositor, carrier-landing test, pilot
log mutation, or mission-unlock state. The direct-core bridge previously used
for screenshot extraction is not a visual oracle for run060 because it bypasses
the native host input scheduler. Future bounded instruction analysis must begin
from a native replay checkpoint at the message interval.

The current pixel-validated no-input continuation from native GUI frame 9,546
to frame 9,550 executes the message-sequence idle path at `$C32CC4-$C32D08`.
It contains no start in `$C31F4C-$C31FFF`, including the static
qualification-record/line path. That is a boundary on this particular late
checkpoint only: it neither proves that the earlier result transition skipped
those routines nor supplies their branch outcomes.
