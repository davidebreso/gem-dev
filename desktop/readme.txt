GEM/3 Desktop: Open Watcom version (Release 3.2.6)
===============================================================================

  This is a GEM desktop, based on John Elliott's GEM Desktop 3.2.4 downloadable
from <http://www.seasip.info/Gem/desktops.html>. Features that have been added
to the original Digital Research's GEM Desktop 3.0 include:

* 'Floppy disk format' utility (inspired by EmuTOS)
* 'Install Accessories' for single- and multi-tasking builds
* Auto-arrange of icons to match the window size
* Four windows and desktop icons (like GEM/1)
* ViewMAX look and feel
* 'Show contents' function (from ViewMAX)

  There are four APPs in this package:

DESKTOP.APP  - small model, for single-tasking GEM.
DESKTOPL.APP - large model, for single-tasking GEM.
DESKSM.APP   - small model, for GEM/XM.
DESKLM.APP   - large model, for GEM/XM.

  To install the package unzip the archive in GEMAPPS\GEMSYS. If GEMSYS does 
not contain a DESKTOP.INF, or if the file is from an old version of GEM Desktop, 
rename SKELETON.INF into DESKTOP.INF to have a minimal configuration file.

Release 3.2.7 Changes (most recent at the top) -- Davide Bresolin
===============================================================================

* The structure of the menus have been changed. There is a  "Window" menu with 
  the commands to close folders and windows, cycle through open windows, select
  all items and set file mask. "Execute DOS commands" is now "To DOS shell" 
  and it is placed in the "File" menu. New option "Size to fit" in "View" menu.

Release 3.2.6 Changes 
===============================================================================

* "Install Accessories" is now available also for single-tasking builds.
  The panel lists the .ACC files in GEMAPPS\GEMBOOT as "Available accessories",
  and the first 3 .ACC files in GEMAPPS\GEMSYS as installed accessories.
  Installing an accessory moves the .ACC file from GEMBOOT to GEMSYS,
  removing an accessory moves the .ACC file from GEMSYS to GEMBOOT.
  Since single-tasking GEM cannot load/unload accessories at runtime,
  a restart of GEM is needed to reload the new accessories.

* Added "Refresh window" item in File menu. "Quit" displays a confirmation 
  alert with a "Restart" option to restart GEM AES instead of exiting to DOS.

* Removed keyboard shortcuts from buttons, to make dialogs look better with 
  the stock GEM/3 AES.

* Most of the string manipulation functions have been replaced with equivalent
  functions of the Open Watcom library.
  
* Added a small utility to format floppy disks. "Format Floppy..." pops up a 
  dialog box to select drive, disk label and capacity. The actual formatting
  is executed by the Desktop itself, with no need to call FORMAT.COM.
  The utility is available as a standalone APP in toolkit/samples/format.

Release 3.2.5 
===============================================================================

* Bugfix: fixed file name conflict resolution when copying files.

* Multi-tasking GEM APPs compiles correctly. "Show Contents" is available in
  both the small and large model MULTIAPP desktop but fails to launch reporting
  a lack of memory. 

* Bugfix: correct disk drive detection in "Format Disk"

* Code is now compiled with Open Watcom v2 on MacOS. All functionalities are 
  maintained with a smaller memory footprint than Pacific C's builds.

* Icon view auto-arrange icons to match the window size. 

Release 3.2.4 Changes (most recent at the top) -- John Elliott
===============================================================================

* Checks the GEM version before calling menu_click(); this allows the desktop
to be run on GEM/2 and GEM/XM systems without "Invalid Function #" errors. 
A note to any future GEM/XM maintainer: If you add the functionality of
menu_click(), please bump the version number to 0x301 or higher so the Desktop
can tell that this function is present.

* Builds have been added for multi-tasking GEM. This has mainly involved 
adding the "Install Accessories" screen (completely untested, I might add)
and getting the sections in #if MULTIAPP to compile. The About box has also
been redesigned to accommodate the free memory message. Note that "Show 
Contents" is not available in the small-model MULTIAPP desktop owing to lack 
of space in the code segment; and on my machine, it fails to launch in the 
large-model MULTIAPP desktop citing a lack of memory.

* Bugfix: String length-checking broke 'Enter DOS Commands'. Unbreak it.

* By using "Install Application" on the Desktop itself, you can get it to open 
certain file types in its internal viewer when you double-click them. I 
recommend you do this by manually editing DESKTOP.INF, and adding (near to the 
top) a line something like:

#G082A DESKTOP.APP@ *.TXT,*.ME,*.INF@

  though you could also do it with the 'configure application' screen.
Doing it by manually editing DESKTOP.INF has two benefits:
  1. Earlier entries in DESKTOP.INF are superseded by later ones, so by 
inserting the Desktop entry at the top we can be sure that any associations 
made by the user will supersede it if necessary.
  2. The line I've given uses the 'generic' application icon, but the 'word 
processor' document icon. The Desktop's configure screen can't
    do this.

* The 'path depth' warnings from GEM/3 have been brought back. The Desktop 
will now not allow a path longer than 67 characters; earlier versions would 
crash trying to construct such a path.
* The string functions strcpy and strcat have mostly been replaced with
length-checked versions strlcpy and strlcat.
* Some internal code reorganisation to break up functions into smaller
chunks; this reduces the burden on the Pacific C optimiser.
* Drive auto-detect code has been adapted from Ben Jemmett's Desktop. At 
startup all drives are scanned and any new ones are added to the list. This 
behaviour can be disabled from the Preferences screen.
* DESKHI.ICN and DESKLO.ICN are now up-to-date and contain matching sets
of icons.

Notes on source code
===============================================================================
To compile DESKTOP.APP from the source code, you need the Open Watcom v2 
C compiler, linker and assembler
<https://open-watcom.github.io>
and Davide Bresolin's GEM bindings for it:
<https://github.com/davidebreso/gem-dev>

Davide Bresolin, January 2022
John Elliott, 26 March 2006

