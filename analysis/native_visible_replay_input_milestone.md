# Visible recorded-input comparisons — 2026-10-09

The recorded-input-only visible run now completes qualification and mission
three, lands, earns the intended save and returns to the menu without resets.
Every one of its **42,706 frames** is presented with real audio. Complete final
RAM, every existing runtime counter and the actual 78-byte save match the
independent headless recording. There are 1,857 qualification scene updates
and 3,770 mission-three scene updates, both in cockpit view zero.

Performance qualification remains open: **two frames exceed 20 ms**, at
86.0423 and 719.6773 ms. Their SDL polling portions are 84.7104 and 718.7189 ms.
All rows remain included. Whole-run work mean/p95/p99 is 1.1217/1.6318/2.1825 ms.
The native flight itself succeeds; the strict performance checker correctly
rejects the budget and retains its full report, CSV, log and compressed final
RAM. Other missions, camera views and visible combat are not covered here.

A user-closed earlier attempt ended
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

Current Release and Debug reproduce the independently started full flight: all 27,110
original and 16,062 native trace observations, complete final RAM, counters and
earned save are preserved. Their complete traces, RAM, counters and flight
comparison agree. The current canonical Release is the same executable used
by the finished visible run, SHA-256
`2f20d3a5caf1a0c8ddce85590ca408c96e4e35faf49e8b7e24cc8fb2e3a0411a`.

Two live window observations establish visibility without minimization. Its
rectangle changed from (792,305)-(1768,1112) to (1438,319)-(2414,1126) during the
measurement. Windows modal move/resize handling is a possible explanation for
the polling stalls: the linked SDL's WIN_PumpEvents dispatches native window
messages, including modal move/resize handling. Neither observation identifies
the message active during each slow frame. No causal attribution or budget
exception is established from these files.

```powershell
python tools/native/measure_visible_performance.py --mission-report build/native-flight/mission-three-recorded-input-reference-final/Release/report.json --mission-input build/native-flight/patrol-entry-review/update-consumed.fa18in --runner build/native/fa18_native.exe --out build/native-flight/visible-independent-mission-three-recorded-only
```

Failure summaries and hashes are retained in the committed checkpoint. Original
whole flights for the other missions, later cockpit drawing, recorded audio/
filter fidelity and broader visible performance remain open. Internal state
cleanup remains outside this goal.
