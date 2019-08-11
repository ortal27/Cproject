.entry LIST
.extern W
.define LL =   2
MAIN:     cmp r3 , LIST[2]
LOOP:     jmp  W
          prn  r1 
          mov  STR[5] , r1
          sub  r1 , r4    
          cmp  K , #1
          bne  END
L1:       inc  L3
.entry LOOP
          bne   LOOP
END:     stop
.define len = 4
STR:   .string "abcdef"
LIST: .data   6 , -9 , len    
K:    .data   22 
.extern L3