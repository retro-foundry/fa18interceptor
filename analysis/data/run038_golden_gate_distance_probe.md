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
121,278 instructions and a normal active renderer workload, including the
projection/clip helpers, but no `$C1F4AC` entry. It therefore does not connect
the red pixels to the existing Golden Gate transform-batch catalogue and must
not be used to identify a mesh or a replacement detail family.

## Result

Run038 is valid visual evidence that the red landmark appears from the sampled
frame 4,000 through 7,000. It is **not** the controlled same-bearing approach
required for an LOD conclusion. The static multi-tier terrain-origin threshold
policy remains a candidate mechanism only.

Authority: sealed `captures/run038`; ignored replay artifacts
`build/run038_bridge_keyframes/`, `build/run038_bridge_red.json`, and
`build/run038_frame06250_12f_trace/`.
