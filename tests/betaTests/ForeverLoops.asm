LDA       R15,VARS
LDA       R14,STRS
LDA       R13,BOOL
LD        R0,#1
STO       R0,+2(R15)
LD        R0,#0
STO       R0,+4(R15)
WRST       +0(R14)
WRNL      
LABEL     WHLBEG0
