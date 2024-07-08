#include "main.hpp"
#include "etl/heap.h"

using namespace Project;
using namespace Project::etl::literals;

[[async]]
static void heap_info() {
    for (;;) {
        etl::this_thread::sleep(100ms);
        auto lock = mutex.lock().await();
        
        oled.setCursor({0, 0});
        oled << f("h: %lu/%lu t: %lu\n", etl::heap::freeSize.get(), etl::heap::totalSize.get(), tasks.resources());
        oled.setCursor({0, 4});
    }
}

#include <list>

APP(heap_info) {
    etl::async(&heap_info);
}

