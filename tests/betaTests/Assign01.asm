LDA       R15,VARS
LDA       R14,STRS
LDA       R13,BOOL
WRST       +0(R14)
WRNL      
RDI       +0(R15)
WRI       +0(R15)
WRST       +20(R14)
WRNL      
LD        R0,+0(R15)
STO       R0,+2(R15)
WRI       +2(R15)
WRST       +22(R14)
WRNL      
LD        R0,#17
STO       R0,+0(R15)
WRI       +0(R15)
WRST       +24(R14)
WRNL      
WRST       +28(R14)
WRNL      
RDF       +4(R15)
WRF       +4(R15)
WRST       +50(R14)
WRNL      
LD        R0,+4(R15)
LD        R1,+6(R15)
STO       R0,+8(R15)
STO       R1,+10(R15)
WRF       +8(R15)
WRST       +52(R14)
WRNL      
LD        R0,+2060(R15)
LD        R1,+2062(R15)
STO       R0,+4(R15)
STO       R1,+6(R15)
WRF       +4(R15)
WRST       +54(R14)
WRNL      
WRST       +58(R14)
WRNL      
RDST      +12(R15)
WRST       +78(R14)
WRST       +98(R14)
WRNL      
STO       R0,"+12(R15)"
JMP        &8
STO       R0,"+12(R15)"
JMP        &8
STO       R0,"+12(R15)"
JMP        &8
STO       R0,"+12(R15)"
JMP        &8
STO       R0,"+12(R15)"
JMP        &8
STO       R0,"+12(R15)"
JMP        &8
STO       R0,"+12(R15)"
JMP        &8
LD        R0,+1036(R15)
STO       R0,"+12(R15)"
JMP        &8
STO       R0,"+12(R15)"
JMP        &8
WRST       +100(R14)
WRST       +102(R14)
WRNL      
LD        R0,+12(R15)
STO       R0,"$"
JMP        &1
STO       R0,"$"
JMP        &2
WRST       +104(R14)
WRST       +106(R14)
WRNL      
HALT      
LABEL     VARS
INT       0
INT       0
REAL      0.0
REAL      0.0
STRING    ""
STRING    ""
REAL      26.03
LABEL     STRS
STRING    Enter an integer:: 
STRING    :012
STRING    :012
STRING    :012:012
STRING    Enter a real number:: 
STRING    :012
STRING    :012
STRING    :012:012
STRING    Enter a character:: 
STRING    
STRING    :012
STRING    
STRING    :012
STRING    
STRING    :012:012
LABEL     BOOL
STRING    "False"
STRING    "True"
