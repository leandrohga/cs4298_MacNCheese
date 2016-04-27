LDA       R15,VARS
LDA       R14,BOOL
WRST      +112(R15)
WRNL      
RDI       +0(R15)
WRI       +0(R15)
WRST      +132(R15)
WRNL      
LD        R0,+0(R15)
STO       R0,+2(R15)
WRI       +2(R15)
WRST      +134(R15)
WRNL      
LD        R0,#17
STO       R0,+0(R15)
WRI       +0(R15)
WRST      +136(R15)
WRNL      
WRST      +140(R15)
WRNL      
RDF       +4(R15)
WRF       +4(R15)
WRST      +162(R15)
WRNL      
LD        R0,+4(R15)
LD        R1,+6(R15)
STO       R0,+8(R15)
STO       R1,+10(R15)
WRF       +8(R15)
WRST      +164(R15)
WRNL      
LD        R0,+166(R15)
LD        R1,+168(R15)
STO       R0,+4(R15)
STO       R1,+6(R15)
WRF       +4(R15)
WRST      +170(R15)
WRNL      
WRST      +174(R15)
WRNL      
RDST      +12(R15)
WRST      +12(R15)
WRST      +194(R15)
WRNL      
LDA       R0,+12(R15)
LD        R1,#50
BKT       R0,STR1
LD        R0,+110(R15)
SRZ       R0,8
SLZ       R0,8
STO       R0,+110(R15)
WRST      +62(R15)
WRST      +196(R15)
WRNL      
LDA       R0,+198(R15)
LD        R1,#2
BKT       R0,STR0
LD        R0,+12(R15)
SRZ       R0,8
SLZ       R0,8
STO       R0,+12(R15)
WRST      +12(R15)
WRST      +200(R15)
WRNL      
HALT      
LABEL     VARS
INT       0
INT       0
REAL      0.0
REAL      0.0
LABEL     STR0
SKIP      50
LABEL     STR1
SKIP      50
LABEL     STR2
STRING    "Enter an integer:: "
LABEL     STR3
STRING    ":012"
LABEL     STR4
STRING    ":012"
LABEL     STR5
STRING    ":012:012"
LABEL     STR6
STRING    "Enter a real number:: "
LABEL     STR7
STRING    ":012"
LABEL     STR8
STRING    ":012"
REAL      26.03
LABEL     STR9
STRING    ":012:012"
LABEL     STR10
STRING    "Enter a character:: "
LABEL     STR11
STRING    ":012"
LABEL     STR12
STRING    ":012"
LABEL     STR13
STRING    "$"
LABEL     STR14
STRING    ":012:012"
LABEL     BOOL
STRING    "False"
STRING    "True"
