; Byte-exact placement-record loop $C1CB74-$C1CCB9.
; The 24-byte record and descriptor-field dataflow are scenario-backed.  The
; descriptor's ownership and the indirect target's game role remain unassigned.

                org     $C1CB74

PLACEMENT_LIST_VARIANT           equ     $C45865
PLACEMENT_LIST_OFFSET             equ     $C459AA
PLACEMENT_LIST_A                  equ     $C4E9AA
PLACEMENT_LIST_B                  equ     $C4F03A
PLACEMENT_RECORD_SENTINEL         equ     $FFFF
PLACEMENT_RECORD_SELECTOR_BYTE    equ     $C4585B
PLACEMENT_RECORD_SELECTOR_MASK    equ     $000F
PLACEMENT_SHIFT_COUNT             equ     $C45AB8
PLACEMENT_COORDINATE_WORDS        equ     $C45B2A
PLACEMENT_SCALED_COORDINATES      equ     $C45B30
PLACEMENT_DESCRIPTOR_TRAMPOLINE   equ     $C1ED48
PLACEMENT_DEPTH_GUARD             equ     $C45A66
PLACEMENT_DEPTH_GUARD_LIMIT       equ     $FFC80000
PLACEMENT_DESCRIPTOR_SPECIAL      equ     $C1ED3C
PLACEMENT_RECORD_VALUE            equ     $C45ABA
PLACEMENT_CONTROL_WORD            equ     $C458DA
PLACEMENT_CONTROL_MASK            equ     $0003
PLACEMENT_RECORD_COUNTER          equ     $C458BC
PLACEMENT_RECORD_BYTE             equ     $C458BD
PLACEMENT_RESULT_WORD             equ     $C45B40
PLACEMENT_COMPARISON_WORD         equ     $C459B2
PLACEMENT_DESCRIPTOR_STREAM       equ     $C45A36
PLACEMENT_DESCRIPTOR_AUX          equ     $C45A3A
PLACEMENT_DESCRIPTOR_KIND         equ     $C459B4
PLACEMENT_RECORD_STRIDE           equ     $18
FIXED_POINT_STAGE                 equ     $C1D91A

walk_flight_placement_records:
                tst.b   PLACEMENT_LIST_VARIANT.l
                bne.b   walk_flight_placement_records_select_b
                lea     PLACEMENT_LIST_A.l,a0
                bra.b   walk_flight_placement_records_base_ready
walk_flight_placement_records_select_b:
                lea     PLACEMENT_LIST_B.l,a0
walk_flight_placement_records_base_ready:
                move.w  PLACEMENT_LIST_OFFSET.l,d0
                adda.w  d0,a0
                move.w  (a0)+,d7
                cmpi.w  #PLACEMENT_RECORD_SENTINEL,d7
                beq.w   walk_flight_placement_records_end
                move.b  d7,PLACEMENT_RECORD_SELECTOR_BYTE.l
                move.b  d7,d0
                andi.w  #PLACEMENT_RECORD_SELECTOR_MASK,d0
                move.w  d0,PLACEMENT_SHIFT_COUNT.l
                movea.l (a0)+,a1
                tst.w   (a1)
                blt.w   walk_flight_placement_records_next
                cmpi.l  #PLACEMENT_DESCRIPTOR_TRAMPOLINE,(a1)
                bne.b   walk_flight_placement_records_descriptor_ready
                cmpi.l  #PLACEMENT_DEPTH_GUARD_LIMIT,PLACEMENT_DEPTH_GUARD.l
                blt.w   walk_flight_placement_records_next
walk_flight_placement_records_descriptor_ready:
                movem.w (a0)+,d2-d4
                movem.w d2-d4,PLACEMENT_COORDINATE_WORDS.l
                asl.l   #8,d2
                asl.l   #8,d3
                asl.l   #8,d4
                movem.l d2-d4,PLACEMENT_SCALED_COORDINATES.l
                cmpi.l  #PLACEMENT_DESCRIPTOR_SPECIAL,(a1)
                beq.b   walk_flight_placement_records_update_counters
                move.w  $04(a0),d1
                ext.l   d1
                asl.l   d0,d1
                cmpi.l  #$0400,d1
                bge.b   walk_flight_placement_records_store_record_value
                cmpi.l  #$0100,d1
                blt.b   walk_flight_placement_records_scale_value
                move.w  PLACEMENT_CONTROL_WORD.l,d0
                move.w  d7,d1
                btst    #8,d1
                bne.b   walk_flight_placement_records_control_bit_set
                andi.w  #PLACEMENT_CONTROL_MASK,d0
                beq.b   walk_flight_placement_records_scale_value
                bra.b   walk_flight_placement_records_store_record_value
walk_flight_placement_records_control_bit_set:
                andi.w  #PLACEMENT_CONTROL_MASK,d0
                cmpi.w  #2,d0
                bne.b   walk_flight_placement_records_store_record_value
walk_flight_placement_records_scale_value:
                movem.w PLACEMENT_COORDINATE_WORDS.l,d2-d4
                jsr     FIXED_POINT_STAGE.l
                move.w  d1,$04(a0)
walk_flight_placement_records_store_record_value:
                move.w  $08(a0),PLACEMENT_RECORD_VALUE.l
                bge.b   walk_flight_placement_records_update_counters
                subq.b  #1,$06(a0)
                bge.b   walk_flight_placement_records_next
                move.b  PLACEMENT_RECORD_COUNTER.l,$06(a0)
walk_flight_placement_records_update_counters:
                subq.b  #1,$07(a0)
                move.b  $07(a0),PLACEMENT_RECORD_BYTE.l
                move.w  $04(a0),d1
                move.w  d1,PLACEMENT_RESULT_WORD.l
                cmp.w   PLACEMENT_COMPARISON_WORD.l,d1
                movea.l (a1)+,a2
                movea.l (a1)+,a0
                move.l  (a1)+,PLACEMENT_DESCRIPTOR_STREAM.l
                move.l  (a1)+,PLACEMENT_DESCRIPTOR_AUX.l
                lsr.w   #8,d7
                ext.w   d7
                move.w  d7,PLACEMENT_DESCRIPTOR_KIND.l
                jsr     (a2)
                tst.b   PLACEMENT_LIST_VARIANT.l
                bne.b   walk_flight_placement_records_result_base_b
                lea     PLACEMENT_LIST_A.l,a0
                bra.b   walk_flight_placement_records_result_base_ready
walk_flight_placement_records_result_base_b:
                lea     PLACEMENT_LIST_B.l,a0
walk_flight_placement_records_result_base_ready:
                move.w  PLACEMENT_LIST_OFFSET.l,d1
                tst.w   d0
                bgt.b   walk_flight_placement_records_result_positive
                moveq   #-1,d0
walk_flight_placement_records_result_positive:
                move.w  d0,$14(a0,d1.w)
walk_flight_placement_records_next:
                addi.w  #PLACEMENT_RECORD_STRIDE,PLACEMENT_LIST_OFFSET.l
                bra.w   walk_flight_placement_records
walk_flight_placement_records_end:
                ; The sentinel target at C1CCBA is outside this slice.
