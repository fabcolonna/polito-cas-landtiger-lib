            ; CONSTANTS FOR SYSTICK REGISTERS

Tick_Ctrl   EQU     0xE000E010
Tick_LoadR  EQU     0xE000E014
Tick_Val    EQU     0xE000E018
Tick_Calib  EQU     0xE000E01C
            
            AREA    |.systicktext|, CODE, READONLY

Tick_Load   PROC
            EXPORT  Tick_Load
            LDR     R1, =Tick_LoadR
            AND     R0, #0xFFFFFF
            STR     R0, [R1]
            BX      LR
            ENDP

Tick_Count  PROC
            EXPORT  Tick_Count
            PUSH    {R0-R2, LR}

            LDR     R1, =Tick_Ctrl
            MOV     R2, #0x5
            STR     R2, [R1]            ; 0b101: Enable the counter, use the processor clock

Loop        LDR     R2, [R1]            ; Read the control register
            TST     R2, #0x10000        ; Check if COUNTFLAG is set
            BEQ     Loop                ; If EQ, i.e. Z=1, i.e. & = 0, i.e. not set, keep waiting

            MOV     R2, #0x0            ; Stop the counter
            STR     R2, [R1]            ; Writeback

            POP     {R0-R2, PC}
            ENDP

Tick_Stop   PROC
            EXPORT  Tick_Stop
            PUSH    {R0-R1, LR}

            LDR     R0, =Tick_Ctrl
            MOV     R1, #0x0            ; 0b00000000: Disable the counter
            STR     R1, [R0]            ; Writeback

            LDR     R0, =Tick_Val
            STR     R1, [R0]            ; Clear the current value

            LDR     R0, =Tick_LoadR
            STR     R1, [R0]            ; Clear the load value
     
            POP     {R0-R1, PC}
			ENDP
			
            END


