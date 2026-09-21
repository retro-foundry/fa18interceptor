# Mission/menu text inventory

Authority: `captures/baseline_menu/slow.bin`, reproduced by
`python scripts/inventory_mission_text.py`.  This is a text-location and
ordering inventory; it does not identify the code that selects a message or
any persistent mission-status byte.

For the corresponding high-level dynamic flow map, see
`analysis/mission_text_flow.md`.
For the exact static selector-code/record mapping, including its unresolved
F1-label gap, see `analysis/mission_selector_record_map.md`.

## Backing segment

- The contiguous printable pool lies in Slow RAM Hunk 64,
  `$C3ED00-$C41127` (9,256 bytes).  Hunk inventory calls it `CODE`, but it
  has zero relocations and the snapshot contains a static text resource here.
  Its runtime mapping is `mutated_or_unmapped`, so the baseline snapshot—not
  a byte identity claim for the extracted executable—is the authority.
- The main selection menu starts at `$C3F23A`; its six paths are listed at
  `$C3F26E-$C3F37D`.
- The selectable-mission menu begins at `$C3F401`; its ordered mission labels
  are `$C3F448`, `$C3F470`, `$C3F498`, `$C3F4BE`, `$C3F4E6`, and `$C3F514`.

## Observed text sequence

| Range | Text evidence | Conservative flow anchor |
|---|---|---|
| `$C3F26E-$C3F37D` | Demo, free flight, training demo/practice, qualification, selectable missions, next advanced mission, flight log | Top-level mode choices. |
| `$C3F302` / `$C3F41E` | `QUALIFICATION: REQUIRED FOR MISSIONS` / `REQUIRES QUALIFICATION` | Qualification gate text; not a status flag. |
| `$C3FAF2-$C3FC14` | Qualification instructions, successful carrier landing, qualification success, return/relaunch text | Qualification setup and success-message family. |
| `$C3FDC3-$C4000C` | Visual confirmation briefing and confirmation result | Mission 1 text family. |
| `$C4002D-$C403FC` | Air Force One emergency-defense briefing and success/failure variants | Mission 2 text family. |
| `$C4043F-$C40636` | Stolen F-16 briefing and recovery-range result | Mission 3 text family. |
| `$C40653-$C408E0` | Rescue briefing and rescue-pod near/far results | Mission 4 text family. |
| `$C4090A-$C40B8A` | Cruise-missile briefing and detonation/no-detonation results | Mission 5 text family. |
| `$C40BB1-$C40CD3` | Shadow-sub detection/approach/destroy briefing | Mission 6 text family. |

## Current code relationship

The steady qualification-screen trace records no direct P-code read of the
preloaded `QUALIFICATION` instances at `$C3F309`, `$C3F42E`, or `$C3FAFF`; see
`analysis/run024.md`.  The transition trace is stronger: at chipset frame 590
it consumes `$C3F31E`, byte `$45` (`E`) in the top-level line
`R5 ... QUALIFICATION: REQUIRED FOR MISSIONS`.

The measured text-to-compositor sequence is:

```text
$C330D8/$C330DA advance a line-layout pointer and the text cursor
  -> $C330DC saves A1/A2/A4 at $C456FE
  -> $C32F54 restores those cursors
  -> $C32FCE reads byte $45 from A2=$C3F31E
  -> $C33002 subtracts $20 and $C3305A indexes glyph offsets at $C3D8FC
```

The trace establishes this as a real static-text consumer during the
menu-to-qualification transition.  A separate flight-return trace now also
observes the adjacent selector choosing an instruction record; its input
producer remains unclassified.

## Static selector scaffold

The byte-exact snapshot disassembly at `$C32D24` provides the adjacent
selection mechanism:

```text
$C32D24  A1 := $C41066                 ; layout descriptor base
$C32D2A  A0 := $C3ED0A                 ; signed relative-word table
$C32D44-$C32D4C  D0 := table[D0 - 1]; A2 := A0 + D0
...
$C32E0E  publish A1/A2/A4 at $C4570A
$C32C16-$C32C1E  restore that triplet and copy it to $C456FE
```

For example, the table words at `$C3ED1A` and `$C3ED1C` resolve to
`$C3F7B6` and `$C3F936`, inside the two training-text record families.
Later entries `$C3EDD0-$C3EDE2` resolve into the main and selectable-mission
menu records, while `$C3ED36-$C3ED84` resolve into the mission 1--6 briefing
and outcome region.

The existing flight-return capture supplies one concrete dynamic mapping.
At frame 19,425, `$C32BD2` advances byte cursor `$C457C6` from `$12` to `$14`
because the next nonzero selector word at `$C4575E` is 109.  `$C32CEE` reads
that word from the `$C4574A` selector sequence; it then reads table word
`$069D` at `$C3EDE2`, selects descriptor `$C3F3A7`, and publishes a cursor at
`$C3F3AB` for `ESC ......... RESTARTS YOUR SELECTION`.  It establishes a
shared live message-sequence/record-selector family; it does not identify the
routine that populated the sequence or map it to a mission-status transition.

The same flight-return snapshot has a nonzero `$C4574A` sequence prefix of
codes `6, 100..109`.  Resolving it through the observed selector yields the
title, `SELECT:`, all eight top-level menu paths in their displayed order, and
the Esc restart instruction.  See
`analysis/run024_flight_return_text_selection.md` for the address/code/payload
table and its scenario limits.

`scripts/decode_message_sequence.py` provides the same relative-table and
descriptor decoding for any captured `slow.bin`; the checked flight-return
result is `analysis/data/run024_flight_return_message_sequence.json`.

In contrast, normal run029 frame 100 visibly remains on the top-level
`SELECT:` menu.  Its saved compositor cursor is `$C3F3FC`, immediately before
selector-64 descriptor `$C3F3FD` (`SELECTABLE MISSIONS`), but that cursor alone
does not prove selector 64 is the active visible record.  It is static
selector evidence only; see `analysis/run029.md`.

An independent run024 post-gate trace reaches selector 71 at frame 750.  It
resolves to `$C3EFB4`, `CRACKED BY A-HA`, an embedded crack/credit record rather
than a mission-text family.  See
`analysis/routines/c10678_post_gate_interstitial_selector.md`.
