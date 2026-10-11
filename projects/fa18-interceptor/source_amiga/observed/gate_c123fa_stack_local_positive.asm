; Byte-exact C123FA stack-local sign gate $C12704-$C12709.

                org     $C12704

gate_c123fa_stack_local_positive:
                tst.l   $1C(a6)
                bmi.b   $C1271C
