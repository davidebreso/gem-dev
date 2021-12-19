#include "ppddesk.h"

void __near debugbreak(void)
{
    _asm {
        int 3
    };
}

__declspec( naked ) WORD __near getcs(void) {
    _asm{
        mov ax, cs
        ret
    }
}
#pragma aux getcs value [ax] modify exact [ax] nomemory;

__declspec( naked ) WORD __near getip(void) {
    _asm{
        pop ax
        push ax
        ret
    }
}
#pragma aux getip value [ax] modify exact [ax] nomemory;

