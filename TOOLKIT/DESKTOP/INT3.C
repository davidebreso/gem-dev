

void debugbreak(void)
{
#asm
	int	#3
#endasm
}

#asm
        .globl  _getcs, _getip
_getcs: mov     ax,cs
        ret
;
_getip: pop     ax
        push    ax
        ret
#endasm


