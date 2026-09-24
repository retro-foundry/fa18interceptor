; Byte-exact descriptor dispatch phase $C1CDFC-$C1CE37.

                org     $C1CDFC

FIXED_POINT_STAGE                equ     $C1D91A
FOLLOWUP_DESCRIPTOR_TABLE        equ     $C22188
FOLLOWUP_DESCRIPTOR_KIND         equ     $C459B4
FOLLOWUP_CONTROL_STREAM          equ     $C45A36
FOLLOWUP_CONTROL_AUX             equ     $C45A3A
FOLLOWUP_RECORD_INDEX            equ     $C459AA
FOLLOWUP_LOOP_HEAD               equ     $C1CCE4

dispatch_flight_followup_descriptor:
                jsr     FIXED_POINT_STAGE.l
                lea     FOLLOWUP_DESCRIPTOR_TABLE.l,a1
                move.w  FOLLOWUP_DESCRIPTOR_KIND.l,d1
                add.w   d1,d1
                add.w   d1,d1
                move.w  d1,d7
                add.w   d1,d1
                add.w   d1,d1
                add.w   d7,d1
                adda.w  d1,a1
                movea.l (a1)+,a2
                movea.l (a1)+,a0
                move.l  (a1)+,FOLLOWUP_CONTROL_STREAM.l
                move.l  (a1)+,FOLLOWUP_CONTROL_AUX.l
                jsr     (a2)
                addq.w  #2,FOLLOWUP_RECORD_INDEX.l
                bra.w   FOLLOWUP_LOOP_HEAD
