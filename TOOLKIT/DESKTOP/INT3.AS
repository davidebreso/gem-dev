	.globl	small_data
	.globl	small_code
	.psect	_TEXT,class=CODE
	.globl	_debugbreak
	.signat	_debugbreak,24
_debugbreak:
	jmp	f10
f11:
# 5 "INT3.C"
 int #3 ;#
;INT3.C: 7: }
	.psect	_TEXT
l1:
	ret	;returns 0 bytes
f10:
	jmp	f11
