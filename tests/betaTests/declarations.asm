LDA       R15,VARS
LDA       R14,STRS
LDA       R13,BOOL
WRST       +0(R14)
WRNL      
RDI       +0(R15)
RDI       +2(R15)
RDI       +4(R15)
WRI       +0(R15)
WRI       +2(R15)
WRI       +4(R15)
WRNL      
WRST       +20(R14)
WRNL      
WRST       +24(R14)
WRNL      
RDF       +6(R15)
RDF       +10(R15)
RDF       +14(R15)
WRF       +6(R15)
WRF       +10(R15)
WRF       +14(R15)
WRNL      
WRST       +48(R14)
WRNL      
WRI       #99
WRI       #2345
WRI       #10
WRNL      
WRF       +22(R15)
