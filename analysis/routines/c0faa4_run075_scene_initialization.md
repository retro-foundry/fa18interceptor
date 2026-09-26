# `$C0FAA4`: run075 demo scene-initialization subset

Classification: **scenario-backed direct state subset with unresolved helper
effects**.

## Evidence

The sealed run075 replay reaches `$C0FA04` at direct-core frame 370 with
`$C45AD6=$FFFF`. Its negative branch calls `$C0FAA4` at trace index 3 in
`build/port_run075_c0fa04_expiry_12k/trace.jsonl`. The initializer begins at
index 4 and returns at index 1128. `$C0FA04` then performs its own direct
followup writes at indices 1129 through 1138.

The direct stores in the initializer copy `$C458A7` to `$C458A8`, clear
`$C458A7`, set `$C45848` first to four and then to three, clear `$C458AE` and
`$C457AD`, set `$C45790`, `$C458A6`, and `$C45795` to one, three, and one,
clear `$C458AD`, `$C45986`, and `$C45988`, write `$FF` to `$C45858`, and set
the shared delay to one. It calls `$C28722`, `$C0924A`, `$C11312`, and
`$C082B0`; the bounded trace contains their effects, but does not assign their
complete behavioral ownership.

The caller immediately replaces the initializer's delay of one with two,
along with its command/followup state. Therefore the observable expired path
must preserve that order.

## Native contract

`fa18_initialize_run075_demo_scene` in `port/menu.c` represents the direct
subset using named `FA18MenuState` fields: scene latches, stage, guard,
native selected-mode value, transition auxiliary flag, counters, marker, and
delay. It does not reproduce the original storage map or encode the helper
calls as fake memory writes.

`fa18_expire_run075_demo_entry` represents the observed composition:

1. apply the `$C0FAA4` direct subset while the prior mode is `$7F` and delay
   is negative;
2. apply `$C0FA04`'s direct followup writes, including delay two and the typed
   `FA18_MENU_CALLBACK_DEMO_FOLLOWUP_MATCH` continuation.

`fa18_demo_contract_test` exercises this route after the preceding run075
entry wait. The called helpers, their wider state, rendering initiated after
this transition, and any uses outside run075 remain open.

Meaning level: direct writes **port-contract**; nested helpers **dataflow**.
