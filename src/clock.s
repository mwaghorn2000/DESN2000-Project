	AREA 	|.text|, CODE, READONLY
	EXPORT	timer0_init
	EXPORT 	clock_service
	IMPORT 	sim_minutes
	IMPORT	last_tick
	
PCONP 	EQU		0xE01FC0C4
T0TCR 	EQU		0xE0004004
T0TC	EQU		0xE0004008
T0PR	EQU 	0xE000400C
T0MCR	EQU 	0xE0004014

ONE_SEC EQU		1000000
DAY_MIN EQU		1400

; void timer0_init(void)
timer0_init
        LDR		R0, =PCONP			; power to our timer
        LDR		R1, [R0]
        ORR		R1, R1, #2
        STR		R1, [R0]

        LDR		R0, =T0PR
        MOV		R1, #35          ; PCLK
        STR		R1, [R0]

        LDR		R0, =T0MCR			; Reset match registers
        MOV		R1, #0
        STR		R1, [R0]
		
		LDR     R0, =T0TCR
        MOV     R1, #2              ; reset counter
        STR     R1, [R0]
        MOV     R1, #1              ; enable, release reset
        STR     R1, [R0]
        MOV     PC, LR

		
; int clock_service(void)
; returns 1 if the clock should advance. 0 otherwise
clock_service
		LDR		R0, = T0TC
		LDR 	R0, [R0]
		LDR		R1, =last_tick
		LDR 	R2, [R1]
		SUB		R3, R0, R2			; Time elapsed
		LDR 	R0, =ONE_SEC
		CMP		R3, R0
		BLT		no_tick				; time elapsed < 1000000us
		
		ADD		R2, R2, R0			; last += ONE_SEC
		STR		R2, [R1]
		
		LDR		R1, =sim_minutes
		LDR		R2, [R1]
		ADD		R2, R2, #5
		LDR		R0, =DAY_MIN		; Wrap sim_minutes if its greater than 1400
		CMP		R2, R0
		MOVHS	R2, #0
		STR		R2, [R1]
		
		MOV		R0, #1
		MOV		PC, LR
		
no_tick
		MOV 	R0, #0
		MOV 	PC, LR
		
		END
