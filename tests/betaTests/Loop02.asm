LDA       R15,VARS
LDA       R14,STRS
LDA       R13,BOOL
WRST       +0(R14)
WRNL      
RDI       +0(R15)
WRST       +52(R14)
WRNL      
LD        R0,#1
STO       R0,+2(R15)
WRST       +54(R14)
WRNL      
LABEL     DOUTL0
WRI       +2(R15)
WRNL      
LD        R0,+2(R15)
IC        R0,+0(R15)
JEQ       &8
LD        R0,#0
JMP       &4
LD        R0,#1
STO       R0,+4(R15)
LD        R0,+4(R15)
IC        R0,#0
JEQ       IFELSE1
JMP       DOUTLEND0
LD        R0,+2(R15)
IA        R0,#1
STO       R0,+6(R15)
LD        R0,+6(R15)
STO       R0,+2(R15)
