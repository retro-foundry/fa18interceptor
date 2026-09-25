; Byte-exact observed stack-argument comparison gate $C148A2-$C148BD.

                org     $C148A2

C148D8                         equ     $C148D8
C148BE                         equ     $C148BE

gate_c148a2_stack_compare:
                link.w  a6,#-2
                movea.l 8(a6),a0
                move.w  (a0),d0
                ext.l   d0
                move.w  $e(a6),d1
                ext.l   d1
                cmp.l   d1,d0
                bgt.b   C148BE
                neg.l   d1
                cmp.l   d1,d0
                bge.b   C148D8
