/* Some more types for OW, to make use of its native support for long 
 * pointers, and to promote type checking
 */

#define FAR  far
#define NEAR near

#ifdef __WATCOMC__
typedef VOID FAR   *LPVOID;
typedef LPVOID FAR *LPLPTR;
typedef BYTE FAR   *LPBYTE;
typedef WORD FAR   *LPWORD;
typedef LONG FAR   *LPLONG;
typedef UWORD FAR  *LPUWORD;
#else
#define LPVOID LONG
#define LPLPTR LONG
#define LPBYTE LONG
#define LPWORD LONG
#define LPLONG LONG
#define LPUWORD LONG
#endif

#ifdef __LARGE__
#include "lmachine.h"
#else
#include "smachine.h"
#endif

