#ifndef ETL_HEAP_H
#define ETL_HEAP_H

#include "FreeRTOS.h"

namespace Project::etl {

    inline auto heapGetFreeSize() { return xPortGetFreeHeapSize(); }

    inline auto heapGetMinimumEverFreeSize() { return xPortGetMinimumEverFreeHeapSize(); }
}

#endif