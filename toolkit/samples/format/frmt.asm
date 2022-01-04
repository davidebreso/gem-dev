Dosseg
.model small
.8086
.stack 100h
.data

     Drive       db      00
     Formattype  db      00
     Curhead     db      00
     Curcylinder db      00

     Parmlen     equ     75
     Origparms:  ; equ     THIS BYTE
     Sforig      db      00
                 db      74 dup(0)

     Newparms:   ; equ     THIS BYTE
     Specfuncnew db      00
     Devicetype  db      00
     Deviceattr  dw      0000
     Cylinders   dw      0000
     Mediatype   db      00

     Diskbpb:    ; equ     THIS BYTE
     Bytessect   dw      0000
     Sectclust   db      00
     Resvsect    dw      0000
     Numfats     db      00
     Rootdir     dw      0000
     Numsect     dw      0000
     Mediaid     db      00
     Sectfat     dw      0000
     Secttrack   dw      0000
     Numheads    dw      0000
     Numhidden   dd      00000000
     Numhuge     dd      00000000
     Unused      db      6 dup (0)
     Tracklayout dw      18*2+1 dup (0)

     Fmtparms:   ; equ     THIS BYTE
     Fmtparmsf   db      00
     Fmtparmhead dw      0000
     Fmtparmcyl  dw      0000

     Midstruct:  ; equ     THIS BYTE
     Infolevel   dw      0000
     Serialnum   dd      06221972
     Vollabel    db      '           '
     Filesys     db      'FAT12   '

     Diskio:     ; equ     THIS BYTE
     Sectbegin   dw      0000,0000
     Sectcount   dw      0000
     Buffstart   dw      0000,0000

     Dctable     dw      offset d360
                 dw      offset d120
                 dw      offset d720
                 dw      offset d144

     D360        dw      40                  ;Number of cylinders (tracks)
                 dw      512                 ;Bytes per sector
                 db      2                   ;Sectors per cluster
                 dw      1                   ;Reserved sectors
                 db      2                   ;Number of fats
                 dw      112                 ;Root directory entries
                 dw      40*9*2              ;Total sectors
                 db      0fdh                ;Media descriptor
                 dw      2                   ;Sectors per fat
                 dw      9                   ;Sectors per track
                 dw      2                   ;Number of heads
                 dd      00000000            ;Hidden sectors
                 dd      00000000            ;Large sector count

     D120        dw      80                  ;Number of cylinders (tracks)
                 dw      512                 ;Bytes per sector
                 db      1                   ;Sectors per cluster
                 dw      1                   ;Reserved sectors
                 db      2                   ;Number of fats
                 dw      224                 ;Root directory entries
                 dw      80*15*2             ;Total sectors
                 db      0f9h                ;Media descriptor
                 dw      7                   ;Sectors per fat
                 dw      15                  ;Sectors per track
                 dw      2                   ;Number of heads
                 dd      00000000            ;Hidden sectors
                 dd      00000000            ;Large sector count

     D720        dw      80                  ;Number of cylinders (tracks)
                 dw      512                 ;Bytes per sector
                 db      2                   ;Sectors per cluster
                 dw      1                   ;Reserved sectors
                 db      2                   ;Number of fats
                 dw      112                 ;Root directory entries
                 dw      80*9*2              ;Total sectors
                 db      0f9h                ;Media descriptor
                 dw      3                   ;Sectors per fat
                 dw      9                   ;Sectors per track
                 dw      2                   ;Number of heads
                 dd      00000000            ;Hidden sectors
                 dd      00000000            ;Large sector count

     D144        dw      80                  ;Number of cylinders (tracks)
                 dw      512                 ;Bytes per sector
                 db      1                   ;Sectors per cluster
                 dw      1                   ;Reserved sectors
                 db      2                   ;Number of fats
                 dw      224                 ;Root directory entries
                 dw      80*18*2             ;Total sectors
                 db      0f0h                ;Media descriptor
                 dw      9                   ;Sectors per fat
                 dw      18                  ;Sectors per track
                 dw      2                   ;Number of heads
                 dd      00000000            ;Hidden sectors
                 dd      00000000            ;Large sector count

     Drmsg2      db      ' disk in drive '
     Ascdrive    db      'A:',13,10,0

     Fmtmsg      db      'Formatting....',0

     Fttable     dw      offset Ft00
                 dw      offset Ft01
                 dw      offset Ft02
                 dw      offset Ft03

     Ft00        db      '360k 5.25"',0
     Ft01        db      '1.2m 5.25"',0
     Ft02        db      '720k 3.5"',0
     Ft03        db      '1.44m 3.5"',0

     Verbose     db      0
     Trackmsg1   db      13,'Formatting track ',0
     Trackmsg2   db      ' of ',0
     Finishmsg   db      13,'Format completed.........',13,10,0

     InsertDisk  db      'Please insert disk to format.',13,10
                 db      ' Press any key when ready...',13,10,0
     Emsg1       db      'Could not determine drive parameters.',13,10,0
     Emsg2       db      'Could not work with this device type.',13,10,0
     Emsg3       db      'Could not set proper parameters.',13,10,0
     Emsg4       db      13,10,'Error formatting track...',13,10,0

