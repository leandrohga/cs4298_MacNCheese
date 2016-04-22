LDA       R15,VARS
LDA       R14,STRS
LDA       R13,BOOL
WRST       +0(R14)
WRI       #1
WRST       +4(R14)
WRF       +12(R15)
WRST       +14(R14)
WRNL      
WRST       +16(R14)
WRNL      
