# Mission-selection F1/F2 baseline probe

Two sealed recordings start from `captures/baseline_menu/state.bin` and use
the same initial configuration.  They test only the menu route; neither is a
mission-selection success oracle.

| Run | Recorded inputs | Last input frame | Focused replay window |
| --- | --- | ---: | --- |
| `run043_mission_selection` | `6`, then `F2` | 1263 | frames 1400--1402 |
| `run044_mission_selection_f1` | `6`, then `F1` | 636 | frames 800--802 |

Each focused export contains 443 observed RAM instruction starts, 1,756
observed RAM instruction bytes, and 36 call-target candidates.  The sampled
screen hash is the same in both exports:
`d58eed69db393075fbee7e93b7dd709e386407fb66af0b6a03d97cabcdebb8aa`.

The F1 and F2 probes therefore do not enter a distinct mission-specific
instruction set in their sampled windows.  This is evidence of unchanged
observed execution only; it does not identify qualification ownership,
availability-state bytes, or the reason either route does not transition.
