K:    .data   22 
.define sz  =   2
MAIN:     mov  r3  LIST[sz]
LOOP:     jmp  L1
          prn  #-5
          mov  STR[5] , STR[2]
          sub  r1 , r4
STR:     .string "abcdef"       
          cmp  r3 , #sz
          bne  END
L1:       inc  K
          bne   LOOP
END:     stop
.define len = 4
LIST: .data   6 , -9 , len    
