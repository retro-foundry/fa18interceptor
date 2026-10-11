; Byte-exact observed projected-segment entry $C2EE4A-$C2EE67.

                org     $C2EE4A

PROJECTED_SEGMENT_INPUT         equ     $C4C592
PROJECTED_SEGMENT_OUTPUT        equ     $C4B390
CONTINUE_NEGATIVE_D3_CLIP       equ     $C2EE94

prepare_projected_segment_entry:
                link.w  a6,#-4
                lea.l   PROJECTED_SEGMENT_INPUT.l,a1
                lea.l   PROJECTED_SEGMENT_OUTPUT.l,a0
                move.w  #1,-2(a6)
                movem.w (a1),d3-d5
                cmp.w   d5,d3
                blt.b   CONTINUE_NEGATIVE_D3_CLIP
