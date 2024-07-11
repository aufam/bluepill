#include "main.hpp"
#include "etl/heap.h"
#include "etl/keywords.h"

[[async]]
static void heap_info() {
    for (;;) {
        etl::this_thread::sleep(100ms);
        auto lock = await | mutex.lock();
        
        oled.setCursor({0, 0});
        oled << f("h: %lu/%lu t: %lu\n", etl::heap::freeSize.get(), etl::heap::totalSize.get(), tasks.resources());
    }
}

APP(heap_info) {
    etl::async(&heap_info);
}

