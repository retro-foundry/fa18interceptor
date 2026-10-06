# Native outer record update

The playable runner reaches C1C63E -> C22C80 -> C25B66. C22C80 now schedules
`advance_control_records`, retaining the slot, named child, child result and
phase in C. The existing source group rules remain: 1..3 share a primary gate,
5 uses the secondary gate, 9/11/13 pair with the preceding slot, slot 7 only
prepares/tests, and standalone records retain their companion. The source's
conditional ready/dispatch results and forced flags on 14/15 are preserved.

The parent composes the retained record-dynamics continuation directly, rather
than dispatching its CPU entry. This deletes 226 source instruction cases;
C1C63E's remaining 112 instruction bodies are byte-identical. The thin CPU
entry adapters and original boundaries for other children remain. Event timing
is deliberately an open gate; no average parent cycle fee was invented.

Latest connected batch: C22C80 now runs a retained game `ControlRecordsFrame`
and calls the retained record-dynamics C owner directly. All 226 outer CPU
instruction cases are deleted; the sibling C1C63E's 112 cases are unchanged.
Together with the previous batch, **780/780 cases of these two targeted CPU
bodies (100%) are removed**. Thin entry adapters, guest memory and other
children's runtime boundaries remain, so this is not whole-plan completion.
4,096 original-child core cases match all state. 4,096 production continuation
cases match registers/PC/SR and RAM outside old CPU stack scratch C7FD00..C7FEFF;
strict all-RAM fails case 0 at C7FED5. IRQ/event timing is held in those proofs.
GNU/MSVC both runners, twelve CTests, profiling and the MSVC/GNU demo pass.
Three 800-frame recording probes reach 129/30/150 direct C22C80 -> C25B66 calls
and zero C25B66 CPU-entry dispatch. Non-fade comparisons versus the previous
batch fail at 346/446/263; final RAM differs. Timing/parity remain open. No full
replay repeated. Bounded demo raw **69.8515%**, delta **-0.0191 pp**;
full **38.4011%** cached, accepted share unavailable, axes **0%**, gate **0/4**.
Raw counters exclude CPU-style port steps and cannot represent whole-plan
completion. Counts remain 25/590/84/691. Evidence:
`analysis/emulation_removal_record_loop_batch.json/.md`.
Next: C1C63E parent ownership and native root/control/render children; preserve
explicit timing, non-fade, stack and matrix acceptance debt.

The core comparison runs recreated C with original children and compares all
registers, PC, full SR and the entire RAM image for 4,096 source-input fixtures.
The separate production continuation activates only C22C80; it includes the
new native record-dynamics subtree. Its 4,096 held-event fixtures pass the same
register/return comparison and RAM outside the explicit old call-stack range.
The strict result is retained as a failed proof, not overwritten by the weaker
one: case 0 has a stale CPU-stack byte difference at C7FED5. The original return
word C7FF00 remains compared. These are valid recorded record layouts, not the
random full-width matrices from the preceding isolated failed fixture.

Three bounded recording probes complete. Compared with 843caf62, the added
non-fade differences total 358/9,586/68,759 pixels for demo/carrier/crash and
start at 346/446/263. All final RAM images differ. Selected palette indices
are included in the comparator even if RGB happens to match. Copper fade is
excluded under the user's policy. Those differences remain acceptance debt.

Reproduce core/production probes after building `record_update_stage_oracle.c`:

```powershell
build/recomp/record_update_stage_oracle.exe 4096 C22C80
build/recomp/record_update_stage_oracle.exe 4096 C22C80 --live --game-ram
```

Omit `--game-ram` to reproduce the strict failed stack-residue comparison.
Logs are in `build/flight-owner/outer-contract.log`, `outer-live.log`,
`outer-live-game-ram.log`, and `build/record-loop/recording-results.json`.
