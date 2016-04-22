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
LABEL     WHLBEG0
LD        R0,+2(R15)
IC        R0,+0(R15)
JLE       &8
LD        R0,#0
JMP       &4
LD        R0,#1
STO       R0,+4(R15)
LD        R0,+4(R15)
IC        R0,#0
JEQ       WHLEND0
WRI       +2(R15)
WRNL      
LD        R0,+2(R15)
IA        R0,#1
STO       R0,+6(R15)
LD        R0,+6(R15)
STO       R0,+2(R15)
JMP       WHLBEG0
LABEL     WHLEND0
WRST       +64(R14)
WRNL      
LD        R0,+0(R15)
STO       R0,+2(R15)
WRST       +66(R14)
WRNL      
LABEL     WHLBEG1
LD        R0,+2(R15)
IC        R0,#1
JGE       &8
LD        R0,#0
JMP       &4
LD        R0,#1
STO       R0,+8(R15)
LD        R0,+8(R15)
IC        R0,#0
JEQ       WHLEND1
WRI       +2(R15)
WRNL      
LD        R0,+2(R15)
IS        R0,#1
STO       R0,+10(R15)
LD        R0,+10(R15)
STO       R0,+2(R15)
JMP       WHLBEG1
LABEL     WHLEND1
WRST       +78(R14)
WRNL      
HALT      
LABEL     VARS
INT       0
INT       0
INT       0
INT       0
INT       0
INT       0
LABEL     STRS
STRING    Enter a positive integer upper limit for counting:: 
STRING    
STRING    Forward:: 
STRING    :012
STRING    Backward:: 
STRING    :012
LABEL     BOOL
STRING    "False"
STRING    "True"
