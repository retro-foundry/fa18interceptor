# Static mission-selector record map

Authority: `captures/baseline_menu/slow.bin`, decoded reproducibly with:

```powershell
python scripts/inventory_message_records.py --output analysis/data/mission_selector_records_1_110.json
```

`$C32D24` indexes the signed-word table rooted at `$C3ED0A`, then treats the
resolved address as a four-byte record header followed by a NUL-terminated
payload.  The map below is a static table fact: it proves record placement and
the text each code resolves to.  It does not prove every code's producer,
reachability, mission progress, or outcome condition.

## Mission briefing/result groups

The contiguous selector-code ranges and record payloads establish these static
families:

| Codes | Record family | Text anchors |
|---|---|---|
| 23–30 | Visual Confirmation | `VISUAL CONFIRMATION OPERATION`, `SCRAMBLE IMMEDIATELY` |
| 31–38 | Emergency Defense | `EMERGENCY DEFENSE STATUS`, `SCRAMBLE IMMEDIATELY` |
| 39–45 | Intercept Stolen Aircraft | `ALERT STATUS`, `SCRAMBLE IMMEDIATELY` |
| 46–51 | Search and Rescue | `RESCUE OPERATION`, `EMERGENCY RESCUE POD` |
| 52–58 | Cruise Missile | `DEFCON`, `SCRAMBLE IMMEDIATELY FOR INTERCEPTION` |
| 59–63 | Carrier Sub | `SHADOW SUB DETECTION`, `SCRAMBLE AND INTERCEPT SHADOW SUB` |

The group boundaries are table order plus the directly decoded text; outcome
or branch semantics remain unassigned unless tied to a recorded scenario.

## Selectable-mission labels

Code 64 is the `SELECTABLE MISSIONS` heading.  Codes 77–81 resolve, in order,
to the visible F2–F6 labels at `$C3F46C`, `$C3F494`, `$C3F4BA`, `$C3F4E2`, and
`$C3F510`.  The adjacent code 76 resolves instead to `$C403C7`, a zero-length
payload record.  The F1 label's descriptor at `$C3F444` is present in the
snapshot, but no `$C3ED0A` table word for selectors 1–109 resolves to it.

Therefore the snapshot proves five selector-to-label links and the existence
of the F1 label, but does **not** prove a six-code F1–F6 selector sequence.
The controlled `$C1017E` conditional-byte probe reinforces the boundary:
the five queue words available before the relative-record table are codes
77--81 (F2--F6), while forcing its sixth tested conditional copies `$0424`,
the first relative record offset at `$C3ED0A`, rather than an F1 selector.
See `analysis/routines/c1017e_build_selectable_missions_queue.md`.
The controlled state also traces selectors 77--81 through `$C32D24` in that
order, resolving their five descriptors one by one; see
`analysis/run029_key6_all_conditionals_display_probe.md`. This confirms the
F2--F6 selector-to-record sequence without promoting the forced condition
bytes to a gameplay-state interpretation.
The controlled run029 digit-6 derivative establishes entry to the screen, but
not a chosen mission.  A separate native F1 probe is only a top-level-menu
probe; it enters `$C3318E` without writing `$C458A6` or directly queuing a
`$C4574A` selector.  See
`analysis/routines/c3318e_indexed_command_side_effect.md` and
`analysis/run029_key6_selectable_missions_probe.md`.

## Runtime-proven submenu instruction record

The controlled key-6 transition enters `$C32D24` at frame 328 with
`D0=$806E`.  Its negative-selector handling masks this to selector 110, reads
the signed word at `$C3EDE4`, and reaches record `$C3F725`.  The negative path
skips two header bytes and publishes text cursor `$C3F729`, whose payload is
`ESC - TO MAIN MENU`.  This matches the visible frame-400 submenu instruction.

No second `$C32D24` entry is reached from frame 329 through 500 with the same
no-future-input key-6 replay.  This proves one live selector/text-compositor
link in the submenu, not the producer of selector `$806E` or the F1 label's
selection route.
