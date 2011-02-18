#ifndef __FILEUTIL_H
#define __FILEUTIL_H

/* File Utility Functions
 * Austin Heilman
 * Last Updated: 10/13/2007
 *
 */

#include <dos.h>

#include "apvector.h"
#include "apstring.h"

struct FileFound
{
	public:
		char attribute;
		apstring name;
		apstring path;

		short depth;
		unsigned short time;
		unsigned short date;
		unsigned long size;
};
typedef apvector<FileFound> FilesFound;

FilesFound GetFilesInDir(apstring path, apstring filename="*.*", int attribute=_A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_VOLID | _A_SUBDIR | _A_ARCH);

#endif // __FILEUTIL_H