GEM/3 Desktop: Open Watcom version (Release 3.2.5)
===============================================================================

  This is a GEM desktop, based on Digital Research's GEM Desktop 3.0. Features 
that have been added include:

* Four windows and desktop icons (like GEM/1)
* ViewMAX look and feel
* 'Show contents' function (from ViewMAX)

  There are four APPs in this package:

DESKTOP.APP  - small model, for single-tasking GEM.
DESKTOPL.APP - large model, for single-tasking GEM.
DESKSM.APP   - small model, for GEM/XM.
DESKLM.APP   - large model, for GEM/XM.

Release 3.2.5 Changes (most recent at the top)
===============================================================================

* Code is now compiled with Open Watcom v2 on MacOS Big Sur. "Show Content" is
  disabled on the small-model desktop. Function that calls DOS interrrupts 
  have been rewritten in ASM to keep the code segment below the 64K limit.

* Icon view auto-arrange icons to match the window size. 

* Bugfix: some portions of code cause Pacific C's global optimisation to fail.
They have been moved to a separate file, so that the rest of the code can be 
compiled with global optimisation enabled and the small-model desktop remains 
below the 64K code segment limit without removing functionalities.

* Multi-tasking GEM builds are disabled by default. 

Release 3.2.4 Changes (most recent at the top)
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

Notes on look and feel
===============================================================================
  If you want it to look pretty, this version of DESKTOP should be run on
a FreeGEM AES. It will also work on a stock GEM/3 AES, though it won't look
as nice.

To compile DESKTOP.APP from the source in this archive, you will need the
Pacific C compiler
<https://clasqm.github.io/freedos-repo/Development.html>
and John Elliott's GEM bindings for it:
<http://www.seasip.info/Gem/index.html>

John Elliott, 26 March 2006
Davide Bresolin, 18 December 2021

