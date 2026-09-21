; Byte-exact observed selector prefix $C12950-$C129AD.
; Bounded in the training frame-600 path, which takes the final early return.

                org     $C12950

CONTROL_RECORD_BASE             equ $C46184
CONTROL_RECORD_INDEX             equ $C458DC
CONTROL_RECORD_STRIDE_SHIFT      equ 9
CURRENT_CONTROL_RECORD           equ $C18210
INPUT_SNAPSHOT_A                 equ $C458B0
INPUT_SNAPSHOT_B                 equ $C458AE
MAP_TRANSITION_ACTIVE            equ $C457AD
MAP_TRANSITION_PENDING           equ $C457AE
UPDATE_SKIP_FLAG                 equ $C45795
CONTROL_RECORD_EARLY_RETURN      equ $C1316E

select_training_control_record:
                link.w  a6,#-30
                movem.l d2,-(a7)
                move.w  CONTROL_RECORD_INDEX.l,d0
                moveq   #CONTROL_RECORD_STRIDE_SHIFT,d1
                ext.l   d0
                asl.l   d1,d0
                movea.l d0,a0
                adda.l  #CONTROL_RECORD_BASE,a0
                move.l  a0,d0
                move.l  d0,CURRENT_CONTROL_RECORD.l
                move.b  INPUT_SNAPSHOT_A.l,-$11(a6)
                movea.l d0,a0
                addq.l  #2,a0
                move.b  INPUT_SNAPSHOT_B.l,d0
                move.l  a0,-$10(a6)
                tst.b   d0
                bne.w   CONTROL_RECORD_EARLY_RETURN
                move.b  MAP_TRANSITION_ACTIVE.l,d0
                move.b  MAP_TRANSITION_PENDING.l,d1
                or.b    d1,d0
                tst.b   d0
                bne.w   CONTROL_RECORD_EARLY_RETURN
                tst.b   UPDATE_SKIP_FLAG.l
                beq.w   CONTROL_RECORD_EARLY_RETURN
