LDA       R15,VARS
LDA       R14,STRS
LDA       R13,BOOL
WRI       #8
WRNL      
HALT      
LABEL     VARS
LABEL     STRS
LABEL     BOOL
STRING    "False"
STRING    "True"
