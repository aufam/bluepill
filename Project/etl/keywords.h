// #ifndef ETL_KEYWORDS_H
// #define ETL_KEYWORDS_H

/*
 * warning!!!
 * - using custom keywords is generally considered as bad practice
 * - custom keywords may conflict with existing source codes or libraries
 * - don't put this into a header file (hence, no header guard)
 * - don't put this before other header files
 * - define ETL_DISABLE_MORE_KEYWORDS to disable non-standard keywords
 */

#include "etl/utility.h" // pair, triple, len, next, iter, enumerate, zip, range
#include <ciso646>       // or, and, not, not_eq


#ifndef ETL_DISABLE_MORE_KEYWORDS

#define in :            // python's in (only for iterating a container)
#define elif else if    // python's elif
#define is ==           // python's is (should be used only for comparing a variable to a literal)
#define is_not !=

#define null nullptr    // kotlin's null
#define var auto        // kotlin's var
#define val const auto  // kotlin's val
#define fun auto        // kotlin's fun (should be used to define a function)

#define lambda []       // capture-less lambda expression
#define _ dummy__[[maybe_unused]] // unused variable

#endif // ETL_DISABLE_MORE_KEYWORDS
// #endif //ETL_KEYWORDS_H
