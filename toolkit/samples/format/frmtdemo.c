#include <stdio.h>
#include <stdlib.h>

/* Format a disk
 * Paramters:
 *   FDrive% = 1 = a:, 2 = b:, can't format hard drive
 *   FSize% = 0 = 360k, 1 = 1.2m, 2 = 720k, 3 = 1.44m
 *   Verb% = not(0) Doesn't print anything, 0 prints status, etc. */
void _FrmtDisk(int FDrive, int FSize, int Verb);

const char *size_desc[] = {"360k", "1.2m", "720k", "1.44m"};

int main(int argc, char *argv[]) {
    int FDrive, FSize, Verb;

    if(argc != 4) {
        printf("Usage: %s FDrive FSize Verb\n", argv[0]);
        printf("\tFDrive = 1 = a:, 2 = b:, can't format hard drive\n");
        printf("\tFSize = 0 = 360k, 1 = 1.2m, 2 = 720k, 3 = 1.44m\n");
        printf("\tVerb != 0 Doesn't print anything, 0 prints status, etc.\n");
        return 1;
    }

    FDrive=atoi(argv[1]);
    if(FDrive < 1 || FDrive > 2) {
        printf("Invalid drive number!\n\tFDrive = 1 = a:, 2 = b:, can't format hard drive\n");
        return 1;
    }
    FSize=atoi(argv[2]);
    if(FSize < 0 || FSize > 3) {
        printf("Invalid size!\n\tFSize = 0 = 360k, 1 = 1.2m, 2 = 720k, 3 = 1.44m\n");
        return 1;
    }
    Verb=atoi(argv[3]);
    
    printf("Formatting a %s floppy in drive %c %s\n", size_desc[FSize], FDrive+'@', (Verb ? "silently" : "verbosely"));
    _FrmtDisk(FDrive, FSize, Verb);
    
    return 0;
}
