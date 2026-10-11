; Byte-exact observed stack-argument gates $C131BE-$C131D1.

                org     $C131BE

C131DA                         equ     $C131DA
C13370                         equ     $C13370
C45797                         equ     $C45797

initialize_c131be_stack_argument_gate:
                link.w  a6,#-6
                tst.w   $a(a6)
                beq.w   C13370
                tst.b   C45797.l
                beq.b   C131DA
