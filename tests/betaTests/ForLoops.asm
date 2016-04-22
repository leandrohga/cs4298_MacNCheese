LDA       R15,VARS
LDA       R14,STRS
LDA       R13,BOOL
WRST       +0(R14)
WRNL      
RDI       +0(R15)
WRST       +52(R14)
WRNL      
WRST       +54(R14)
WRNL      
LD        R0,#1
STO       R0,+2(R15)
LABEL     FORTST0
LD        R0,+2(R15)
IC        R0,+0(R15)
JLE       &8
LD        R0,#0
JMP       &4
LD        R0,#1
STO       R0,+4(R15)
LD        R0,+4(R15)
IC        R0,#0
JNE       FORBDY0
JMP       FOREND0
LABEL     FORUPD0
LD        R0,+2(R15)
IA        R0,#1
STO       R0,+6(R15)
LD        R0,+6(R15)
STO       R0,+2(R15)
JMP       FORTST0
LABEL     FORBDY0
WRI       +2(R15)
WRNL      
JMP       FORUPD0
LABEL     FOREND0
WRST       +64(R14)
WRNL      
WRST       +66(R14)
WRNL      
LD        R0,+0(R15)
STO       R0,+2(R15)
LABEL     FORTST1
LD        R0,+2(R15)
IC        R0,#1
JGE       &8
LD        R0,#0
JMP       &4
LD        R0,#1
STO       R0,+8(R15)
LD        R0,+8(R15)
IC        R0,#0
JNE       FORBDY1
JMP       FOREND1
LABEL     FORUPD1
LD        R0,+2(R15)
IS        R0,#1
STO       R0,+10(R15)
LD        R0,+10(R15)
STO       R0,+2(R15)
JMP       FORTST1
LABEL     FORBDY1
WRI       +2(R15)
WRNL      
JMP       FORUPD1
LABEL     FOREND1
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
