# Ghidra function-entry coverage

Generated from every `pcode/raw/*/functions.json` export and the `org` address
of every `source_amiga/observed/*.asm` source slice on 2026-09-21.

| Measure | Count | Meaning |
|---|---:|---|
| P-code exports containing a function inventory | 109 | Independent capture packets; entries recur across packets. |
| Distinct Ghidra function entries | 197 | Union by entry address. All retain Ghidra's `structural` classification. |
| Entries with an exact source-slice entry address | 51 | A readable, byte-exact source slice starts at the same address. This does not imply its entire Ghidra function body is reconstructed. |
| Entries represented within any byte-exact source range | 56 | Includes five entries reached through an adjacent source slice: `$C1C214`, `$C1D974`, `$C25A6A`, `$C26428`, and `$C2B3C2`. |
| Entries with no byte-exact source representation | 141 | Require source reconstruction or a range extension. |
| Entries with a bounded behavioural routine contract | 31 | Evidence-backed project classifications in `analysis/semantics.json`; this is not a claim that all 30 are among the 44 exact-entry matches. |
| Entries without an exact source-slice entry | 146 | Either require an entry reconstruction or are represented only by an adjacent source slice. |

The coverage measure intentionally counts **function entries**, not source files:
several source files are adjacent pieces of one function, and several current
static callback slices were not discovered in a P-code packet. A meaningful
function identification requires a bounded trace or equivalent direct evidence;
a descriptive source filename alone does not upgrade an entry from structural
status.















