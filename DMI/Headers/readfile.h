#ifndef __READFILE_H
#define __READFILE_H

#include <fstream.h>
#include "apstring.h"
#include "apvector.h"

apvector<apstring> ReadFile(const char* filename);

#endif // __READFILE_H
