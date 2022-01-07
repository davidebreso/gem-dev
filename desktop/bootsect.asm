.8086
; This is the actual code that will be written to disk for the boot sector.
Bsect       segment public
            assume cs:bsect, ds:bsect
            org     0                   ;Boot sector will be here
            public  "C",Bootsector      ; make Bootsector public
            public  "C",Bootbpb         ; make Bootbpb public
            public  "C",Bootserial      ; make Bootserial public
            public  "C",Bootlabel       ; make Bootserial public

Bootsector  db     0ebh,03ch,090h

            db      'GEMDESK '
Bootbpb     db      25 dup (0)
            db      00                  ;Drive number, if a hard disk
            db      00                  ;Reserved
            db      29h                 ;Extended boot signature
Bootserial  dd      00                  ;Volume serial number
Bootlabel   db      '           '       ;Volume label
            db      'fat12   '          ;File type

Bootcode:   mov     ax,7C0h             ;Standard loading place for
            mov     ds,ax               ;   boot sector
            mov     es,ax

            mov     ah,0fh              ;Get current display mode
            int     10h                 ;Bios services

            mov     si,offset bootmsg
Msgloop:    mov     al,[si]             ;Get next character
            or      al,al               ;End of string?
            jz      msgdone             ;Yes, so end
            mov     ah,0Eh              ;Write text in teletype mode
            int     10h                 ;Bios services
            inc     si
            jmp     short msgloop

Msgdone:    mov     ah,00h              ;Read keyboard character
            int     16h                 ;Bios services
            mov     al,13               ;Process a carriage return
            mov     ah,0eh
            int     10h
            mov     al,10               ;Process a line feed
            mov     ah,0eh
            int     10h
            int     19h                 ;Do a warm boot

Bootmsg     db      13,10,13,10
            db      '     This disk was formatted with GEM Desktop 3.2.6.',13,10
            db      13,10
            db      '     This is a non-system disk.  Remove this disk',13,10
            db      '        and press any key to reboot your system.',13,10
            db      13,10, 43 dup (' '), 0

            org     510
            db      55h                 ;Boot sector has to end with
            db      0AAh                ;   these bytes, in this order
Bsect       ends
           end                          ; end of assembly code
