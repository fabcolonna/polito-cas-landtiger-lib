            ; CONSTANTS FOR SYSTICK REGISTERS

Tick_Ctrl   EQU     0xE000E010
Tick_Load   EQU     0xE000E014
Tick_Val    EQU     0xE000E018
Tick_Calib  EQU     0xE000E01C
            
            AREA    |.systicktext|, CODE, READONLY

Tick_Init   PROC
            EXPORT  Tick_Init
            PUSH    {R0, R1, LR}

            LDR     R0, =Tick_Ctrl
            MOV    R1, #0x4             ; 0b00000110: Use Processor Clock, use COUNTFLAG for detecting 0
            STR     R1, [R0]            ; Writeback

            LDR     R0, =Tick_Load
            MOV     R1, #0x00FFFFFF     ; Load 24 bits to 1
            STR     R1, [R0]            ; Writeback

            LDR     R0, =Tick_Val
            MOV     R1, #0x0            ; Clear the current value
            STR     R1, [R0]            ; Writeback

            POP     {R0, R1, PC}
            ENDP

Tick_Delay  PROC
            EXPORT  Tick_Delay
			IMPORT  __aeabi_uidiv
            PUSH    {R0-R2, R8, LR}

            ; Delay is in ms (32 bit) is in R0

            ; Since we don't know the clock frequency, we can't calculate the number of ticks
            ; but we know that SysTick is calibrated to timer intervals of 10ms for OS use.
            ; Wew can read the TENMS register in the CALIB (bits 23:0) to get the number of
            ; ticks for 10ms. We can then use them to calculate the # of ticks for the delay.
			MOV		R8, R0				; Moving R0 in R8
			
            LDR     R2, =Tick_Calib
            LDR     R0, [R2]
            AND     R0, #0x00FFFFFF     ; Mask out the upper 8 bits (reserved)
			MOV		R1, #10				; Divide by 10, arg in R1
			BL		__aeabi_uidiv		; Division, result in R0
			MUL		R0, R8				; Total number of ticks in R0

            LDR     R2, =Tick_Load
            AND     R0, #0x00FFFFFF     ; Mask out the upper 8 bits (reserved)
            STR     R0, [R2]            ; Write the value to the LOAD register

            LDR     R2, =Tick_Ctrl
            ORR     R1, #0x1            ; 0b00000001: Enable the counter
            STR     R1, [R2]            ; Writeback

            ; Wait for the COUNTFLAG to be set
Delay_Loop  TST     R2, #0x10000
            BEQ     Delay_Loop          ; If Z=1, & = 0, meaning COUNTFLAG is not set

            ; Disabling counter
            AND     R1, #0xFFFFFFFE     ; 0b11111110: Disable the counter
            STR     R1, [R2]            ; Writeback

            POP     {R0-R2, R8, PC}
            ENDP

            END


