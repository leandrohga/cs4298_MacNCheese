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
