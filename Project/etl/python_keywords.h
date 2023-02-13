#ifndef ETL_PYTHON_KEYWORDS_H
#define ETL_PYTHON_KEYWORDS_H

/* not all keywords are exactly like python's */

#include <ciso646> // or, and, not
#define in :
#define elif else if
#define is ==
#define is_not !=

#include "utility.h" // len, next, enumerate, zip, range

#endif //ETL_PYTHON_KEYWORDS_H
