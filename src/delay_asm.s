	AREA 	|.text|, CODE, READONLY
	EXPORT 	delay_us
	EXPORT 	timer0_init
; Cycles per us

PCONP	EQU		0xE01FC0C4
T0TCR	EQU		0xE0004004
T0TC	EQU		0xE0004008
T0PR	EQU		0xE000400C
T0MCR   EQU     0xE0004014

; Power the timer and set the prescaler
timer0_init
        LDR     R0, =PCONP			; power to our timer
        LDR     R1, [R0]
        ORR     R1, R1, #2
        STR     R1, [R0]

        LDR     R0, =T0PR
        MOV     R1, #71             ; PCLK 72MHz / 72 = 1 MHz so TC counts us
        STR     R1, [R0]

        LDR     R0, =T0MCR			; Reset match registers
        MOV     R1, #0
        STR     R1, [R0]
        MOV 	PC, LR 				; return
		
; void delay_us(unsigned int us)
delay_us
		LDR		R1, =T0TCR
		MOV 	R2, #2
		STR 	R2, [R1] 			; reset timer
		MOV		R2, #1
		STR 	R2, [R1]			; start timer
		
		LDR		R3, =T0TC			; R3 = our count
		
delay_loop
		LDR 	R2, [R3]			; compare T
		CMP 	R2, R0
		BLT		delay_loop
		
		MOV		R2, #0				; turn off timer
		LDR 	R1, =T0TCR
		STR		R2, [R1]
		MOV		PC, LR				; return 
		
		
		END
