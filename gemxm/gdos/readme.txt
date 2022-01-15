---------------------------------------------------------------------------
FreeGEM/XM                                             Release 3.0beta5-je1
Source distribution                                          Build 20060515
README.TXT - Information on the FreeGEM/XM GDOS source code.
---------------------------------------------------------------------------
Copyright (C) Digital Research, Inc. 1985 - 1986.
          (C) Caldera Thin Clients, Inc. 1999.
          (C) FreeGEM Programmers, 2000, 2006.
          This software is released under the GNU General Public License
---------------------------------------------------------------------------

This archive contains the source to the FreeGEM/XM GDOS/GEMVDI, as well as
a compiled binary.

This release includes:
* John Elliott's modifications to use modern DOS I/O functions rather than 
 FCBs, which allow the code to function properly on certain modern-day
 (and/or defective) DOS kernels. 
* Ben Jemmett's changes to the two keyboard interrupt hooks, which add 
 support for the extended keyboard functions.  This restores the DOS-mode 
 menu and clipboard functionality in applications that would not react to 
 the hotkey in previous releases.
* The builtin search path for the AES is now "..\GEMBOOT" (like GEM/3) 
 rather than "\GEMBOOT;\GEMAPPS" (like GEM/2).
* A new GDOS escape has been added to set the name of the clipboard file
 to use (rather than hardcoding C:\GEMSCRAP\SCRAP.TXT). The matching AES 
 release will set this up, but if you want to call it by hand it's:

	contrl[0] = -1
	contrl[5] = 81
	intin[0-1] = far pointer to ASCIIZ clipboard pathname.

John Elliott
15 May 2006.
