; Byte-exact static-only segment-37 blitter sequence $C306D4-$C30751.

                org     $C306D4

CUSTOM_BASE                      equ $DFF000
RENDERER_POINTER_BLOCK_1         equ $C456BA
RENDERER_POINTER_TABLE_1         equ $C306C0
EXTERNAL_BLIT_HELPER             equ $C53F44
CUSTOM_BLTCON0                   equ $40
CUSTOM_BLTCON1                   equ $42
CUSTOM_BLTAFWM                   equ $44
CUSTOM_BLTALWM                   equ $46
CUSTOM_BLTAMOD                   equ $64
CUSTOM_BLTBMOD                   equ $66
CUSTOM_BLTAPTH                   equ $50
CUSTOM_BLTBPTH                   equ $54
CUSTOM_BLTSIZE                   equ $58
INITIAL_BLTCON0                  equ $09F0
INITIAL_BLTSIZE                  equ $0014
INITIAL_MODULO                   equ $00C8
INITIAL_BLTAPTH                  equ $0001
BSR_W_STORE_FIRST_OPCODE         equ $6100
BSR_W_STORE_FIRST_DISPLACEMENT   equ $001E
BSR_W_STORE_NEXT_OPCODE          equ $6100
BSR_W_STORE_NEXT_DISPLACEMENT_8  equ $000A
BSR_W_STORE_NEXT_DISPLACEMENT_4  equ $0006
BSR_W_STORE_NEXT_DISPLACEMENT_2  equ $0002

submit_segment37_blitter_sequence:
                lea.l   CUSTOM_BASE,a0
                movea.l RENDERER_POINTER_BLOCK_1.l,a2
                lea.l   RENDERER_POINTER_TABLE_1(pc),a1
                move.w  #INITIAL_BLTCON0,d2
                movea.w #INITIAL_BLTSIZE,a4
                move.w  #INITIAL_MODULO,d6
                asl.w   #6,d6
                addi.w  #$14,d6
                movea.w #INITIAL_BLTAPTH,a5
                move.l  (a2)+,d4
                movea.l (a1)+,a3
                move.l  (a3),d0
                jsr     EXTERNAL_BLIT_HELPER.l
                move.w  d2,CUSTOM_BLTCON0(a0)
                move.w  #0,CUSTOM_BLTCON1(a0)
                move.w  #$FFFF,CUSTOM_BLTAFWM(a0)
                move.w  #$FFFF,CUSTOM_BLTALWM(a0)
                move.w  a5,CUSTOM_BLTAMOD(a0)
                move.w  a5,CUSTOM_BLTBMOD(a0)
                ; Preserve the original BSR.W encodings.
                dc.w    BSR_W_STORE_FIRST_OPCODE,BSR_W_STORE_FIRST_DISPLACEMENT
                dc.w    BSR_W_STORE_NEXT_OPCODE,BSR_W_STORE_NEXT_DISPLACEMENT_8
                dc.w    BSR_W_STORE_NEXT_OPCODE,BSR_W_STORE_NEXT_DISPLACEMENT_4
                dc.w    BSR_W_STORE_NEXT_OPCODE,BSR_W_STORE_NEXT_DISPLACEMENT_2

.next:
                move.l  (a2)+,d4
                movea.l (a1)+,a3
                move.l  (a3),d0
                jsr     EXTERNAL_BLIT_HELPER.l
                move.w  d2,CUSTOM_BLTCON0(a0)
.store:
                move.l  d0,CUSTOM_BLTAPTH(a0)
                move.l  d4,CUSTOM_BLTBPTH(a0)
                move.w  d6,CUSTOM_BLTSIZE(a0)
                rts