.code
Even                                    ; make all math even for faster execution
           PUBLIC    "C",FrmtDisk_      ; make FrmtDisk public
            ;Paramters:
            ;   AX: FDrive% = 1 = a:, 2 = b:, can't format hard drive
            ;   DX: FSize% = 0 = 360k, 1 = 1.2m, 2 = 720k, 3 = 1.44m
            ;   BX: Verb% = not(0) Doesn't print anything, 0 prints status, etc.
FrmtDisk_   PROC near c uses bp ax bx cx dx si di ds es 

            mov     Verbose,bl          ; Verb is in BX
                                        ; FSize in in DX
            mov     formattype,dl       ; Store format size
                                        ; FDrive in in AX
            mov     drive,al            ;Store drive number
            add     al,64               ;Make it into an ASCII character
            mov     ascdrive,al         ;Store it
            mov     al,0                ;Special function desired
            call    getparms            ;Go find out about drive
            jnc     NoErr1
            jmp     error1              ;Didn't work, so exit
NoErr1:     call    xferparms           ;Transfer to real work area

            mov     al,devicetype
            cmp     al,3                ;Valid device types?
            jb      deviceok
            cmp     al,7                ;Still valid?
            je      NotErr
            jmp     error2              ;Nope, so exit
NotErr:     mov     al,3                ;Force 7=3 for format type

Deviceok:
Ready:      mov     al,formattype       ; Assume 0 or 2
            shl     al,1                ;X's by 2
            mov     ah,0
            mov     bx,offset fttable
            add     bx,ax               ;Adjust for proper message
            mov     si,[bx]             ;Get address from table
            call    prtstring
            mov     si,offset drmsg2
            call    prtstring

            call    doparms             ;Set up parameters for format
            mov     al,5                ;Special code desired
            call    setparms            ;Send them to IOCTL
            jc      error3

            mov     si,offset InsertDisk
            call    prtstring
            xor     ah,ah
            int     16h

            mov     curhead,0
            mov     cx,cylinders        ;Number of tracks to format
            mov     curcylinder,-1      ;Track 0
Fmtloop:    inc     curcylinder         ;
            call    status              ;Display status
            call    formattrack         ;Format the track
            jc      error4
            loop    fmtloop

            call    writeboot           ;Write the boot sector
            call    setmid

            call    writefat            ;Write the fat
            call    writedir            ;Write the directory

            call    restparms           ;Set the ioctl back right
            mov     si,offset finishmsg
            call    prtstring
            jmp     alldone

Error1:     mov     si,offset emsg1
            jmp     errcommon
Error2:     mov     si,offset emsg2
            jmp     errcommon
Error3:     call    restparms           ;Restore parameters
            mov     si,offset emsg3
            jmp     errcommon
Error4:     call    restparms           ;Restore parameters
            mov     si,offset emsg4
Errcommon:  call    prtstring
Alldone:   ret
FrmtDisk_   ENDP


; Get current parms for desired drive.  AL = to special function
Getparms    proc near uses ax bx cx dx
            mov     sforig,al           ;Want for current bpb
            mov     ah,44h              ;IOCTL
            mov     al,0dh              ;Generic block I/O
            mov     bl,drive
            mov     ch,08h              ;Disk drive
            mov     cl,60h              ;Get device parameters
            mov     dx,offset origparms
            int     21h
            ret
Getparms    endp

; Transfer from retrieved parameter area into one that will be used
; for the real work in the program
Xferparms   proc near uses cx si di es
            cld
            push    ds
            pop     es
            mov     si,offset origparms
            mov     di,offset newparms
            mov     cx,parmlen
            rep     movsb
            clc                         ;Exit with no error
            ret
Xferparms   endp


; Set parameters.  AL = to special function desired
Setparms    proc near uses ax bx cx dx
            mov     specfuncnew,al
            mov     ah,44h              ;Ioctl
            mov     al,0dh              ;Generic block I/O
            mov     bl,drive
            mov     ch,08h              ;Disk drive
            mov     cl,40h              ;Set device parameters
            mov     dx,offset newparms
            int     21h
            ret
Setparms    endp


; Restore parms to their old condition
Restparms   proc near uses ax bx cx dx
            mov     sforig,00000100b
            mov     ah,44h              ;Ioctl
            mov     al,0dh              ;Generic block I/O
            mov     bl,drive
            mov     ch,08h              ;Disk drive
            mov     cl,40h              ;Set device parameters
            mov     dx,offset origparms
            int     21h
            ret
