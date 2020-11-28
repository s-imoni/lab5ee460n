.ORIG x1200 
; timer ISR

; saving registers on stack
STW R0 R6 #-1
STW R1 R6 #-2
STW R2 R6 #-3

LEA R0 pt 
LDW R0 R0 #0; load x1000 into R0
LEA R1 size
LDW R1 R1 #0; load 128 into R1


loop 	LDW R2 R0 #0 ; load
		AND R2 R2 #-2 ; clear with xFFFE
		STW R2 R0 #0 ; store
		ADD R0 R0 #2 ; increment
		ADD R1 R1 #-1; decrement counter
		BRp loop ; loop

; reloading registers
LDW R2 R6 #-3
LDW R1 R6 #-2
LDW R0 R6 #-1

RTI

pt 	  .FILL x1000 ; start of page table
size  .FILL #128
.END