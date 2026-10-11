# run060 root pose motion timeline

Classification: **scenario-backed dynamic pose evidence**. This establishes
that the root record's `+$14/+18/+1C` tuple is moving state during the
qualification flight segment. It does not yet distinguish an aircraft pose
from a camera pose that follows it exactly.

The sealed replay was sampled every 25 frames through frame 4000, reading all
root-record words. The three longwords form a coherent moving tuple after the
qualification setup at frame 189:

| Replay interval | `+$14` | `+$18` | `+$1C` | Observation |
| --- | --- | --- | --- | --- |
| 201--676 | `$11982C00` | `$00007708` | `$1059A000` | initial placed tuple remains stable |
| 701--901 | `$11982C00` | `$00007708` | `$1059A114 -> $105BB9C4` | one horizontal component advances |
| 926--1751 | `$11982C00` | `$00007448 -> $00010F60` | `$105C358C -> $10940664` | vertical and horizontal components advance together |
| 1801--1975 | `$11982C00 -> $11A2A120` | `$0001EA00 -> $000002EE` | `$109A1A50 -> $10A9E44C` | all three components change during recorded joystick events |
| 2072 | `$11982C00` | `$00007708` | `$1059A000` | tuple returns to its qualification-start values on recorded event `[J, 0, 7, 1]` |

The middle component continues to use the previously proven cockpit scale
(`>>10`, then `*5`): for example `$00007708` is 145 feet, while the sampled
values around frames 926--1751 vary along with the motion segment. The first
and third components are consumed with it as the projection-base vector.

Therefore the root triple is not merely a static terrain origin or renderer
constant. It is a scenario-backed **moving pose tuple** for the active
qualification flight. The reset at frame 2072 also confirms that the frame-189
tuple is a reusable flight-start placement, not a one-time display cache.

Authority: `build/run060_root_record_first4000_25_frame_samples` and the
projection/formatter contracts cited by `data/root_record_pose_candidate.md`.
Sampling at 25-frame cadence does not identify the per-frame integrator or
which joystick axis controls which component.
