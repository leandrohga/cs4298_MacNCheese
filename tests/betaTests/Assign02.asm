LDA       R15,VARS
LDA       R14,STRS
LDA       R13,BOOL
LD        R0,#32
STO       R0,+0(R15)
WRST       +0(R14)
WRI       +0(R15)
WRNL      
LD        R0,+12(R15)
LD        R1,+14(R15)
FA        R0,+16(R15)
STO       R0,+20(R15)
STO       R1,+22(R15)
LD        R0,+20(R15)
LD        R1,+22(R15)
FM        R0,+24(R15)
STO       R0,+28(R15)
STO       R1,+30(R15)
LD        R0,+28(R15)
LD        R1,+30(R15)
STO       R0,+2(R15)
STO       R1,+4(R15)
WRST       +6(R14)
WRF       +2(R15)
WRNL      
LD        R0,+36(R15)
LD        R1,+38(R15)
FS        R0,+40(R15)
STO       R0,+44(R15)
STO       R1,+46(R15)
LD        R0,+32(R15)
LD        R1,+34(R15)
FM        R0,+44(R15)
STO       R0,+48(R15)
STO       R1,+50(R15)
LD        R0,+48(R15)
LD        R1,+50(R15)
STO       R0,+6(R15)
STO       R1,+8(R15)
WRST       +12(R14)
WRF       +6(R15)
WRNL      
LD        R0,#6
STO       R0,+10(R15)
WRST       +18(R14)
WRI       +10(R15)
WRNL      
HALT      
LABEL     VARS
INT       0
REAL      0.0
REAL      0.0
INT       0
REAL      2.3
REAL      7.1
REAL      2.3
REAL      5.0
REAL      2.3
REAL      5.0
REAL      3.6
REAL      2.9
REAL      3.6
REAL      5.0
LABEL     STRS
STRING    :012x ::=
STRING    :012y ::=
STRING    :012z ::=
STRING    :012n ::=
LABEL     BOOL
STRING    "False"
STRING    "True"
