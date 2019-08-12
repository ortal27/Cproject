.define sz  =   2
K:    .data   22 
MAIN:     mov r3 , LIST[sz]
LOOP:     jmp  r1
          prn  #-5
          mov  STR[5] , STR[5]
          sub  r1 , r4    
          cmp  K , #sz
          bne  r4
L1:       inc  K
          bne   LOOP
END:     stop
.define len = 4
STR:   .string "abcdef"
LIST: .data   6 , -9 , len    
