.define 2as = 5
LOOP:    add r1, r4

        sub r1, #2as
        cmp H,  r1
        mov #6, LOOP[1]
        H:      prn #-2
S:      .data 1,5,#2as
        .data    5 #3, 1
END:    Stop