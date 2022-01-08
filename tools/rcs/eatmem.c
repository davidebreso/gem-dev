#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int amount = atoi(argv[1]);
    
    printf("Eating %d KB of memory.\n");
    amount = (amount << 6);     /* get number of paragraphs */
    _asm{
        /* save flags and registers */
        pushf
        push    ax
        push    bx
        /* Get current allocation strategy */
        mov     ax, 0x5800
        int     0x21        
        push    ax          /* save strategy for later */
        /* Set strategy to low memory last fit */
        mov     ax, 0x5801
        mov     bx, 0x0002
        int     0x21
        /* Allocate memory block */
        mov     ax, 0x4800
        mov     bx, amount
        int     0x21
        /* Restore allocation strategy */
        mov     ax, 0x5801
        pop     bx
        int     0x21  
        /* terminate and stay resident to keep memory allocated */
        mov     ax, 0x3100
        mov     dx, 0x00
        int     0x21
    }
    return 0;
}
