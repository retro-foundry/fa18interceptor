; Byte-exact stream branch initialization $C1EFE6-$C1EFFF.

                org     $C1EFE6

STREAM_BRANCH_STATUS            equ $C4BF90
STREAM_RECORD_BASE              equ $C48390

initialize_c1ee14_stream_branch:
                move.w  #0,STREAM_BRANCH_STATUS.l
                lea     STREAM_RECORD_BASE.l,a3
                move.b  -100(a6),d6
                btst    #0,d6
                beq.w   $C1F464
