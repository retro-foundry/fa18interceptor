# Mission and menu text flow

This is the current high-level, evidence-backed map of the menu/mission text
system.  It intentionally separates user-visible scenario facts from static
text families and does not assign pilot persistence.

| Stage | Proven mechanism | Scenario evidence | Limit |
|---|---|---|---|
| Top-level menu batch | `$C0FBE0` initializes `$C4574A` with title, `SELECT:`, options 1–8, and Esc selector codes. | Flight-return snapshot contains the exact 11-code sequence; `$C32CEE/$C32D24` consumes code 109. | Writer itself is static in this trace window. |
| Top-level menu poll | `$C0FCB4` reads `$C458A6`; its static positive branches queue Demo, Free Flight, training, qualification, and advanced-mission codes. | Flight-return repeatedly observes the zero/no-selection path. | Other input mappings are not inferred. |
| Qualification selection | `$C1BD78` maps observed `D4=4` to `$C458A6=9`; `$C0FCB4` writes selector 105. | Sealed run024 key `5` at frame 584 and independent run029 key `5` at frame 266; both take mode 9. | This is one menu-context mapping, not a universal key contract. |
| Qualification gate | Selector 105 resolves to `$C3F2FF`, `5 ... QUALIFICATION: REQUIRED FOR MISSIONS`; static compositor consumes the selected cursor. | Run024 transition and frame-600 visible anchor. | Text is a gate prompt, not a persistent status byte. |
| Post-gate callbacks | Mode 9 routes through `$C0FECE -> $C10102 -> $C101FC -> $C10228 -> $C10678`. | Bounded run024 bodies at frames 627, 713, and 750. | Gameplay ownership of these state initializers remains unknown. |
| Embedded interstitial | `$C10678` queues selector 71, `CRACKED BY A-HA`; `$C1072E` advances to whitespace selector 4. | Run024 frames 750 and 790. | Crack/credit resource, explicitly not mission text. |
| Selectable-missions entry | Digit 6 reaches `$C1BD78`, maps `D4=5` to `$FF`; `$C0FCB4` installs `$C1017E`, which builds queue `[64,$806E,0]` in this state. `$C32FCE` consumes heading bytes. Controlled conditions 3--7 append `77..81`, visibly add F2--F6, and then reach their five `$C32D24` records in order; F1 is independently visible, not a queue member. Forced-state native F2 follows the zero-word `$C3318E` route rather than selecting a mission. | Controlled run029 key-6 trace, queue/display/record/F2 probes, and frame-400 submenu. | Legitimate F1--F6 selection, real availability ownership, and chosen-mission flow remain unproven. |
| Mission briefs/results | Six ordered static text families cover mission briefings and outcomes. | Snapshot inventory gives address/order/text. | Individual selection routes and outcome-state writers remain incomplete. |

## Evidence artifacts

- `analysis/data/mission_text_inventory.json`: printable text-run inventory.
- `analysis/data/run024_flight_return_message_sequence.json`: decoded live
  top-level selector batch.
- `analysis/data/run029_mission_menu_message_sequence.json`: retained decoder
  output from the frame-100 top-level-menu snapshot; it must not be used as a
  submenu-entry claim.
- `analysis/run024_qualification_selection_chain.md`: full recorded `5` to
  qualification-gate selector path.
- `analysis/run029_key6_selectable_missions_probe.md`: controlled digit-6
  transition from the top-level menu to the selectable-missions screen.
- `analysis/routines/c1017e_build_selectable_missions_queue.md`: bounded
  producer trace for the selector-64 / `$806E` queue pair.
- `analysis/routines/c15bf8_allocate_pointer_selected_condition_state.md`:
  static allocation/store chain for the `$C1AB74` conditional-state block.
- `analysis/run029_key6_all_conditionals_display_probe.md`: controlled
  F2--F6 queue-to-visible-label result.

The next high-value extension is a bounded selection and outcome trace for one
selectable mission, followed by a controlled pilot-log save experiment.  Until
those are captured, mission completion/qualification persistence is unknown.
