#ifndef ETL_PYTHON_KEYWORDS_H
#define ETL_PYTHON_KEYWORDS_H

/*
 * warning!!!
 * - not all keywords are exactly like python's
 * - don't put this to a header file
 * */

#include <ciso646> // or, and, not
#define in :
#define elif else if
#define is ==
#define is_not !=

#include "utility.h" // len, next, enumerate, zip, range

#endif //ETL_PYTHON_KEYWORDS_H
