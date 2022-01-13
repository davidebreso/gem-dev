Notes on Open Watcom and RSCREATE:

RSCREATE generates a .RSC binary resource file from a .RSH source file for the 
resources. 

The program allocates a block of memory big enough to contain all resource data,
then copies the data structures defined by the .RSH file in the correct order, 
populates the header of the resource file and "fixes" the resource data
structures by converting references in memory into offsets in the file.
The content of the memory block is then dumped into the binary .RSC file.

The current version is able to recreate resource files for the Desktop and for 
the Resource Construction Set that are almost identical to the original, except 
for a few bytes. The recreated files do work

The code works only in the large memory model.

