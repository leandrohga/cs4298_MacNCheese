LDA       R15,VARS
LDA       R14,BOOL
LD        R0,#32
STO       R0,+0(R15)
WRST      +12(R15)
WRI       +0(R15)
WRNL      
LD        R0,+18(R15)
LD        R1,+20(R15)
FA        R0,+22(R15)
STO       R0,+26(R15)
STO       R1,+28(R15)
LD        R0,+26(R15)
LD        R1,+28(R15)
FM        R0,+30(R15)
STO       R0,+34(R15)
STO       R1,+36(R15)
LD        R0,+34(R15)
LD        R1,+36(R15)
STO       R0,+2(R15)
STO       R1,+4(R15)
WRST      +38(R15)
WRF       +2(R15)
WRNL      
LD        R0,+48(R15)
LD        R1,+50(R15)
FS        R0,+52(R15)
STO       R0,+56(R15)
STO       R1,+58(R15)
LD        R0,+44(R15)
LD        R1,+46(R15)
FM        R0,+56(R15)
STO       R0,+60(R15)
STO       R1,+62(R15)
LD        R0,+60(R15)
LD        R1,+62(R15)
STO       R0,+6(R15)
STO       R1,+8(R15)
WRST      +64(R15)
WRF       +6(R15)
WRNL      
LD        R0,#6
STO       R0,+10(R15)
WRST      +70(R15)
WRI       +10(R15)
WRNL      
HALT      
LABEL     VARS
INT       0
REAL      0.0
REAL      0.0
INT       0
LABEL     STR0
STRING    ":012x ::="
REAL      2.3
REAL      7.1
REAL      2.3
REAL      5.0
REAL      2.3
LABEL     STR1
STRING    ":012y ::="
REAL      5.0
REAL      3.6
REAL      2.9
REAL      3.6
REAL      5.0
LABEL     STR2
STRING    ":012z ::="
LABEL     STR3
STRING    ":012n ::="
LABEL     BOOL
STRING    "False"
STRING    "True"
