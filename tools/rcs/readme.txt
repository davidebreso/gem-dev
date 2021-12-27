GEM RCS: version 2.3 GPL for Open Watcom bindings
=================================================

  This version of RCS (the Resource Construction Set) has been ported to
Open Watcom from the Pacific C sources by John Elliott. Compared to the 
original DR v2.3 for Metware High C, the following changes have been made:

* It will take advantage of checkboxes, radio buttons and 3D controls, if
  these are present in the AES.

* It supports all the object flags and states used in GPLed AES versions.

  This RCS has been tested with GEM/2 v2.0 and with FreeGEM 3.0 (current
development builds).

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