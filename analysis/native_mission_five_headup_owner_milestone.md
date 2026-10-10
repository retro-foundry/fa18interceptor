# Mission Five head-up owner and carried inputs — 2026-10-10

The first independent drawing difference is now attributed to original
**C332BC**, the head-up owner. It produces all **395 differing scene bytes**
on the two games' actual radar-return inputs. The C owner matches original
instructions in every non-stack RAM byte on both inputs, and its scene output
matches each actual recorded message-entry scene byte. Other HUD calls between
the head-up return and message entry do not change those scene pixels.

This explains the first difference; the complete independent drawing gate
remains open until this owner is included throughout both full histories.
Native gameplay and the accepted 13,763-body result remain unchanged.

## Actual input ownership

The caller is `native_flight_tick -> native_hud_draw ->
draw_postflight_hud`, corresponding to the original `C0F18E -> C332BC` call.
Its inputs are retained original state, rather than newly seeded mission data.
The source earned escort prefix ends with selection marker `00E50001`, range
rate `FF90` (-112) and previous range `0FB1`. The native earned prefix ends with
marker `00000645`, rate `00FC` (252) and previous range `156F`.

All four marker/target/range fields exactly preserve each game's own actual
earned-prefix final RAM at the first Mission Five input boundary. The source
prefix report, complete final RAM and preceding successful execution are
verified against the original Mission Five evidence. The native prefix's
complete RAM, trace, pilot and current executable are verified by the existing
earned-continuation helper. The preceding independently controlled escort
flights have different cached values; this does not imply missing Mission Five
reset behavior or authorize adding a reset.

All **161 actual update identities** through the first difference are checked
using the executed original update mapping and verified continuation, rather
than an assumed ordinal offset. Source and native values are read at the
named globals' actual addresses. The retained native marker changes again
before the first drawing difference; both initial marker and range origins
remain bound to the actual earlier flight.

Five diagnostic-only input substitutions isolate the immediate cause.
Replacing only the marker accounts for **362 bytes**; replacing only the range
rate accounts for **33 bytes**. Replacing both accounts for every differing
scene byte. Replacing the target-coordinate remnant or previous range alone
does not change this output. Each substituted input also passes the complete
original/C owner comparison. These probes never alter gameplay or become
native flight initialization.

## Every-bit prediction

The external owner probe also executes the native and original owner with
zero pages, one pages and a spatially mixed diagnostic background. Every
non-stack RAM byte matches in all four executions for each actual input.
The zero/one results describe each bit's set, clear, preserve or invert
operation. Applying that operation to the actual background predicts all
**64,000 bytes per game**, including every scene and cockpit bit; the mixed
background separately checks the transfer. No rectangle, colour or pixel is
excluded from the prediction.

The reusable checker binds its inputs to the preceding sealed capture
checkpoint, verifies all **128,000 actual output bytes** and **81,920 recorded
caller scene bytes**, and retains both transfers compressed. Eight real
mutations reject lost set writes, lost clear writes, wrong page roles and an
omitted owner on both games' actual inputs. Executable, source and fixture
hashes remain unchanged during the check.

The initial component/probe executable and the later transfer executable have
separate recorded identities and remain compressed. Current tool hashes
describe the later transfer implementation. Original instructions and RAM
variants remain external diagnostics; no CPU, chipset or reference pixels
enter the playable runner. Native gameplay adds no allocation or random
behavior. Passing temporary raw RAM is removed and normal pruning runs;
protected compiler/reference data still exceeds the unchanged 4 GiB budget.

The [checkpoint](figures/native_mission_five_headup_owner_checkpoint.json)
binds actual component outputs, input probes, complete prefix carry,
executed update mapping, bit predictions and rejection controls.

The next complete-flight check must account for head-up writes and verify
shared scene generation before those writes, while preserving each game's
own earlier state. The existing full drawing rejection remains retained.
Broader independent flights and original sound matching remain open; campaign
continuity remains waived and broad named-state migration remains separate.

```powershell
python scripts/build_recomp.py --main tools/native/native_headup_owner_oracle.c --output build/native-flight/mission-five-headup-recheck/native_headup_owner_oracle.exe
python tools/native/check_headup_transfer.py --oracle build/native-flight/mission-five-headup-recheck/native_headup_owner_oracle.exe --evidence analysis/figures/native_mission_five_headup_history_checkpoint.json --source build/native-flight/mission-five-complete-cockpit-history/rejected-61956.source-C0F18E.dat.gz --native build/native-flight/mission-five-complete-cockpit-history/actual-61253.native-radar.after.dat.gz --source-post build/native-flight/mission-five-complete-cockpit-history/rejected-61956.source-C322EE.dat.gz --native-post build/native-flight/mission-five-complete-cockpit-history/actual-61253.native-message.before.dat.gz --out build/native-flight/mission-five-headup-recheck-prediction
```
