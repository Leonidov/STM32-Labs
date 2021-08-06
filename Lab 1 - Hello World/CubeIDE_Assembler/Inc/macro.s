//псевдокоманда MOV32
.macro	MOV32 regnum,number
  MOVW \regnum,:lower16:\number
  MOVT \regnum,:upper16:\number
.endm