Restparms   endp


; Set up parameters (including BPB) for selected format type.  Also
; transfers information into the BPB in the boot sector.
Doparms     proc near uses ax bx cx si di es
            mov     ah,0
            mov     al,formattype
            shl     ax,1                ;X'2 by 2
            mov     bx,offset dctable
            add     bx,ax               ;Adjust for format type
            mov     si,[bx]             ;Get proper address
            mov     ax,[si]             ;Get number of cylinders
            inc     si                  ;Point to next parameter
            inc     si
            cmp     cylinders,ax        ;Default capacity for drive?
            je      dp1
            mov     mediatype,1         ;Set for low format
            mov     cylinders,ax
Dp1:        push    si                  ;Store for a moment
            mov     cx,25               ;Bytes to transfer
            mov     di,offset diskbpb
            push    ds
            pop     es
            cld
            rep     movsb               ;Transfer parms
            call    dolayout

            pop     si                  ;Get start of bpb back
            mov     ax,seg bootbpb
            mov     es,ax
            mov     di,offset es:bootbpb
            mov     cx,25
            rep     movsb               ;Transfer to boot sector
            ret
Doparms     endp


; set the track layout field
Dolayout    proc near uses ax bx cx si di es
            cld
            mov     di,offset tracklayout
            mov     ax,secttrack
            push    ds
            pop     es
            stosw
            mov     bx,1                ;First sector number
            mov     cx,ax               ;Number of sectors per track
            mov     dx,bytessect        ;Bytes per sector
Trkloop:    mov     ax,bx               ;Sector number
            stosw
            inc     bx                  ;Point to next sector
            mov     ax,dx               ;Bytes per sector
            stosw
            loop    trkloop             ;Do it all again
            ret
Dolayout    endp

; set the media id
Setmid      proc near uses ax bx cx dx
            mov     ah,44h              ;IOCTL
            mov     al,0dh              ;Generic block I/O
            mov     bl,drive
            mov     ch,08h              ;Disk drive
            mov     cl,46h              ;Set media ID
            mov     dx,offset midstruct
            int     21h
            ret
Setmid      endp

; Handle formatting a single track.  Return with carry set if error
Formattrack proc near uses cx
            mov     cx,numheads
            dec     cx                  ;Make zero based
Ftloop:     mov     curhead,cl
            call    formatside
            jc      ftdone
            loop    ftloop
            clc                         ;Return with no errors
Ftdone:     ret
Formattrack endp

; Format one head of a track.  Return with carry set if error
Formatside  proc near uses ax bx cx dx
            mov     fmtparmsf,0
            mov     ah,0
            mov     al,curhead
            mov     fmtparmhead,ax
            mov     al,curcylinder
            mov     fmtparmcyl,ax

            mov     ah,44h              ;IOCTL
            mov     al,0Dh              ;Generic block I/O
            mov     bh,0
            mov     bl,drive
            mov     ch,08h              ;Disk drive
            mov     cl,42h              ;Format/verify track
            mov     dx,offset fmtparms
            int     21h
            ret
Formatside  endp

; Display the status message during formatting
Status      proc near uses ax si
            mov     si,offset trackmsg1
            call    prtstring
            mov     ah,0
            mov     al,curcylinder
            inc     ax
            call    prtdec
            mov     si,offset trackmsg2
            call    prtstring
            mov     ax,cylinders
            call    prtdec
            ret
Status      endp

Prtstring   proc near uses ax dx si
            cmp     Verbose,0
            jne     Ps2
Ps1:        mov     dl,[si]             ;Get character
            inc     si                  ;Point to next one
            cmp     dl,0                ;End of string?
            je      ps2                 ;Yes, so exit
            mov     ah,02h              ;Output a character
            int     21h
            jmp     ps1                 ;Keep doing it
Ps2:        ret
Prtstring   endp

Keymsg      proc near uses ax
            call    prtstring
            mov     ah,0
            int     16h
            ret
Keymsg      endp


; The following routine prints the value in ax as a decimal number
Prtdec      proc near uses ax cx dx
            cmp     Verbose,0
            jne     Pd3
            mov     cx,0FFFFh           ;Ending flag
            push    cx
            mov     cx,10
Pd1:        mov     dx,0
            div     cx                  ;Divide by 10
            add     dl,30h              ;Convert to ASCII
            push    dx                  ;Store remainder
            cmp     ax,0                ;Are we done?
            jne     pd1                 ;No, so continue

Pd2:        pop     dx                  ;Character is now in DL
            cmp     dx,0FFFFh           ;Is it the ending flag?
            je      pd3                 ;Yes, so continue
            mov     ah,02h              ;Output a character
            int     21h
            jmp     pd2                 ;Keep doing it

Pd3:        ret
Prtdec      endp


