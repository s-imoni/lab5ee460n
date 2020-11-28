.ORIG x3000

; test normal
LEA R0, normal
LDW R1, R0, #0 ; normal
STW R1, R0, #0 ; normal
LDB R1, R0, #0 ; normal
STB R1, R0, #0 ; normal
JMP R0 ; normal

; test page fault
; mark page with xc000 invalid... should cause a page fault during user prog.. 
; cant think of any other way .. can't mark user program in valid
; could write a VERY long program that goes into the next page. would need to be 256+ lines of code.

; test protection
.FILL xF022 ; should halt the machine but no exception during address translation

LEA R0, protection
LDW R1, R0, #0 ; causes exception
STW R1, R0, #0 ; causes exception
LDB R1, R0, #0 ; exception
STB R1, R0, #0 ; exception
JMP R0 ; causes exception

; test unaligned
LEA R0, unaligned
LDW R1, R0, #0 ; causes exception
STW R1, R0, #0 ; causes exception
LDB R1, R0, #0 ; no exception
STB R1, R0, #0 ; no exception
JMP R0 ; causes exception

; test unused
.FILL xA000 ; causes exception
.FILL xB102 ; causes exception

; test timer
; stop at cycle 299, mdump x1000 to x1200
; stop at cycle 300,
; stop at end of ISR

HALT

unaligned .FILL xC015
protection .FILL x2020
normal .FILL xC000

.END