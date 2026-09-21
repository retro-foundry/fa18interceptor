# Coarse deterministic attract-mode timeline

All points replay `captures/attract_run001/playback.e9k` from the same preserved
menu state. The only recording events select Demonstration Flight at frames 60
and 64. These are full-frame samples, not instruction traces.

| Frame | Approx. PAL time after capture epoch | Visual observation | Video SHA-256 |
| ---: | ---: | --- | --- |
| 600 | 12 s | Forward flight view and minimal HUD | `d124c2f8bde077230c8fb7ff455453326ada4d1d5fc26e4784041ec1d46e31e1` |
| 1800 | 36 s | Cockpit/forward view | `6999ce97db48b673994fe83560d23f5aeb2b75fb1ed5c122e0c94ecca5763061` |
| 3600 | 72 s | Cockpit/forward view, banked horizon | `62c893ba4f6ad86c39fda3b6b1bd3905962c4d1e8b6c87edb6040021e99077c2` |
| 5400 | 108 s | Cockpit/forward view, alternate bank/heading | `102814d99fc71e9b69ab4a557ad6dd393efe3ab2da5587e8c4a06b523b180f77` |
| 7200 | 144 s | Cockpit/forward view, alternate bank/heading | `b4f679ceeddae408d7ebf3aa2a1b1e41172ac7a047bea61cd0f84d537d3ca0b4` |
| 9000 | 180 s | Forward flight view | `07049a77bb3f40e99dc3f900af907bc05ab89cf7b396997b6f808894f3906107` |

This establishes multiple changing visual states under one deterministic input
recording. It does not prove where the demo loop begins or ends, nor establish
which renderer routines produce each component.
