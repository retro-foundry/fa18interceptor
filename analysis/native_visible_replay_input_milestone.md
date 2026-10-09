# Visible recorded-input comparisons — 2026-10-09

Visible performance qualification is still open. A user-closed attempt ended
at 22,648 of 42,706 expected frames, before mission three began. Its complete
CSV includes a 65.4517 ms quit/event-poll frame and is retained as a failure.

A second attempt presented all 42,706 frames but hit the frame limit before
the recorded controls finished: 26,502 of 27,108 replay updates were consumed.
It took one unexpected gameplay reset, did not earn the intended mission
result, and failed timing on two SDL event-poll frames. Work was 247.5437 ms
at frame 40,333 and 81.3995 ms at 41,068. Those frames remain in the report.
The earlier checker exited before retaining this attempt's final RAM or save;
the retained log and CSV are the evidence, and neither missing artifact is
claimed. The cause of the altered flight is not established by those files.

## Recorded controls and the real caller

The actual `port/native/main.c` SDL loop previously delivered physical mouse,
button and keyboard events alongside both recording formats. It still does so
in ordinary play. Opt-in `--recorded-input-only` now gives recorded comparison
controls sole ownership of gameplay input. It requires `--input` or `--replay`.
The SDL loop still polls every frame, handles window close, processes backend
messages, measures the entire poll cost and presents every rendered frame.
This removes physical input as a comparison confounder; it does not prove it
caused the failed flight or resolve the observed poll-time spikes.

`native_host_event` in `port/native/host_input.c` owns the common SDL boundary.
The playable entry and connected SDL-key test use the same function. Actual
recorded events still enter the existing frontend/replay owners. There is no
clock, physics, mission, gear, Escape or music change.

The visible checker accepts an independently earned mission-three report as
well as its existing campaign path. The uninterrupted campaign is waived, so
it is not a prerequisite for this comparison. A fresh pilot enlists normally,
then receives the full qualification/mission recording. Acceptance requires
the same executable/media/input hashes, every final RAM byte, all existing
counters, the actual earned save, real audio, every expected SDL presentation
and all frame work within 20 ms. Other mission/view coverage remains separate.

The checker now writes complete failure reports and compressed final RAM for
normal nonzero exits that export it, including a frame-limit failure. It does
not discard timing spikes, warm-up frames or short runs. The launch text
explicitly says that menus are intermediate steps and the window closes itself
after the recorded mission finishes.

## Validation and scope

Release and Debug pass five suppressed physical-control events without any
frontend state change, preserve normal mouse/button handling and accept window
close in both modes. Both retain the existing connected **26 SDL input entries
and 52 flight bodies**, compared with original instructions' RAM/display.
Their 6,500-frame timing checks preserve complete RAM, final pixels and counters
with the new option. Invalid option use is rejected; all eight dummy-window
frames are still presented. These dummy rows are integration evidence only.

Current Release reproduces the independently started full flight: all 27,110
original and 16,062 native trace observations, complete final RAM, counters and
earned save are preserved. A new paced visible run uses recorded-input-only;
its final acceptance must be checked separately. Current Debug is also being
checked against the full-flight recording. No passing result is inferred from
the earlier attempts or from component tests.

```powershell
python tools/native/measure_visible_performance.py --mission-report build/native-flight/mission-three-recorded-input-reference-final/Release/report.json --mission-input build/native-flight/patrol-entry-review/update-consumed.fa18in --runner build/native/fa18_native.exe --out build/native-flight/visible-independent-mission-three-recorded-only
```

Failure summaries and hashes are retained in the committed checkpoint. Original
whole flights for the other missions, later cockpit drawing, recorded audio/
filter fidelity and broader visible performance remain open. Internal state
cleanup remains outside this goal.
