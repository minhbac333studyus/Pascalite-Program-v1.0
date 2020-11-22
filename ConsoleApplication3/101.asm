; Anh Minh Le, Minh Huynh      Sat Nov 21 20:58:45 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; program start

_start:                                 
        mov     eax,[I0]                ; load c in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        cmp     eax,[I0]                ; compare c and c
        jne     .L0                     ; if c <> c then jump to set eax to TRUE
        mov     eax,[FALSE]             ; else set eax to FALSE
        jmp     .L1                     ; unconditionally jump
.L0:                                    
        mov     eax,[TRUE]              ; set eax to TRUE
.L1:                                    
        mov     [B2],eax                ; k = AReg
        not     eax                     ; AReg = !AReg
        mov     [T0],eax                ; deassign AReg
        mov     eax,[B0]                ; AReg = a
        not     eax                     ; AReg = !AReg
        and     eax,[T0]                ; AReg = T1 and T0
        mov     [B2],eax                ; k = AReg
        Exit    {0}                     

SECTION .data                           
B0      dd      -1                      ; a
B1      dd      -1                      ; b
I0      dd      4301                    ; c
FALSE   dd      0                       ; false
TRUE    dd      -1                      ; true

SECTION .bss                            
T0      resd    1                       ; T0
I1      resd    1                       ; f
I2      resd    1                       ; g
I3      resd    1                       ; i
I4      resd    1                       ; j
B2      resd    1                       ; k

-------------------------------------------------------------------Table-----------------------------------------------------------------------
exName              | inName            | stype             | smode             | value             | salloc            | unit              
-------------------------------------------------------------------------------------------------------------------------------------------------
T0                  |T0                 |BOOLEAN            |VARIABLE           |1                  |YES                |1                  
T1                  |T1                 |BOOLEAN            |VARIABLE           |1                  |NO                 |1                  
a                   |B0                 |BOOLEAN            |CONSTANT           |-1                 |YES                |1                  
b                   |B1                 |BOOLEAN            |CONSTANT           |-1                 |YES                |1                  
c                   |I0                 |INTEGER            |CONSTANT           |4301               |YES                |1                  
f                   |I1                 |INTEGER            |VARIABLE           |1                  |YES                |1                  
false               |FALSE              |BOOLEAN            |CONSTANT           |0                  |YES                |1                  
g                   |I2                 |INTEGER            |VARIABLE           |1                  |YES                |1                  
i                   |I3                 |INTEGER            |VARIABLE           |1                  |YES                |1                  
j                   |I4                 |INTEGER            |VARIABLE           |1                  |YES                |1                  
k                   |B2                 |BOOLEAN            |VARIABLE           |1                  |YES                |1                  
start               |P0                 |PROG_NAME          |CONSTANT           |start              |NO                 |0                  
true                |TRUE               |BOOLEAN            |CONSTANT           |-1                 |YES                |1                  
