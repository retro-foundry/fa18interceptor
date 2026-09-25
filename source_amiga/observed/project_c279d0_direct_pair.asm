; Byte-exact observed direct-pair branch in $C279D0, $C27C62-$C27D0F.

                org     $C27C62

MATRIX_COEFFICIENTS             equ     $C45BD8
DIRECT_PAIR_MODE_LIMIT          equ     $C45984
DIRECT_PAIR_RENDERER_A          equ     $C2F5F4
DIRECT_PAIR_RENDERER_B          equ     $C2F60A

project_c279d0_direct_pair:
                move.w  a4,d3
                move.w  a5,d4
                lea     MATRIX_COEFFICIENTS.l,a0
                move.w  (a0)+,d0
                addq.w  #2,a0
                move.w  (a0)+,d2
                muls.w  d3,d0
                muls.w  d4,d2
                add.l   d2,d0
                asr.l   #8,d0
                add.w   d1,d0
                move.w  d0,d6
                move.w  (a0)+,d0
                addq.w  #2,a0
                move.w  (a0)+,d2
                muls.w  d3,d0
                muls.w  d4,d2
                add.l   d0,d2
                asr.l   #8,d2
                add.w   d5,d2
                muls.w  (a0)+,d3
                muls.w  $2(a0),d4
                add.l   d3,d4
                asr.l   #8,d4
                add.w   d7,d4
                ble.b   $C27C3C
                cmp.w   d4,d6
                bgt.b   $C27C3C
                move.w  d6,d0
                neg.w   d0
                cmp.w   d4,d0
                bgt.b   $C27C3C
                cmp.w   d4,d2
                bgt.b   $C27C3C
                move.w  d2,d0
                neg.w   d0
                cmp.w   d4,d0
                bgt.b   $C27C3C
                muls.w  #$a0,d6
                divs.w  d4,d6
                addi.w  #$a0,d6
                blt.b   $C27D10
                cmpi.w  #$140,d6
                bge.b   $C27D18
                muls.w  #$5a,d2
                divs.w  d4,d2
                addi.w  #$5a,d2
                blt.b   $C27D14
                cmpi.w  #$b4,d2
                bge.b   $C27D1E
                subi.w  #$13f,d6
                neg.w   d6
                subi.w  #$b3,d2
                neg.w   d2
                cmp.w   DIRECT_PAIR_MODE_LIMIT.l,d2
                bgt.w   $C27C3C
                move.w  d6,d0
                move.w  d2,d1
                move.l  a3,-(sp)
                cmpi.w  #2,-$18(a6)
                beq.b   .renderer_b
                jsr     DIRECT_PAIR_RENDERER_A.l
                bra.b   .renderer_done
.renderer_b:
                jsr     DIRECT_PAIR_RENDERER_B.l
.renderer_done:
                movea.l (sp)+,a3
                bra.w   $C27C3C
