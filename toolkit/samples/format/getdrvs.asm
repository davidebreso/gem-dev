; Get all of the Disk Drives accessible by DOS. 
; Version: 1.00b
; Author: Ben Lunt (Forever Young Software)
; Date: 09 Dec 1998
; URL: http://www.fysnet.net/getdrvs.htm

.model tiny
.8086
.code
org 100h

; we know that all machines have a drive a: (first floppy)
; we know that all machines have a drive b: also, whether it be
;  a second floppy drive or a logical floppy drive.
; let us use DOS's Low Memory to tell us if drive b: is a floppy
;  or if it is a logical floppy.
start:
           mov  dx,offset DrvAChkS
           mov  ah,09
           int  21h
           call GetADrvs
           mov  ah,02                   ; print 'A'
           mov  dl,'A'
           int  21h
           test cl,00000001b
           jz   short DrvANPrs
           mov  dx,offset DrvThere
           jmp  short DrvAPrsP
DrvANPrs:  mov  dx,offset DrvNThere
DrvAPrsP:  mov  ah,09
           int  21h
           mov  ah,02                   ; print 'B'
           mov  dl,'B'
           int  21h
           test cl,00000010b
           jz   short DrvBNPrs
           mov  dx,offset DrvThere
           jmp  short DrvBPrsP
DrvBNPrs:  mov  dx,offset DrvNThere
DrvBPrsP:  mov  ah,09
           int  21h


; we need to save current drive letter so when routine is done, we will
;  be on same drive that we started

           mov  ah,19h                  ; save current disk drive
           int  21h                     ;
           push ax                      ; save the letter

; we scan drives c: through z: to check if they're are accessible by DOS.
;   The way we do it:
;     Change drive to DriveLetterWanted
;     Try to access drive
;       if accessed then drive ok, else drive not present
;
; NOTE:  When changing to a drive using service 0Eh of INT 21h,
;         no physical thing happens until next drive access.
;         So, we must try to access it using service 19h (get current disk).
;         This service will report an error if disk drive not available.

           mov  dl,01h                  ; check for the drive (start w/ C:)
           mov  cx,24                   ; do 24 times
LoopIt:    inc  dl                      ; inc to next drive letter
           push dx                      ; save drive letter
           push dx                      ; print drive letter
           mov  ah,02                   ;
           add  dl,65                   ;
           int  21h                     ;
           pop  dx                      ;
           mov  ah,0Eh                  ; select disk drive service
           int  21h                     ;
           mov  ah,19h                  ; Try to access it
           int  21h                     ;
           cmp  al,dl                   ; if current drive = saved drive
           jne  short NotExist          ;
           mov  dx,offset DrvThere      ;
           jmp  short DoesExist         ;
NotExist:  mov  dx,offset DrvNThere     ;
DoesExist: mov  ah,09                   ;
           int  21h                     ;
           pop  dx                      ;
           loop LoopIt                  ;


; restore save drive letter from above
           pop  dx                      ; restore saved drive letter
           mov  ah,0Eh                  ; restore current disk
           int  21h                     ;

Done:      mov  ah,4Ch                  ; exit to DOS
           int  21h                     ; 

GetADrvs   proc near uses ax bx dx es
                                        ;                      (floppies only)
           xor  cl,cl                   ; bits set represent drvs available
           mov  dx,0040h                ; low memory area we want to use
           mov  es,dx                   ;
           mov  bx,0010h                ; we want word at 0040:0010h
           mov  dx,es:[bx]              ;
           shr  dl,01h                  ; Bit 0 of dx: 0- no floppies
           jnc  short NoFlops           ;              1- floppies exist
           push cx
           mov  cl,05h
           shr  dl,cl                   ; bits 7 and 6 show how many
           pop  cx
           jz   short OneDrv            ;  0- one drive, 1- two drives
           or   cl,00000011b            ;  2- three drives, 3- four drives
           jmp  short NoFlops           ;
OneDrv:    mov  dx,0050h                ; let us see which drive is active
           mov  es,dx                   ;  a: or b: (which is currently
           mov  bx,0004h                ;  the default drive letter)
           mov  dl,es:[bx]              ; 0- Acting as drive A:
           or   dl,dl                   ; 1- Acting as drive B:
           je   short DrvAasA           ;
           or   cl,00000010b            ; set accordingly
           jmp  short NoFlops           ;  .
DrvAasA:   or   cl,00000001b            ;  .
NoFlops:   ret                          ;
GetADrvs   endp                         ; end of procedure

DrvAChkS  db  13,10,'Checking Floppy drives a: and b:...',13,10,36
DrvThere  db  '   Drive present',13,10,36
DrvNThere db  '   Drive NOT present',13,10,36

end start

; Note: If you use the value in AL after calling service 0Eh, to determine
;         the number of drives in your system, beware.  In MS-DOS versions
;         3.0 and later, the value is determined by taking the largest of
;         the following:  the number of logical drives actually installed
;         (including RAM disks), the number of drives indicated in the
;         LASTDRIVE command in CONFIG.SYS, or the number 5.  Therefore, the
;         minimum value returned by this function is 05h. (Norton p.318)
;
; Note: Even though this routine will return 'Drive NOT present' if you only
;         have one floppy drive, the other drive letter is a logical drive
;         and if accessed will use the single floppy drive renaming its drive
;         letter to B: if was A: (or visa-versa)
;         ie: if the current floppy drive letter is a: and the following was
;           typed at the DOS prompt:
;             b:
;           then the drive would now be accessed with the drive letter 'b'
;           (to change back, simply do the opposite, entering a: at the DOS
;            prompt)  Most of the time, DOS will prompt you before it changes
;            the disk drives' letter (name).
;         **** In a Windows95 DOS session, Windows95 'gracefully' displays
;              a blue screen asking you to enter the disk in the drive.  You
;              would think that Microsoft could have just printed a line at
;              the DOS prompt rather than going to so much trouble and
;              'flashing' this message at you.
;                    (*If it ain't broke don't fix it*)
