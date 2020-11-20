; Anh Minh Le, Minh Huynh      Fri Nov 20 12:17:47 2020
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .text                           
global  _start                          ; programstage1no140

_start:                                 
        mov     eax,[I0]                ; load a in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        mov     eax,[I1]                ; load b in eax
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        add     eax,[I0]                ; AReg = b + a
        mov     [I2],eax                ; sum = AReg
        call    WriteInt                ; write int in eax to standard out
        call    Crlf                    ; write \r\n to standard out
        Exit    {0}                     

SECTION .data                           
I0      dd      0                       ; a
I1      dd      3                       ; b

SECTION .bss                            
I2      resd    1                       ; sum
T0                  T0                  INTEGER             VARIABLE            1                   NO                  1                   
a                   I0                  INTEGER             CONSTANT            0                   YES                 1                   
b                   I1                  INTEGER             CONSTANT            3                   YES                 1                   
stage1no140         P0                  PROG_NAME           CONSTANT            stage1no140         NO                  0                   
sum                 I2                  INTEGER             VARIABLE            1                   YES                 1                   
