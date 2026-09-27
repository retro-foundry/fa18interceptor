# `$C0FAA4`: run075 scene initialization

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

`fa18_initialize_scene_state` in `port/scene_initialization.c` represents the
full observed direct-store sequence with named scene fields and required,
ordered helper callbacks. Its caller owns the signed countdown because the
initializer writes the same traced `$C45AD6` word that `$C0FA04` immediately
replaces. The component does not reproduce the original storage map or encode
the helper calls as fake memory writes.

`fa18_finish_post_input_followup` plus the scene-initializer adapter represent
the observed composition:

1. apply the `$C0FAA4` direct subset while the prior mode is `$7F` and delay
   is negative;
2. apply `$C0FA04`'s direct followup writes, including delay two and the typed
   `FA18_MENU_CALLBACK_DEMO_FOLLOWUP_MATCH` continuation.

`scene_entry_contract_test` exercises the shared countdown and ordering: the
initializer sees negative `$C45AD6`, stores one on return, and `$C0FA04` then
stores two while installing its next callback. The called helpers, their wider
state, rendering initiated after this transition, and any uses outside run075
remain open.

The `$C11312` third helper is now the native `message_sequence` contract, and
the `$C082B0` fourth helper is the native `scene_finalization` direct-store
contract, in that composition. `$C28722` and `$C0924A` remain required caller
boundaries because their broader state ownership is not yet reconstructed.

Within `$C0924A`, its consecutive `$C09620` and `$C095C0` nested root-setup
helpers are now native `scene_root_setup`; the table/placement remainder of
the caller remains open.

Meaning level: direct writes **port-contract**; nested helpers **dataflow**.
