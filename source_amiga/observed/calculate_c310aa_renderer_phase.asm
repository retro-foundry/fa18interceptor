; Byte-exact observed renderer phase helper $C310AA-$C310E1.

                org     $C310AA

RENDERER_RECORD_BASE             equ     $C46184
RENDERER_RECORD_OFFSET           equ     $C458DE
RENDERER_PHASE_WORD              equ     $C459A0
RENDERER_PHASE_INDEX             equ     $C458C4

calculate_c310aa_renderer_phase:
                lea     RENDERER_RECORD_BASE.l,a0
                move.w  RENDERER_RECORD_OFFSET.l,d1
                move.w  $68(a0,d1.w),d0
                ext.l   d0
                asr.l   #3,d0
                divu.w  #$A,d0
                move.w  d0,RENDERER_PHASE_WORD.l
                asr.w   #1,d0
                mulu.w  #$87,d0
                asr.l   #8,d0
                ; Keep SUBI encoding: VASM otherwise substitutes SUBQ.
                dc.w    $0440,$0004
                bge.b   .phase_ready
                addi.w  #$60,d0
.phase_ready:
                move.w  d0,RENDERER_PHASE_INDEX.l
                rts
