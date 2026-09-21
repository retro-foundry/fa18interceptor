; Byte-exact context-special vector calculation $C1B6C2-$C1B77B.

                org     $C1B6C2

COMMAND_REQUEST_FLAGS           equ $C4599C
COMMAND_REQUEST_BIT             equ 2
COMMAND_STATE_PENDING           equ $C457A6
COMMAND_STATE_PENDING_VALUE     equ $FF
COMMAND_STATE_A                 equ $C457B5
COMMAND_CONTEXT_FLAG            equ $C45785
COMMAND_RECORD_INDEX            equ $C45848
SIGNED_RECORD_TABLE_A           equ $C42A02
SIGNED_RECORD_TABLE_B           equ $C42A54
RECORD_DESCRIPTOR_BASE          equ $C46184
RECORD_DESCRIPTOR_TYPE_OFFSET   equ $62
RECORD_DESCRIPTOR_TYPE_SPECIAL  equ $20
VECTOR_ADJUSTMENT_TABLE         equ $C1D7E2
NEGATIVE_CASE_D3_SPECIAL        equ $FFDC
NEGATIVE_CASE_D4_SPECIAL        equ $002F
NEGATIVE_CASE_D5_SPECIAL        equ $FFD0
NEGATIVE_CASE_D3_DEFAULT        equ 0
NEGATIVE_CASE_D4_DEFAULT        equ $0014
NEGATIVE_CASE_D5_DEFAULT        equ $FF96

CALCULATE_NEGATIVE_VECTOR       equ $C091E0
SUBMIT_CONTEXT_VECTOR           equ $C0915A
DISPATCH_CONTEXT_SPECIAL_REQUEST equ $C1B68C
SHARED_COMMAND_QUEUE            equ $C1C23C

calculate_context_special_vectors:
                bset.b  #COMMAND_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                move.b  #COMMAND_STATE_PENDING_VALUE,COMMAND_STATE_PENDING.l
                lea.l   SIGNED_RECORD_TABLE_A.l,a0
                move.b  COMMAND_RECORD_INDEX.l,d0
                ext.w   d0
                asl.w   #4,d0
                move.w  d0,d1
                adda.w  d0,a0
                move.w  (a0),d0
                bge.b   .positive_record

                andi.w  #$7FFF,d0
                asl.w   #8,d0
                add.w   d0,d0
                lea.l   RECORD_DESCRIPTOR_BASE.l,a1
                adda.w  d0,a1
                cmpi.b  #RECORD_DESCRIPTOR_TYPE_SPECIAL,RECORD_DESCRIPTOR_TYPE_OFFSET(a1)
                bne.b   .negative_default
                move.w  #NEGATIVE_CASE_D3_SPECIAL,d3
                move.w  #NEGATIVE_CASE_D4_SPECIAL,d4
                move.w  #NEGATIVE_CASE_D5_SPECIAL,d5
                bra.b   .calculate_negative
.negative_default:
                moveq   #NEGATIVE_CASE_D3_DEFAULT,d3
                move.w  #NEGATIVE_CASE_D4_DEFAULT,d4
                move.w  #NEGATIVE_CASE_D5_DEFAULT,d5
.calculate_negative:
                jsr     CALCULATE_NEGATIVE_VECTOR.l
                bra.b   .submit_vector

.positive_record:
                lea.l   SIGNED_RECORD_TABLE_B.l,a0
                adda.w  d1,a0
                movem.w (a0),d0-d5
                swap    d0
                asl.l   #6,d0
                swap    d1
                asl.l   #6,d1
                add.w   d2,d2
                add.w   d2,d2
                lea.l   VECTOR_ADJUSTMENT_TABLE.l,a4
                movem.w (a4,d2.w),d6-d7
                asl.l   #8,d6
                asl.l   #8,d7
                add.l   d6,d0
                add.l   d7,d1
                asl.l   #8,d4
                asl.l   #8,d5
                add.l   d4,d0
                add.l   d5,d1
                asl.l   #8,d3
                move.l  d1,d2
                move.l  d3,d1
.submit_vector:
                jsr     SUBMIT_CONTEXT_VECTOR.l
                clr.b   COMMAND_STATE_A.l
                tst.b   COMMAND_CONTEXT_FLAG.l
                beq.w   DISPATCH_CONTEXT_SPECIAL_REQUEST
                clr.b   COMMAND_STATE_PENDING.l
                bra.w   SHARED_COMMAND_QUEUE
