#ifndef ETL_MAP_H
#define ETL_MAP_H

#include "etl/tuple.h"
#include "etl/vector.h"

namespace Project::etl {

    /// collection of key-value pairs, keys are unique
    template <class K, class V>
    struct Map : Vector<Pair<K, V>> {
        typedef K Key;
        typedef V Value;

        bool hasKey(const K& key) {
            for (auto& [x, y] : *this) if (x == key) return true;
            return false;
        }

        V& operator[] (const Key& key) {
            for (auto& [x, y] : *this) if (x == key) return y;
            this->append(Pair<K,V> { key, Value {}});
            return this->back().y;
        }

        const V& operator[] (const Key& key) const {
            for (auto& [x, y] : *this) if (x == key) return y;
            return Value {};
        }
    };
}

#endif //ETL_MAP_H
