# Scenario evidence matrix

This is an evidence inventory, not a game design summary.  A row is marked
only for what the cited sealed recording and its documented replay establish.
An empty or partial row is an acquisition target, not a claim that the feature
is absent.

| Scenario / state | Current authority | Established observation | Meaning level | Still unproven |
|---|---|---|---|---|
| Top-level menu | `captures/baseline_menu` and [run024](run024.md) | Run024 frame 600 visibly presents `QUALIFICATION REQUIRED FOR MISSIONS`; frame 20,000 is a mission-selection menu. | scenario | Menu-state ownership and persistent qualification reader. |
| Demonstration Flight | `captures/attract_run001` and [attract timeline](attract_timeline.md) | Recorded menu selection starts the demonstration; frames 600--9,000 show changing cockpit/forward-flight views without recorded flight controls. | scenario | Demo loop boundary, scripted-control producer, and return path. |
| Qualification menu selection | [run024 qualification selection chain](run024_qualification_selection_chain.md) | Frame-584 key `5` reaches `$C1BD78`, normalizes mode 9, and queues the qualification-gate line. | behavioral | Qualification status storage, eligibility predicate, and flight transition ownership. |
| Qualification success | sealed `captures/run060`; [run060 result](run060_qualification_success.md) | Native replay frame 9,545 visibly shows `LANDING SUCCESSFUL` and `YOU ARE NOW QUALIFIED FOR MISSIONS`. | scenario | Landing predicate, result/status writer, persistence, and mission-unlock consumer. |
| Qualification failure and result return | sealed `captures/run062`; [run062 result](run062_failed_qualification.md) | Native replay shows the crash result text at frame 2,300 and the top-level F/A-18 menu by frame 2,400; the final frame-2,470 checkpoint continues with no input to matching menu frame 2,475. The user additionally confirms the run returned from game to menu at its end. | scenario / port-contract boundary | Failure predicate, result payload producer, status writer, and causal difference from run060. |
| Other crash-result return | sealed `captures/run024`; [run024 result sequence](run024.md) | Frames 19,050--19,400 visibly progress from crash warning through result text to mission-selection menu. | scenario | Whether this was qualification context and its relationship to run062's result route. |
| Mission-selection key probes | sealed `captures/run043_mission_selection` and `captures/run044_mission_selection_f1`; [probe](run043_run044_mission_selection_probe.md) | Inputs `6` then F2/F1 did not enter distinct mission-specific observed instruction sets in their focused windows. | structural / scenario | Mission availability, selected mission, and mission launch route. |
| Free flight, map, and landmark traversal | sealed `captures/run024`; [run024](run024.md) | Flight cockpit, full-screen map, and the user-identified San Francisco/Golden Gate view are visibly anchored at frames 1,500, 8,000, and 23,000. | scenario | Control mapping, map-mode transition, landmark/model identity, and collision/terrain behavior. |
| Weapons, hostile AI, missile guidance, ejection/rescue | none promoted as a sealed scenario authority | No row is established by the cited capture evidence. | unknown | Capture and trace each state before assigning code ownership or gameplay semantics. |
| Individual mission completion / persistence | none promoted as a sealed scenario authority | No mission outcome or pilot-log mutation is established here. | unknown | Mission objectives, completion predicate, pilot-log fields, and unlock behavior. |

## Consequence for next captures

Run060 and run062 now bracket two qualification outcomes under the same
boot-restore protocol and canonical restored-state SHA-256.  They are the
strongest current comparison pair, but their differing result screens do not
identify the decision point.  The next qualification analysis must capture
native checkpoints around the earlier transition into each result state, then
compare state writers/readers and frame output.  It must not derive a failure
or success rule from the terminal text alone.

The mission, AI, guided-weapon, ejection/rescue, and persistence rows remain
explicit scenario gaps.  The existing demo and map/flight records improve
renderer and display coverage, but they do not substitute for those gameplay
oracles.
