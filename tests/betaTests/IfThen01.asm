LDA       R15,VARS
LDA       R14,STRS
LDA       R13,BOOL
WRST       +0(R14)
WRNL      
RDI       +0(R15)
RDI       +2(R15)
LD        R0,+0(R15)
IC        R0,+2(R15)
JGT       &8
LD        R0,#0
JMP       &4
LD        R0,#1
STO       R0,+4(R15)
LD        R0,+4(R15)
IC        R0,#0
JEQ       IFELSE0
WRI       +0(R15)
WRST       +22(R14)
WRI       +2(R15)
WRST       +40(R14)
WRNL      
LABEL     IFELSE0
JMP       IFEND0
LABEL     IFEND0
LD        R0,+2(R15)
IC        R0,+0(R15)
JGT       &8
LD        R0,#0
JMP       &4
LD        R0,#1
STO       R0,+6(R15)
LD        R0,+6(R15)
IC        R0,#0
JEQ       IFELSE1
WRI       +2(R15)
WRST       +42(R14)
WRI       +0(R15)
WRST       +60(R14)
WRNL      
LABEL     IFELSE1
JMP       IFEND1
LABEL     IFEND1
WRST       +62(R14)
WRNL      
HALT      
LABEL     VARS
INT       0
INT       0
INT       0
INT       0
LABEL     STRS
STRING    Enter two integers:: 
STRING     is greater than 
STRING    :012
STRING     is greater than 
STRING    :012
STRING    :012
LABEL     BOOL
STRING    "False"
STRING    "True"
