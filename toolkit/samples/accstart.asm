;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  DOS 16-bit startup code.
;*
;*****************************************************************************


;       This must be assembled using one of the following commands:
;               wasm cstrt086 -bt=DOS -ms -0r
;               wasm cstrt086 -bt=DOS -ml -0r
;               wasm cstrt086 -bt=DOS -mh -0r
;

.286p

        name    accstart

        assume  nothing


 DGROUP group _NULL,_AFTERNULL,CONST,STRINGS,_DATA,DATA,_BSS

_TEXT   segment word public 'CODE'

        extrn   __CMain                 : proc
        extrn   __InitRtns              : proc
;        extrn   __FiniRtns              : proc

_TEXT   ends

FAR_DATA segment byte public 'FAR_DATA'
FAR_DATA ends

        assume  ds:DGROUP

_NULL   segment para public 'BEGDATA'
_NULL   ends

_AFTERNULL segment word public 'BEGDATA'
_AFTERNULL ends

CONST   segment word public 'DATA'
CONST   ends

STRINGS segment word public 'DATA'
STRINGS ends

_DATA   segment word public 'DATA'

        extrn   "C",_curbrk             : word
        extrn   "C",_psp                : word
        extrn   "C",_osmajor            : byte
        extrn   "C",_osminor            : byte
        extrn   "C",_osmode             : byte
;        extrn   "C",_HShift             : byte
        extrn   "C",_STACKLOW           : word
        extrn   "C",_STACKTOP           : word
;        extrn   "C",_cbyte              : word
;        extrn   __child                 : word
        extrn   __no87                  : byte
        extrn   "C",__uselfn            : byte
        extrn   "C",__FPE_handler       : dword
        extrn   "C",_LpCmdLine          : dword
        extrn   "C",_LpPgmName          : dword
        extrn   __get_ovl_stack         : word
        extrn   __restore_ovl_stack     : word
        extrn   __close_ovl_file        : word
;        extrn   __DOSseg__              : byte

_DATA   ends

DATA    segment word public 'DATA'
DATA    ends

_BSS    segment word public 'BSS'

        extrn   _edata                  : byte  ; end of DATA (start of BSS)
        extrn   _end                    : byte  ; end of BSS (start of STACK)

_BSS    ends

;
; Create a special segment EDATA that contains a dummy variable.
; This special segment EDATA will be placed at the end of the memory map by WLINK,
; so that the executable file contains all memory needed by the program,
; including the _BSS and STACK segments that are normally not contained in the 
; executable file.
;
EDATA    segment word public 'EDATA'
_end_of_world   dw 0AA55h
EDATA   ends

; to guarantee that the _accstart function is at address CS:0 we it in BEGTEXT 
; (WLINK will keep segment 'BEGTEXT' in front).

BEGTEXT  segment word public 'CODE'
        assume  nothing

        assume  cs:BEGTEXT

_accstart proc near
        public  "C",_accstart

        int     3
        sti                             ; enable interrupts
        mov     cx,DGROUP               ; get proper stack segment

        assume  es:DGROUP

        mov     es,cx                   ; point to data segment
        mov     bx,offset DGROUP:_end   ; get bottom of stack
        add     bx,0Fh                  ; ...
        and     bl,0F0h                 ; ...
        mov     _STACKLOW,bx            ; this is actually es:_STACKLOW
        mov     _psp,ds                 ; and es:_psp. Save segment address of PSP
                                        ; WHAT TO DO HERE? there is no PSP in a DA
                                        ; SP should be equal to stack size (800h) ?
        add     bx, STACKSIZ            ; calculate top address for stack
        add     bx,0Fh                  ; round up to paragraph boundary
        jnc     not64k                  ; if 64K
        mov     bx,0fffeh               ; set _STACKTOP to 0xfffe
not64k:
        and     bl,0F0h                 ; round up to paragraph boundary
        mov     ss,cx                   ; set stack segment
        mov     sp,bx                   ; set sp relative to DGROUP
        mov     _STACKTOP,bx            ; set stack top
        mov     _curbrk,bx              ; set top of memory owned by process 
                                        ; (this is actually es:_curbrk)
        mov     dx,DGROUP
        mov     es,dx                   ; es:di is destination
        mov     di,_STACKLOW
        mov     word ptr _LpCmdLine+0,di ; stash lpCmdLine pointer
        mov     word ptr _LpCmdLine+2,es ; ...
        sub     al,al
        stosb                           ; store NULLCHAR
        stosb                           ; no pgm name, store NULLCHAR
        dec     di                      ; back up pointer 1
;
;       get DOS version number
;
        mov     ah,30h
        int     21h
        mov     _osmajor,al             ; this is es:_osmajor
        mov     _osminor,ah             ; this is es:_osminor
        mov     cx,di                   ; remember address of pgm name
        assume  ds:DGROUP
        mov     ds,dx
        mov     si,cx                   ; save address of pgm name
        mov     word ptr _LpPgmName+0,si ; stash LpPgmName pointer
        mov     word ptr _LpPgmName+2,es ; ...
        mov     bx,sp                   ; end of stack in data segment
        mov     ax,0100h                ; 100h if NO87 not present and LFN=N not present
        mov     __no87,al               ; set state of "NO87" environment var
        and     __uselfn,ah             ; set "LFN" support status
        mov     _STACKLOW,di            ; save low address of stack

        mov     cx,offset DGROUP:_end   ; end of _BSS segment (start of STACK)
        mov     di,offset DGROUP:_edata ; start of _BSS segment
        sub     cx,di                   ; calc # of bytes in _BSS segment
        xor     al,al                   ; zero the _BSS segment
        rep     stosb                   ; . . .

        cmp     word ptr __get_ovl_stack,0 ; if program not overlayed
        jne     _is_ovl                 ; then
        mov     ax,offset __null_ovl_rtn; - set vectors to null rtn
        mov     __get_ovl_stack,ax      ; - ...
        mov     __get_ovl_stack+2,cs    ; - ...
        mov     __restore_ovl_stack,ax  ; - ...
        mov     __restore_ovl_stack+2,cs; - ...
        mov     __close_ovl_file,ax     ; - ...
        mov     __close_ovl_file+2,cs   ; - ...
_is_ovl:                                ; endif
        xor     bp,bp                   ; set up stack frame
if __LARGE__
        push    bp                      ; ... for new overlay manager
        mov     bp,sp                   ; ...
endif
        ; DON'T MODIFY BP FROM THIS POINT ON!
        mov     ax,offset __null_FPE_rtn; initialize floating-point exception
        mov     word ptr __FPE_handler,ax       ; ... handler address
        mov     word ptr __FPE_handler+2,cs     ; ...

        mov     ax,0FFh                 ; run all initalizers
        call    __InitRtns              ; call initializer routines
        call    __CMain
_accstart endp

__null_FPE_rtn proc far
        ret                             ; return
__null_FPE_rtn endp

__null_ovl_rtn proc far
        ret                             ; return
__null_ovl_rtn endp

BEGTEXT   ends

        end  
