; Byte-exact OCS JOY0DAT derived-bit update stage $C16F1C-$C16FF3.
; Derived-test directions and helper/gameplay meanings remain unassigned.
                org     $C16F1C

JOY0DAT                         equ $DFF00C
JOY0DAT_SNAPSHOT                equ $C45950
JOY0DAT_DERIVED_MASK_A          equ $0200
JOY0DAT_DERIVED_MASK_B          equ $0100
STORED_WORD_HIGH_BYTE_BIT        equ 1
STORED_WORD_LOW_BYTE_BIT         equ 1
JOY0DAT_TEST_ONE_ACTIVE_FLAG     equ $C45831
JOY0DAT_TEST_TWO_ACTIVE_FLAG     equ $C45832
HANDLE_TEST_ONE_A                equ $C1B510
HANDLE_TEST_ONE_B                equ $C1B50C
HANDLE_TEST_ONE_RELEASE          equ $C1B514
HANDLE_TEST_TWO_A                equ $C1B558
HANDLE_TEST_TWO_B                equ $C1B55C
HANDLE_TEST_TWO_RELEASE          equ $C1B560

update_joy0dat_bit_pair_state:
                link    a6,#-10
                move.l  #JOY0DAT,-4(a6)
                movea.l -4(a6),a0
                move.w  (a0),d0
                move.w  d0,JOY0DAT_SNAPSHOT.l
                move.w  d0,-6(a6)
                ext.l   d0
                dc.w    $2F40,0                 ; move.l d0,0(sp)
                andi.l  #JOY0DAT_DERIVED_MASK_A,d0
                asr.l   #1,d0
                dc.w    $222F,0                 ; move.l 0(sp),d1
                andi.l  #JOY0DAT_DERIVED_MASK_B,d1
                eor.l   d1,d0
                tst.l   d0
                beq.s   .check_test_one_b
                jsr     HANDLE_TEST_ONE_A.l
                move.b  #1,JOY0DAT_TEST_ONE_ACTIVE_FLAG.l
                bra.s   .check_test_two_a
.check_test_one_b:
                move.w  -6(a6),d0
                ext.l   d0
                dc.w    $2F40,0                 ; move.l d0,0(sp)
                andi.l  #2,d0
                asr.l   #1,d0
                dc.w    $222F,0                 ; move.l 0(sp),d1
                andi.l  #1,d1
                eor.l   d1,d0
                tst.l   d0
                beq.s   .check_test_one_release
                jsr     HANDLE_TEST_ONE_B.l
                move.b  #1,JOY0DAT_TEST_ONE_ACTIVE_FLAG.l
                bra.s   .check_test_two_a
.check_test_one_release:
                tst.b   JOY0DAT_TEST_ONE_ACTIVE_FLAG.l
                beq.s   .check_test_two_a
                jsr     HANDLE_TEST_ONE_RELEASE.l
                clr.b   JOY0DAT_TEST_ONE_ACTIVE_FLAG.l
.check_test_two_a:
                btst    #STORED_WORD_HIGH_BYTE_BIT,-6(a6)
                beq.s   .check_test_two_b
                jsr     HANDLE_TEST_TWO_A.l
                move.b  #1,JOY0DAT_TEST_TWO_ACTIVE_FLAG.l
                bra.s   .done
.check_test_two_b:
                btst    #STORED_WORD_LOW_BYTE_BIT,-5(a6)
                beq.s   .check_test_two_release
                jsr     HANDLE_TEST_TWO_B.l
                move.b  #1,JOY0DAT_TEST_TWO_ACTIVE_FLAG.l
                bra.s   .done
.check_test_two_release:
                tst.b   JOY0DAT_TEST_TWO_ACTIVE_FLAG.l
                beq.s   .done
                jsr     HANDLE_TEST_TWO_RELEASE.l
                clr.b   JOY0DAT_TEST_TWO_ACTIVE_FLAG.l
.done:
                unlk    a6
                rts
