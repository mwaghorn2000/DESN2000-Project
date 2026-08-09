	AREA 	|.text|, CODE, READONLY
	EXPORT 	delay_us
	EXPORT 	timer1_init
; Cycles per us

PCONP	EQU		0xE01FC0C4
T1TCR   EQU     0xE0008004
T1TC    EQU     0xE0008008
T1PR    EQU     0xE000800C
T1MCR   EQU     0xE0008014

; Power the timer and set the prescaler
timer1_init
        LDR		R0, =PCONP			; power to our timer
        LDR		R1, [R0]
        ORR		R1, R1, #4
        STR		R1, [R0]

        LDR		R0, =T1PR
        MOV		R1, #35             ; PCLK 72MHz / 72 = 1 MHz so TC counts us
        STR		R1, [R0]

        LDR		R0, =T1MCR			; Reset match registers
        MOV		R1, #0
        STR		R1, [R0]
        MOV		PC, LR 				; return
		
; void delay_us(unsigned int us)
delay_us
		LDR		R1, =T1TCR
		MOV		R2, #2
		STR		R2, [R1] 			; reset timer
		MOV		R2, #1
		STR 	R2, [R1]			; start timer

		LDR		R3, =T1TC			; R3 = our count
		
delay_loop
		LDR		R2, [R3]			; compare T
		CMP		R2, R0
		BLT		delay_loop
		
		MOV		R2, #0				; turn off timer
		LDR		R1, =T1TCR
		STR		R2, [R1]
		MOV		PC, LR				; return 
		
		
		END
