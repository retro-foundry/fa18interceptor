# Actual message-owner pages - 2026-10-09

The later mission-three colour window, original observations 25,694..25,698,
now passes complete C322EE drawing checks on its actual inputs. Five source
and five native message owners match original instructions on every non-stack
RAM byte and defined text return. Every live original and native owner page
matches the standalone result. An independent predictor using the actual
immutable glyphs, layout, cached text and source plane-selection rules predicts
all **640,000 complete page bytes**. It includes glyph-cell clearing, and
checks unchanged pixels too.

The active call is `main -> native_frontend_tick -> native_flight_tick ->
native_hud_draw -> draw_message_line`. Native body-before/after exports come
from the current playable runner. Original frame-body execution on each actual
native input exports C322EE entry and its saved caller return; the whole body
still passes its existing explicit scratch/audio/busy-state scope. These
exports are reference-only instrumentation, with no runtime or allocation
changes. Component correctness and this five-body integration result are
separate from cross-runtime whole-flight page agreement.

The original reference captures stop at the actual C0F286 return saved by the
C0F280 call. All 25,699 preceding original observations remain unchanged.
Each saved stack return is checked against the capture end, and a capture
including the next caller instruction is rejected. All five live original
text-return bytes also match their independently derived values.

In this window the original cache draws `NO SIG` heading text; the native
cache retains its earlier `M1G-29` heading text and makes no draw request. The
original's first two draws use the kind reloaded by C11BFC. Its next information
refresh chooses the neutral kind, then the following redraws clear old cells
from the other colour planes. The native cache performs no page writes here.
The predictions reproduce both behaviours on all complete pages, with no
clock adjustment or image exclusions. Wrong destination plane, lost cell
clearing and wrong glyph pixels each fail the page prediction.

The current-build full-flight message assessment also passes all **4,964
real transitions**, 4,967 producer observations and nine negative controls.
It verifies every text cache, kind, redraw/delay gate, notification step and
elapsed-second page cycle. Initial ALT/HDG cycling preserves the same two
sampled-second events under the agreed timing policy. The current Release
executable is unchanged from the six-point scenery fix; Release/Debug full
flight traces, complete cores/page hashes, final RAM, earned saves and memory
reports remain preserved.

The subsequent [whole-plane history check](native_complete_plane_history_milestone.md)
now predicts all plane-1 XOR bytes in this window and the later colour window,
and accounts for every plane-1/2 difference across the complete flight. The
remaining wording below records this earlier owner-only scope.

Strict complete page agreement remains **287/4,967**. This result proves
message drawing on the bounded actual inputs; it does not yet carry a
cross-runtime pixel-history prediction through these five complete frame
entries. Their radar pixels and retained message differences remain in the
strict reports. Broader whole-flight drawing, original audio onset/handoffs
and visible performance remain open. The uninterrupted campaign is waived;
named-state cleanup stays outside this goal.

## Reproduction and evidence

```powershell
python tools/native/check_mission_message_pages.py --window build/native-flight/cockpit-message-colour-window --original-bodies build/native-flight/cockpit-message-colour-bodies --trace-evidence build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --out build/native-flight/cockpit-message-colour-pages
python tools/native/assess_mission_message_timing.py --traces build/native-flight/six-point-model-flight/Release --source-evidence build/native-flight/original-mission-three-patrol-runway --native-evidence build/native-flight/original-mission-three-ground-bounds-release --source-updates build/native-flight/patrol-entry-review --out build/native-flight/six-point-message-page-timing
```

The previous three-owner command interface still passes on the retained
original input. The [checkpoint](figures/native_message_owner_pages_checkpoint.json)
binds current runner/traces, original probe, actual owner RAM and page hashes,
layout, timing report, drawing mutations and tool sources. All retained RAM
is compressed; passing raw inputs and predictions are temporary.
