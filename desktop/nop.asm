; A very simple program that terminates immediately

.MODEL small
.STACK 100h
.CODE
start:
    mov ah,4ch  ; DOS terminate program function
    int 21h     ;terminate the program
END start