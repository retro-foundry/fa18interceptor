# Record projection branch at `$C20A52`

Classification: **dataflow with renderer descendant**. This observed branch
shares the enclosing `$C1F6F8` record-walker frame rather than establishing a
new independent stack frame. Its exact setup `$C20A52-$C20A7F` is in
`source_amiga/observed/initialize_c20a52_record_projection.asm`.

The setup writes selector `$000D` to `$C45954`, initializes four renderer
state words at `$C456E6`, derives an address within `$C48390` from a signed
word consumed through `A2`, and retains two following words in enclosing-frame
locals. The observed continuation fills `$C4BF90`-based six-word records and
calls `$C246A0`, the known projection/clip stage. This establishes a
record-to-renderer-workspace path, but not a final primitive topology or object
identity.

The adjacent exact block `$C20A80-$C20AE3` is now in
`prepare_c20a52_projection_differences.asm`. It consumes two adjacent triples
from the selected `$C48390` workspace, forms six signed component differences,
and derives two three-word intermediates in the shared `A6` frame. This
establishes vector-difference preparation before the `$C4BF90` submission
loop; it does not by itself identify a polygon, normal, or object.

`build_c20a52_projection_pairs.asm` now reconstructs `$C20AE4-$C20B91`.
It builds two adjacent six-word records at `$C4BF94`, calls `$C246A0`, ORs the
returned status into the shared result word, and repeats according to local
`-$38(A6)`. This is a bounded workspace-to-projection loop; the record format
is structural and not yet promoted to a named primitive topology.

The exact `$C20C10-$C20C21` return block is in
`return_c20a52_record_projection.asm`. It repeats the alternate local
`-$3A(A6)` loop while positive; otherwise it restores `A1/A2/A5` and returns
the accumulated `-$7E(A6)` status in `D0`. This proves the branch's bounded
call/return contract.

Authority: `pcode/raw/run001_c1f6f8_record_walk_stage/observed.asm.txt`.
