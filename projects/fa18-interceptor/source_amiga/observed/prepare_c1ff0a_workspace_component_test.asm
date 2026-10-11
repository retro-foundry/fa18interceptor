; Byte-exact workspace setup for component test $C1FF0A-$C1FF3D.
; The untaken $C1FF3E-$C1FF3F path remains outside this slice.

                org     $C1FF0A

TRIPLE_WORKSPACE               equ $C4BF94
OFFSET_VERTEX_TABLE             equ $C48390
COMPONENT_TEST_SELECTOR         equ $C1FB82

prepare_c1ff0a_workspace_component_test:
                lea     TRIPLE_WORKSPACE.l,a0
                lea     OFFSET_VERTEX_TABLE.l,a3
                movem.w (a2)+,d0-d2
                move.l  0(a3,d0.w),(a0)+
                move.w  4(a3,d0.w),(a0)+
                move.l  0(a3,d1.w),(a0)+
                move.w  4(a3,d1.w),(a0)+
                move.l  0(a3,d2.w),(a0)+
                move.w  4(a3,d2.w),(a0)+
                movea.w (a2)+,a3
                move.w  a3,d7
                jsr     COMPONENT_TEST_SELECTOR.l
                bne.b   $C1FF40
