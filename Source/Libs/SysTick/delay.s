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
            MOV     R1, #0x4            ; 0b00000110: Use Processor Clock, use COUNTFLAG for detecting 0
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
            PUSH    {R0-R2, LR}

            ; Delay is in ms (32 bit) is in R0
            LDR     R1, =Tick_Load
            AND     R0, #0x00FFFFFF     ; Mask out the upper 8 bits (reserved)
            STR     R0, [R1]            ; Write the value to the LOAD register

            LDR     R2, =Tick_Ctrl
            ORR     R1, #0x1            ; 0b00000001: Enable the counter
            STR     R1, [R2]            ; Writeback

            ; Wait for the COUNTFLAG to be set
Delay_Loop  LDR     R1, [R2]            ; Reading the control register
            TST     R1, #0x10000
            BEQ     Delay_Loop          ; If Z=1, & = 0, meaning COUNTFLAG is not set

            ; Disabling counter
            AND     R1, #0xFFFFFFFE     ; 0b11111110: Disable the counter
            STR     R1, [R2]            ; Writeback

            POP     {R0-R2, PC}
            ENDP

            END


