
#define DEBUG 0

#include <string.h>	/* strlen, strcpy etc. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#include "ppdgem.h"
#include "rclib.h"

typedef LPBYTE FAR *LPLSTR;
typedef LPBIT  FAR *LPLBIT;
typedef LPTREE FAR *LPLTREE;
typedef RSHDR  FAR *LPRSHDR;

#define NOTREE ((LPTREE)-1)

#include "deskdefs.h"
#include "deskapp.h"
#include "deskfpd.h"
#include "infodef.h"
#include "deskwin.h"
#include "deskbind.h"
#include "deskprot.h"
#include "deskvars.h"
#include "desktop.h"