; Write the boot sector to the disk
Writeboot   proc near uses ax bx cx es ds
            mov     ax,seg bootsector   ;Point to boot sector
            mov     es,ax
            mov     bx,offset es:bootsector
            mov     buffstart[0],bx
            mov     buffstart[2],es
            mov     sectbegin,0
            mov     sectcount,1
            mov     al,drive
            dec     al                  ;Make it zero based
            mov     bx,offset diskio    ;Point to buffer
            mov     cx,-1
            int     26h
            popf                        ;Get rid of flags
            ret
Writeboot   endp

; Write the fat to the disk
Writefat    proc near uses ax bx cx dx di es ds
            mov     ax,sectfat
            mov     bl,32               ;Paragraphs per 512 bytes
            mul     bl                  ;Ax now contains paragraphs needed
            mov     bx,ax               ;   for one fat image
            mov     ah,48h              ;Allocate memory
            int     21h                 ;DOS services
            mov     es,ax               ;Put in proper place

            mov     ax,sectfat
            mov     cl,8                ;2^8 power (256)
            shl     ax,cl               ;Number of words in memory area
            mov     cx,ax               ;Iteration count
            mov     di,0                ;Point to start of memory block
            mov     ax,0                ;Want to clear memory area
            cld                         ;Go in proper direction
            rep     stosw               ;Store 0s in memory

            mov     ax,0FFFFh
            mov     bl,mediaid
            mov     es:[0],bl           ;Store FAT ID
            mov     es:[1],ax           ;Store filler

            mov     ax,1
            mov     ch,0
            mov     cl,numfats
Fatloop:    call    putfat
            add     ax,sectfat
            loop    fatloop

            mov     ah,49h              ;Release memory block
            int     21h                 ;DOS services

            ret
Writefat    endp

; Put one copy of the fat on disk.  AX=beginning sector number
Putfat      proc near uses ax bx cx es
            mov     buffstart[0],0
            mov     buffstart[2],es
            mov     sectbegin,ax
            mov     ax,sectfat
            mov     sectcount,ax
            mov     al,drive
            dec     al
            mov     bx,offset diskio
            mov     cx,-1
            int     26h
            popf                       ;Get rid of flags
            ret
Putfat      endp

; Write the directory to the disk
Writedir    proc near uses ax bx cx dx di es ds
            mov     ax,rootdir          ;Maximum root directory entries
            mov     cl,4                ;2^4 power (16)
            shr     ax,cl               ;Divide by 16, now ax = sectors needed
            push    ax                  ;Store for later

            mov     bl,32               ;Paragraphs per 512 bytes
            mul     bl                  ;AX now contains paragraphs needed
            mov     bx,ax
            mov     ah,48h              ;Allocate memory
            int     21h                 ;DOS services
            mov     es,ax               ;Put in proper place

            pop     ax                  ;Get back number of directory sectors
            push    ax                  ;And store for later
            mov     cl,8                ;2^8 power (256)
            shl     ax,cl               ;AX now has number of words in data area
            mov     cx,ax               ;Put into loop counter
            mov     di,0                ;Start at beginning of block
            mov     ax,0                ;Want to clear memory area
            cld                         ;Go in proper direction
            rep     stosw               ;Store 0s in memory

            mov     ax,sectfat          ;Number of sectors in one FAT
            mov     bl,numfats          ;Get number of fats
            mul     bl                  ;AX now has number of sectors
            inc     ax                  ;Point to sector past FAT

            mov     buffstart[0],0
            mov     buffstart[2],es
            mov     sectbegin,ax
            pop     ax                  ;Get back directory sectors
            mov     sectcount,ax
            mov     al,drive
            dec     al
            mov     bx,offset diskio
            mov     cx,-1
            int     26h
            popf                        ;Get rid of flags

            mov     ah,49h              ;Release memory block
            int     21h                 ;DOS services
            ret
Writedir    endp

; This is the actual code that will be written to disk for the boot sector.
Bsect       segment public
            assume cs:bsect, ds:bsect
            org     0                   ;Boot sector will be here

Bootsector  db     0ebh,03ch,090h

            db      'FYFYFYFY'
Bootbpb     db      25 dup (0)
            db      00                  ;Drive number, if a hard disk
            db      00                  ;Reserved
            db      29h                 ;Extended boot signature
            dd      06221972            ;Volume serial number
            db      'formatted 1'       ;Volume label
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
            db      '     This disk was formatted with a program from',13,10
            db      '        Forever Young Software, by Ben Lunt.',13,10
            db      13,10
            db      '     This is a non-system disk.  Remove this disk',13,10
            db      '        and press any key to reboot your system.',13,10
            db      13,10, 43 dup (' '), 0

            org     510
            db      55h                 ;Boot sector has to end with
            db      0AAh                ;   these bytes, in this order
Bsect       ends
           end                          ; end of assembly code
