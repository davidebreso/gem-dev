RSCONV - Source for Open Watcom bindings
========================================

  These files are the source for an Intel port of Tim Oren's RSCONV (which
converts .RSC files between Atari and Intel formats). They have been
designed to compile with Open Watcom v2.

  The following changes were made to the source:

* Changes to account for the native resource format being Intel rather
 than 68000.
* Rather than allocate all of memory for transformations, the program
 allocates only what is needed.
* All file I/O is done with fopen/fread/fwrite/fclose.
* Resources redrawn for optimal appearance under AESes that support 3D
 controls.

