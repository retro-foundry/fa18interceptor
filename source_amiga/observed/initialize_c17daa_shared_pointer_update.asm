; Byte-exact observed entry and guards $C17DAA-$C17DC5.

                org     $C17DAA

SHARED_UPDATE_FLAGS             equ     $C45B5B
SHARED_UPDATE_PRIMARY_POINTER   equ     $C4FE3C
SHARED_UPDATE_RETURN             equ     $C17E42

initialize_c17daa_shared_pointer_update:
                link.w  a6,#0
                movem.l d2,-(sp)
                btst.b  #1,SHARED_UPDATE_FLAGS.l
                beq.w   SHARED_UPDATE_RETURN
                tst.l   SHARED_UPDATE_PRIMARY_POINTER.l
                beq.b   SHARED_UPDATE_RETURN
