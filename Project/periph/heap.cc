#include "FreeRTOS.h"

void *operator new(size_t size) { return pvPortMalloc(size); }
void operator delete(void *ptr) { vPortFree(ptr); }
void operator delete(void *ptr, size_t) { vPortFree(ptr); }