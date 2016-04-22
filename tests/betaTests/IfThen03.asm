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
JMP       IFEND0
LABEL     IFELSE0
LD        R0,+0(R15)
IC        R0,+2(R15)
JLT       &8
LD        R0,#0
JMP       &4
LD        R0,#1
STO       R0,+6(R15)
LD        R0,+6(R15)
IC        R0,#0
JEQ       IFELSE1
WRI       +0(R15)
WRST       +42(R14)
WRI       +2(R15)
WRST       +58(R14)
WRNL      
JMP       IFEND1
LABEL     IFELSE1
WRI       +0(R15)
WRST       +60(R14)
WRI       +2(R15)
WRST       +74(R14)
WRNL      
LABEL     IFEND1
LABEL     IFEND0
WRST       +76(R14)
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
STRING     is less than 
STRING    :012
STRING     is equal to 
STRING    :012
STRING    :012
LABEL     BOOL
STRING    "False"
STRING    "True"
