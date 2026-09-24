# Run038 Golden Gate distance probe

Classification: **sealed replay landmark measurement; rejected as a
distance-only LOD experiment**.

`run038` is the requested continuous cockpit-view Golden Gate approach. Its
automatic recorder was sealed with 342 events through replay frame 7,294. The
landmark scan deliberately searches only the forward world viewport
`x=40..679, y=18..159` for RGB `#880000`; cockpit and HUD red pixels are
excluded.

| Frame | Red pixels | Bounding rectangle, inclusive |
| ---: | ---: | --- |
| 3,750 | 0 | -- |
| 4,000 | 30 | `116,115`--`145,115` |
| 4,250 | 24 | `318,116`--`341,116` |
| 4,750 | 30 | `378,110`--`407,110` |
| 5,000 | 40 | `288,110`--`373,116` |
| 5,750 | 72 | `130,111`--`351,126` |
| 6,250 | 160 | `270,113`--`357,122` |
| 6,500 | 246 | `248,115`--`385,131` |
| 6,750 | 790 | `144,120`--`397,156` |
| 7,000 | 228 | `384,138`--`483,151` |
| 7,250 | 0 | -- |

The measured red Golden Gate candidate grows substantially, but its horizontal
centre moves from 130.5 at frame 4,000 to 329 at frame 4,250, 240.5 at frame
5,750, 313.5 at frame 6,250, 270.5 at frame 6,750, and 433.5 at frame 7,000.
That is too much bearing/camera-relative motion to isolate physical distance.
This run therefore cannot prove or disprove a distance-only terrain or bridge
LOD scheme.

A twelve-frame trace at the medium visible checkpoint (frame 6,250) records
121,278 instructions and reaches `$C1F4AC` 10 times, alongside clipping,
projection, polygon submission, and line emission. Its transform sources are
`$C35B56` (twice), `$C35B80`, `$C363EC` (twice), `$C367B8`, `$C36954`,
`$C3A9A8`, `$C3ACD4`, and `$C3B588`.

The nearby larger checkpoint at frame 6,500 likewise reaches `$C1F4AC` 11
times. It shares `$C35B56`, `$C363EC`, `$C367B8`, `$C36954`, `$C3ACD4`, and
`$C3B588`, while using `$C35B1A` and `$C3A96E` where the frame-6,250 sample
uses `$C35B80` and `$C3A9A8`. This is a concrete source-family difference,
but it coincides with material landmark-bearing motion; it is therefore
compatible with camera-relative culling/detail and is not distance-only LOD
evidence.

## Result

Run038 is valid visual evidence that the red landmark appears from the sampled
frame 4,000 through 7,000. It is **not** the controlled same-bearing approach
required for an LOD conclusion. The static multi-tier terrain-origin threshold
policy remains a candidate mechanism only.

Authority: sealed `captures/run038`; ignored replay artifacts
`build/run038_bridge_keyframes/`, `build/run038_bridge_red.json`, and
`build/run038_frame{06250,06500}_12f_trace/`.
