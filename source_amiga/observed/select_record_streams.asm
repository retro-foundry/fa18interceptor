; Byte-exact observed control-record slice $C1F7A0-$C1F837 (Hunk 10 +$E68).
; D0 is a negative control word supplied by the containing walker.  This slice
; establishes flags and resolves A1/A2 streams; ownership is not assigned.

                org     $C1F7A0

CONTROL_TERMINATOR             equ $FFFF
CONTROL_BIT_2000               equ $2000
CONTROL_BIT_1000               equ $1000
CONTROL_BIT_4000               equ $4000
CONTROL_BIT_8000               equ $8000
CONTROL_OFFSET_MASK            equ $0FFF
CONTROL_BASE_LOCAL             equ -$2C
CONTROL_A1_COUNT_LOCAL         equ -$66
CONTROL_A2_COUNT_LOCAL         equ -$6C
CONTROL_A1_FLAG_LOCAL          equ -$68
CONTROL_A2_FLAG_LOCAL          equ -$7A
RECORD_WALKER_DISPATCH         equ $C1F906
RECORD_WALKER_LOOP             equ $C1F716
RECORD_WALKER_RETURN_ZERO      equ $C1F79A
RECORD_WALKER_A1_RESUME        equ $C1F838
RECORD_WALKER_POST_STREAM      equ $C1F844

select_record_streams:
                cmpi.w  #CONTROL_TERMINATOR,d0
                beq.s   RECORD_WALKER_RETURN_ZERO
                clr.w   CONTROL_A2_COUNT_LOCAL(a6)
                clr.w   CONTROL_A2_FLAG_LOCAL(a6)
                move.w  d0,d1
                andi.w  #CONTROL_BIT_2000,d1
                beq.s   .select_a1_stream
                addq.w  #1,CONTROL_A2_COUNT_LOCAL(a6)
                move.w  d0,d1
                andi.w  #CONTROL_BIT_1000,d1
                beq.s   .a2_from_control_base
                movea.l (a5)+,a2
                bra.w   RECORD_WALKER_DISPATCH

.a2_from_control_base:
                movea.l CONTROL_BASE_LOCAL(a6),a3
                andi.w  #CONTROL_OFFSET_MASK,d0
                bra.s   .a2_selector_ready

.select_a1_stream:
                move.w  d0,d1
                andi.w  #CONTROL_BIT_4000,d1
                beq.s   .a1_stream_flag_ready
                addq.w  #1,CONTROL_A1_COUNT_LOCAL(a6)
.a1_stream_flag_ready:
                move.w  d0,d1
                andi.w  #CONTROL_BIT_1000,d1
                beq.s   .a1_from_control_base
                movea.l (a5)+,a1
                bra.s   .streams_selected

.a1_from_control_base:
                movea.l CONTROL_BASE_LOCAL(a6),a3
                andi.w  #CONTROL_OFFSET_MASK,d0
                lea     (a3,d0.w),a1
.streams_selected:
                clr.w   CONTROL_A1_FLAG_LOCAL(a6)

.select_a2_stream:
                tst.w   CONTROL_A2_FLAG_LOCAL(a6)
                bne.s   RECORD_WALKER_A1_RESUME
                tst.w   CONTROL_A2_COUNT_LOCAL(a6)
                bne.s   RECORD_WALKER_POST_STREAM
                tst.w   CONTROL_A1_FLAG_LOCAL(a6)
                bne.s   RECORD_WALKER_A1_RESUME
                move.w  (a1)+,d0
                move.w  d0,d1
                andi.w  #CONTROL_BIT_8000,d1
                beq.s   .a2_selector_ready
                addq.w  #1,CONTROL_A1_FLAG_LOCAL(a6)
.a2_selector_ready:
                move.w  d0,d1
                andi.w  #CONTROL_BIT_1000,d1
                beq.s   .a2_from_offset
                movea.l (a1)+,a2
                bra.w   RECORD_WALKER_DISPATCH

.a2_from_offset:
                andi.w  #CONTROL_OFFSET_MASK,d0
                movea.l CONTROL_BASE_LOCAL(a6),a3
                lea     (a3,d0.w),a2
                bra.w   RECORD_WALKER_DISPATCH

