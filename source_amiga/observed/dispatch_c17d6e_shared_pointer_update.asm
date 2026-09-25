; Byte-exact observed forwarding wrapper $C17D6E-$C17D8F.
; Its flag-clear route at $C17D90 is outside this observed slice.

                org     $C17D6E

SHARED_UPDATE_FLAGS             equ     $C45B5B
SHARED_UPDATE_CHILD             equ     $C17DAA
SHARED_UPDATE_RETURN            equ     $C17DA6

dispatch_c17d6e_shared_pointer_update:
                link.w  a6,#0
                btst.b  #0,SHARED_UPDATE_FLAGS.l
                beq.b   $C17D90
                move.l  $10(a6),-(sp)
                move.l  $c(a6),-(sp)
                move.l  $8(a6),-(sp)
                bsr.b   SHARED_UPDATE_CHILD
                lea     $c(sp),sp
                bra.b   SHARED_UPDATE_RETURN
