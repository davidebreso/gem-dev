------------------------------------------------------------------------
FreeGEM/XM                                          Release 3.0beta5-je1
Source distribution                                       Build 20060515
README.TXT - FreeGEM/XM AES build directory
------------------------------------------------------------------------
Copyright (C) Digital Research, Inc. 1985 - 1986.
          (C) Caldera Thin Clients, Inc. 1999.
          (C) FreeGEM Programmers, 2000, 2006.
          This software is released under the GNU General Public License
------------------------------------------------------------------------

This now builds, links and runs correctly.  The changes made since the
3.0beta5 release are:

 * The AES now assumes a GEM/3 directory layout, with the AES, VDI and
  desktop all in \GEMAPPS\GEMSYS.

 * The menu_click function has been copied in from GEM/3. The version number
  reported by the AES has been increased from 3.00 to 3.01, so that programs
  can tell this function is safe to use. On GEM/XM version of the FreeGEM
  desktop, the "click to drop down menus" preference will be activated.

 * The shell info buffer is now erased to zeroes at AES startup, so that
  drive detection in the FreeGEM desktop now works.

 * The clipboard directory is \GEMAPPS\CLIPBRD. scrp_write calls are passed
  down to the GDOS so that copying and pasting works between DOS and GEM
  programs.

John Elliott
15 May 2006.
