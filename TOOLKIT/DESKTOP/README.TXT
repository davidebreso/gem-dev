GEM/3 Desktop: Pacific C version (Release 3.2.3-pacc10)
=======================================================

  This is a GEM desktop, based on Digital Research's GEM Desktop 3.0. Features 
that have been added include:

* Four windows and desktop icons (like GEM/1)
* ViewMAX look and feel
* 'Show contents' function (from ViewMAX)

  There are two APPs in this package. DESKTOP.APP is compiled in the small
model, while DESKTOPL.APP is compiled in the large model.

Recent Changes 
==============
* Bugfix: String length-checking broke 'Enter DOS Commands'. Unbreak it.

* By using "Install Application" on the Desktop itself, you can get it to 
open certain file types in its internal viewer when you double-click them. 
I recommend you do this by manually editing DESKTOP.INF, and adding 
(near to the top) a line something like:

#G082A DESKTOP.APP@ *.TXT,*.ME,*.INF@

  though you could also do it with the 'configure application' screen.
Doing it by manually editing DESKTOP.INF has two benefits:
  1. Earlier entries in DESKTOP.INF are superseded by later ones, so by 
    inserting the Desktop entry at the top we can be sure that any 
    associations made by the user will supersede it if necessary.
  2. The line I've given uses the 'generic' application icon, but the 
    'word processor' document icon. The Desktop's configure screen can't
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
======================
  If you want it to look pretty, this version of DESKTOP should be run on
a FreeGEM AES. It will also work on a stock GEM/3 AES, though it won't look
as nice.

To compile DESKTOP.APP from the source in this archive, you will need the
Pacific C compiler
<http://www.hitech.com.au/>
and my GEM bindings for it:
<http://www.seasip.demon.co.uk/Gem/>

John Elliott, 26 August 2005.

