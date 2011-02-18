#ifndef __STRINGUTIL_H
#define __STRINGUTIL_H

/* apstring Utility Functions
 * Last Update: 12/31/2007 10:05:19 AM
 * Austin Heilman
 *
 */

#include "apstring.h"
#include "apvector.h"

// Constants for the RemoveSpaces() function.
#define CLR_LEADING_SPACES	0x1
#define CLR_TRAILING_SPACES	0x2
#define CLR_DOUBLE_SPACES	0x4
#define CLR_ALL_SPACES		0x8

// Constants for the RemoveFromString() function.
#define STR_DEL_LEADING		0x1
#define STR_DEL_TRAILING		0x2
#define STR_DEL_ALL			0x4
#define STR_DEL_ONCE		0x8

typedef apvector<apstring> StringArray;


StringArray SplitWords(apstring str, apstring delim=" ");
apstring RemoveSpaces(apstring str, unsigned flags=0);

#endif // __STRINGUTIL_H