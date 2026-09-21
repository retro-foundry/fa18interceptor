; Byte-exact alternate entry $C0D74A-$C0D751 into $C0D752 preparation code.
; The alternate matrix/data meaning remains unassigned.
                org $C0D74A
ALTERNATE_MATRIX_WORDS equ $C45BEA
select_alternate_display_matrix_base:
 lea ALTERNATE_MATRIX_WORDS.l,a2
 bra.b $C0D758
