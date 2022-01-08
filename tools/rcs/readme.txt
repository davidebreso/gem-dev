GEM RCS: version 2.3 GPL for Open Watcom bindings
===================================================

  This version of RCS (the Resource Construction Set) has been ported to
Open Watcom from the Pacific C sources by John Elliott. The following 
changes have been made by me (Davide Bresolin):

* the number of icons in the workbench view is determined at runtime to adapt
  to the actual resolution of the screen. The memory needed for the icons and
  objects is dynamically allocated during the initialisation of the program.

* the .DFN files have been reconstructed, since they where missing from John
  Elliott's sources.

* New option '^Z at the end of file' in the Output preferences. With the option
  selected, RCS puts a Control-Z character at the end of application bindings
  and .RSH files, as expected by DOS programs. With the option deselected, 
  output files end with a newline, as expected by the Open Watcom compiler.
  
* Merge RCS files now works correctly.

Changes made by John Elliott to the original DR v2.3 for Metware High C:

* It will take advantage of checkboxes, radio buttons and 3D controls, if
  these are present in the AES.

* It supports all the object flags and states used in GPLed AES versions.

  This RCS has been tested with GEM/3 v3.13. It should work also with FreeGEM
and GEM/2.

  BE CAREFUL! There may well be bugs I haven't found. Take a backup before
editing files with this RCS. Save early and often. Tell me if you discover any
bugs.

Changes from previous build
===========================

i)  Detects GEM/5 and takes advantage of its 3D support.
ii) The captions "MENU" and "ALERT" in the Tree Type screen were transposed,
   and have now been fixed.

Notes on the source
===================

  The .DFN files (missing from the source release) have been reconstructed
and are included in this program.

  If you want to change anything in the resources, check that you have changed
both high- and low- resolution versions.