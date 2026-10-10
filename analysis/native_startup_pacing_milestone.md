# Native startup presentation pacing

The playable runner now initializes `FA18FramePacer` after device callbacks,
renderer preparation and event-cache initialization, beside the gameplay clock
origin in `port/native/main.c`. The preceding build started presentation deadlines
before these startup operations. Its first visible frames could therefore arrive
close together while that accumulated debt cleared. The existing 50 Hz pacer,
game clocks, voice sequencing and PCM renderer retain their existing behavior.
No timing offset or additional gameplay delay is introduced.

Actual Direct3D/WASAPI windows at 44100 Hz present all 180 frames in both Release
and Debug. The first frame now includes its normal presentation wait: 16.1507 ms
in Release and 16.8561 ms in Debug. The next five observed start intervals range
from 18.6480 to 21.0726 ms and 18.8533 to 21.4684 ms respectively. These are host
measurements, not fitted clock values or new production tolerances. Maximum
frame work is 4.4811 ms in Release and 6.4020 ms in Debug. Each complete recorded
WAV and final RAM is byte-identical to the preceding qualified startup capture.
Every project gameplay heap violation, SDL pool request and arena failure is zero.
Driver-internal allocations are outside the instrumented project/SDL scope.

Both configurations pass all four affected CTests: allocation, complete startup
output preservation, frame timing and artifact cleanup. The current Release
runner also preserves the complete independently qualified Mission Five: all
13763 bodies, 41289 full-MiB snapshots, trace, timing, counters, final RAM, actual
earned pilot and menu return remain exact. Two fresh games use ordinary recorded
controls and the actual enlisted save; reference RAM is never supplied to gameplay.
The active `build/native/fa18_native.exe` has this qualified Release identity.

The [checkpoint](figures/native_startup_pacing_checkpoint.json) binds executable
witnesses, source identity, completed test logs, visible recordings and complete
Mission Five preservation. Evidence remains under `build/native-audio/startup-pacing`;
WAV, RAM and executable witnesses are compressed after exact decoded verification.

This completes the startup presentation follow-up in the
[output-rate evidence](native_audio_output_rate_milestone.md). Full original/native
sound onset and flight/combat waveforms remain unaccepted. Broader unrecorded
Free Flight road paths remain unverified. Named-state cleanup remains outside
the goal, and uninterrupted campaign completion remains waived.
